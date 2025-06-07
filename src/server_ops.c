#include "server_ops.h"
#include "free_list.h"
#include "index_table.h"
#include "document.h"
#include "utils.h"
#include "defs.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef struct server {
    char * document_folder;
    int metadata_file;
    int requests_log_pipe;
    Free_List * free_list;
    Index_Table * index_table;
} Server;


static void record_requests(int reading_side) {

    int file = open(REQUESTS_LOG, O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (file == -1) {
        perror("open()");
        return;
    }

    ssize_t out = 0;
    Request temp;
    char buffer[BUFSIZ];
    char op;
    time_t t;
    struct tm *tm_info;
    char temp_time[20];
    char args[512];

    // read messages from the server
    while ((out = read(reading_side, &temp, sizeof(temp))) > 0) {
        // Get the current time
        time(&t);

        // Convert it to local time representation
        tm_info = localtime(&t);

        strftime(temp_time, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        switch (temp.operation) {
        case INDEX:
            op = 'A';
            sprintf(args, "%s %s %s %s", temp.title, temp.authors, temp.year, temp.path);
            break;
        case REMOVE:
            op = 'D';
            sprintf(args, "%s", temp.title);
            break;
        case CONSULT:
            op = 'C';
            sprintf(args, "%s", temp.title);
            break;
        case COUNT_WORD:
            op = 'L';
            sprintf(args, "%s %s", temp.title, temp.authors);
            break;
        case LIST_WORD:
            op = 'S';
            sprintf(args, "%s %s", temp.title, temp.authors);
            break;
        case KILL:
            op = 'K';
            memset(args, 0, sizeof(args));
            break;
        case SHUTDOWN:
            op = 'F';
            memset(args, 0, sizeof(args));
            break;
        default:
            op = 'X';
            memset(args, 0, sizeof(args));
            break;
        }

        sprintf(buffer, "[%d] requested %c | args: %s | (%s)\n", temp.client, op, args, temp_time);
        out = write(file, buffer, strlen(buffer));
    }
}

Server * start_server(const char *document_folder, int cache_size) {
    printf("\n[SERVER IS STARTING]\n");

    Server * server = (Server *) calloc(1, sizeof(Server));
    if (server == NULL) {
        return NULL;
    }

    server->document_folder = strdup(document_folder);
    if (server->document_folder == NULL) {
        free(server);
        return NULL;
    }

    // open metadata.bin
    server->metadata_file = open(STORAGE_FILE, O_CREAT | O_RDWR, 0666);
    if (server->metadata_file == -1) {
        perror("open()");
        free(server->document_folder);
        free(server);
        return NULL;
    }

    // open the control metadata file
    int control_file = open(CONTROL_FILE, O_CREAT | O_RDONLY, 0666);
    if (control_file == -1) {
        perror("open()");
        close(server->metadata_file);
        free(server->document_folder);
        free(server);
        return NULL;
    }

    // upload the free list from the control file
    server->free_list = fl_upload(control_file);
    if (server->free_list == NULL) {
        close(server->metadata_file);
        close(control_file);
        free(server->document_folder);
        free(server);
        return NULL;
    }

    // upload the index table from the control file
    server->index_table = it_upload(control_file);
    if (server->index_table == NULL) {
        close(server->metadata_file);
        close(control_file);
        free(server->document_folder);
        fl_destroy(server->free_list);
        free(server);
        return NULL;
    }

    close(control_file);
    unlink(CONTROL_FILE);

    int requests_pipe[2];
    if(pipe(requests_pipe) != 0) {
        perror("pipe()");
        shutdown_server(server);
        return NULL;
    }

    switch (fork()) {
    case -1:
        /* error code */
        perror("fork()");
        shutdown_server(server);
        return NULL;
    case 0:
        /* child code */
        close(requests_pipe[1]); // close writing side of the pipe
        close(server->metadata_file);

        record_requests(requests_pipe[0]);
        close(requests_pipe[0]); // close reading side of the pipe

        _exit(0);
    default:
        break;
    }

    close(requests_pipe[0]);
    server->requests_log_pipe = requests_pipe[1];

    // TODO
    // start the cache

    it_show(server->index_table);
    fl_show(server->free_list);

    printf("\n[SERVER IS ONLINE]\n");
    return server;
}


static void send_response(pid_t client, const void * response, size_t size) {
    char client_fifo[50];
    sprintf(client_fifo, "%s_%d", CLIENT_FIFO, client);

    // open the client fifo to send response
    int output = open(client_fifo, O_WRONLY);
    if (output == -1) {
        perror("open()");
        return;
    }

    // send response to client
    ssize_t out = write(output, response, size);
    if (out == -1) {
        perror("write()");
        return;
    }

    close(output);

    int server = open(SERVER_FIFO, O_WRONLY);
    if (server == -1) {
        perror("open()");
        return;
    }

    Request request;
    request.operation = KILL;
    request.client = getpid();
    memset(request.title, 0, sizeof(request.title));
    memset(request.authors, 0, sizeof(request.authors));
    memset(request.year, 0, sizeof(request.year));
    memset(request.path, 0, sizeof(request.path));

    // tell the server that the job is done
    out = write(server, &request, sizeof(request));
    close(server);
    if (out == -1) {
        perror("write()");
        return;
    }
}

static Document * get_document(Server * server, int identifier) {
    // check if the entry is valid
    if (it_entry_is_valid(server->index_table, identifier) == 0) {
        return NULL;
    }

    Document doc;

    // go to the right position
    if (lseek(server->metadata_file, identifier * sizeof(Document), SEEK_SET) == -1) {
        perror("lseek()");
        return NULL;
    }

    // read the metadata
    ssize_t out = read(server->metadata_file, &doc, sizeof(doc));
    if (out == -1) {
        perror("read()");
        return NULL;
    }

    return clone_document(&doc);
}


static char * list_documents(Server * server, const char * keyword, int n_procs) {

    // close the metadata file, so that processes don't share the offset
    close(server->metadata_file);

    // get the number of documents indexed
    int identifier = 0;
    ssize_t out = 0;
    Document *doc = NULL;
    // open the comunication channels
    int fildes[2];
    if (pipe(fildes) == -1) {
        perror("pipe()");
        return NULL;
    }
    
    // get the valid ids
    int * valid_ids = it_get_valid_ids(server->index_table);
    unsigned count = it_size(server->index_table);
    unsigned chunk = count / n_procs;
    int metatada = -1;
    char * path;

    if (n_procs > count) {
        n_procs = count;
    }

    for (int i = 0; i < n_procs; i++) {
        switch (fork()) {
        case -1:
            /* error code */
            perror("fork()");
            return NULL;
        case 0:
            /* child code */

            // close readind side of the pipe
            close(fildes[0]);

            identifier = i * chunk;

            // last process does the remaining ids
            if (i == n_procs - 1) {
                chunk += count % n_procs;
            }

            // open the metadata.bin file
            metatada = open(STORAGE_FILE, O_RDONLY);
            if (metatada == -1) {
                perror("open()");
                _exit(1);
            }

            count = 0;
            while (count < chunk) {

                // get document from cache or disk
                doc = get_document(server, valid_ids[identifier]);

                path = join_paths(server->document_folder, doc->path);

                // check if the keyword exists in the file
                out = keyword_exists(path, keyword);

                if (path != NULL) {
                    free(path);
                }

                if (out == 0) {
                    // send the id to the parent process
                    out = write(fildes[1], &(valid_ids[identifier]), sizeof(valid_ids[identifier]));
                }

                identifier++;
                count++;
            }

            _exit(0);
        default:
            /* parent code */
            break;
        }
    }

    close(fildes[1]);

    char buffer[BUFSIZ];
    char temp[12];

    strcat(buffer, "[");

    // receive the ids with the keyword
    while ((out = read(fildes[0], &identifier, sizeof(identifier))) > 0) {
        sprintf(temp, "%d, ", identifier);
        strcat(buffer, temp);
    }

    // terminate the string
    if (strlen(buffer) > 1) {
        buffer[strlen(buffer) - 2] = ']';
        buffer[strlen(buffer) - 1] = '\0';
    }

    // wait for the child processes
    for (int i = 0; i < n_procs; i++) {
        wait(NULL);
    }

    return strdup(buffer);
}



int process_request(Server * server, const Request *request) {
    int identifier = 0;
    off_t position = 0;
    Document * doc = NULL;
    int temp = 0;
    char result[BUFSIZ];
    ssize_t out;

    // record the request in the log file
    out = write(server->requests_log_pipe, request, sizeof(Request));
    if (out == -1) {
        perror("write()");
        return -1;
    }

    switch (request->operation) {
    case INDEX:
        /* index document */

        if (fl_is_empty(server->free_list) != 0) {
            // empty list, append to the file
            position = lseek(server->metadata_file, 0, SEEK_END);
            identifier = position / sizeof(Document);
        } else {
            identifier = fl_pop(server->free_list);
            // go to the empty spot
            lseek(server->metadata_file, identifier * sizeof(Document), SEEK_SET);
        }

        // create document
        doc = create_document(request->title, request->authors, request->year, request->path);
        if (doc == NULL) {
            return -1;
        }

        // write the document in the metadata file
        out = write(server->metadata_file, doc, sizeof(Document));
        if (out == -1) {
            perror("write()");
            return -1;
        }

        destroy_document(doc);

        // add entry to the index table
        if (it_add_entry(server->index_table, identifier) != 0) {
            return -1;
        }

        switch (fork()) {
            case -1:
                perror("fork()");
                return 1;
            case 0:
                send_response(request->client, &identifier, sizeof(identifier));
                _exit(0);
            default:
                break;
        }

        break;

    case REMOVE:
        /* remove index */

        identifier = atoi(request->title);

        // remove entry from table
        temp = it_remove_entry(server->index_table, identifier);

        if (temp != -1) {
            // add free id to free list
            fl_push(server->free_list, identifier);
        } else {
            // document not found
            identifier = -1;
        }

        switch (fork()) {
            case -1:
                perror("fork()");
                return 1;
            case 0:
                send_response(request->client, &identifier, sizeof(identifier));
                _exit(0);
            default:
                break;
        }

        break;
    
    case CONSULT:
        /* consult a document */

        identifier = atoi(request->title);

        switch (fork()) {
            case -1:
                perror("fork()");
                return 1;
            case 0:

                close(server->metadata_file);
                // re-open the file, so the offset is not shared
                server->metadata_file = open(STORAGE_FILE, O_RDONLY);
                if (server->metadata_file == -1) {
                    perror("open()");
                    _exit(1);
                }

                // get the document (from cache or disk)
                doc = get_document(server, identifier);

                // document was not found
                if (doc == NULL) {
                    doc = malloc(sizeof(Document));
                    sprintf(doc->title, "Document was not found");
                }

                send_response(request->client, doc, sizeof(Document));
                _exit(0);
            default:
                break;
        }

        destroy_document(doc);

        break;

    case COUNT_WORD:
        /* count keyword */

        switch (fork()) {
            case -1:
                perror("fork()");
                return 1;
            case 0:

                identifier = atoi(request->title);

                // get the document (from cache or file)
                doc = get_document(server, identifier);

                int count = -1;
                if (doc != NULL) {
                    char *path = join_paths(server->document_folder, doc->path);
                    // count the number of lines
                    count = count_keyword(path, request->authors);

                    if (path != NULL) {
                        free(path);
                    }
                }

                send_response(request->client, &count, sizeof(count));
                _exit(0);
            default:
                break;
        }

        break;

    case LIST_WORD:
        /* identify the documents that contain the keyword */

        switch (fork()) {
        case -1:
            /* error code */
            perror("fork()");
            return -1;
        case 0:

            // get the number of processes
            int n_procs = atoi(request->authors);

            // get the list of ids
            char * other = list_documents(server, request->title, n_procs);
            if (other != NULL) {
                strcpy(result, other);
                free(other);
            } else {
                sprintf(result, "Error searching the documents!!");
            }

            send_response(request->client, result, strlen(result) + 1);
            _exit(0);
        default:
            break;
        }

        break;

    case KILL:
        /* wait for child process */

        // NEEDS IMPROVEMENTS, TEMPORARY
        waitpid(request->client, NULL, 0);

        break;
    case SHUTDOWN:
        /* shutdown the server */

        identifier = atoi(request->title);

        return 1;

        break;

    default:
        break;
    }




    return 0;
}




void shutdown_server(Server * server) {
    printf("\n[SERVER IS SHUTTING DOWN]\n");

    if (server->document_folder != NULL) {
        free(server->document_folder);
    }

    it_show(server->index_table);
    fl_show(server->free_list);

    // close the metadata file
    close(server->metadata_file);

    // open the control file to record auxiliar data structures
    int control_file = open(CONTROL_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (control_file == -1) {
        perror("open()");
        return;
    }

    // record the auxiliar dada structures
    fl_record(server->free_list, control_file);
    it_record(server->index_table, control_file);

    close(control_file);

    // free the data structures
    fl_destroy(server->free_list);
    it_destroy(server->index_table);

    close(server->requests_log_pipe);

    // TODO
    // FREE THE CACHE

    
    close(server->metadata_file);
    free(server);

    unlink(SERVER_FIFO);

    printf("\n[SERVER IS OFFLINE]\n");
}
