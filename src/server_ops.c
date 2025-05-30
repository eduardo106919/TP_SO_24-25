#include "server_ops.h"
#include "free_list.h"
#include "index_table.h"

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


    // TODO
    // start the cache


    return server;
}


static void send_response(const Request *request) {

    Reply reply;

    reply.operation = request->operation;
    reply.valid = 1;
    strcpy(reply.responde, "hello from server");

    char client_fifo[50];
    sprintf(client_fifo, "%s_%d", CLIENT_FIFO, request->client);

    int output = open(client_fifo, O_WRONLY);
    if (output == -1) {
        perror("open()");
        return;
    }

    ssize_t out = write(output, &reply, sizeof(reply));
    if (out == -1) {
        perror("write()");
        return;
    }
}

int process_request(Server * server, const Request *request) {
    
    printf("client: %d\n", request->client);
    printf("operation: %d\n", request->operation);
    printf("title: %s\n", request->title);
    printf("authors: %s\n", request->authors);
    printf("year: %s\n", request->year);
    printf("path: %s\n", request->path);


    send_response(request);

    return 0;
}

void shutdown_server(Server * server) {
    if (server->document_folder != NULL) {
        free(server->document_folder);
    }

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


    free(server);
}
