#include "server_ops.h"
#include "free_list.h"
#include "index_table.h"
#include "document.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct server {
    char *document_folder;
    int metadata_file;
    Free_List * free_list;
    Index_Table * index_table;
} Server;


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

    set_global_id(it_size(server->index_table) + 1);

    // TODO
    // start the cache

    it_show(server->index_table);
    fl_show(server->free_list);

    printf("\n[SERVER IS ONLINE]\n");
    return server;
}


static void send_response(pid_t client, const char * response, Operation operation) {

    Reply reply;

    reply.operation = operation;
    reply.valid = 1;
    strcpy(reply.response, response);
    
    char client_fifo[50];
    sprintf(client_fifo, "%s_%d", CLIENT_FIFO, client);

    // open the client fifo to send response
    int output = open(client_fifo, O_WRONLY);
    if (output == -1) {
        perror("open()");
        return;
    }

    // send response
    ssize_t out = write(output, &reply, sizeof(reply));
    if (out == -1) {
        perror("write()");
        return;
    }

    close(output);
}



int process_request(Server * server, const Request *request) {
    unsigned identifier = 0;
    off_t position = 0;
    char response[50];
    
    switch (request->operation) {
    case INDEX:
        /* index document */

        position = fl_pop(server->free_list, &identifier);
        
        if (position == -1) {
            // empty list, append to the file
            position = lseek(server->metadata_file, 0, SEEK_END);
        } else {
            // go to the empty spot
            lseek(server->metadata_file, position, SEEK_SET);
        }

        // create document
        Document * doc = create_document(identifier, request->title, request->authors, request->year, request->path);
        if (doc == NULL) {
            return -1;
        }

        // write the document in the metadata file
        ssize_t out = write(server->metadata_file, doc, sizeof(Document));
        if (out == -1) {
            perror("write()");
            return -1;
        }

        identifier = get_document_id(doc);
        destroy_document(doc);

        // add entry to the index table
        if (it_add_entry(server->index_table, position, identifier) != 0) {
            return -1;
        }

        sprintf(response, "%u", identifier);

        send_response(request->client, response, request->operation);

        break;

    case REMOVE:
        /* remove index */

        identifier = atoi(request->title);

        // remove entry from table
        position = it_remove_entry(server->index_table, identifier);

        if (position != -1) {
            // add free position and id to free list
            fl_push(server->free_list, position, identifier);
        }

        sprintf(response, "%u", identifier);

        send_response(request->client, response, request->operation);

        break;
    
    case SHUTDOWN:
        /* shutdown the server */

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


    // TODO
    // FREE THE CACHE

    
    close(server->metadata_file);
    free(server);

    unlink(SERVER_FIFO);

    printf("\n[SERVER IS OFFLINE]\n");
}
