
#include "utils.h"
#include "defs.h"
#include "server_ops.h"
#include "cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


static void usage(const char *command) {
    printf("Usage:\n");
    printf("%s [-g] document_folder cache_size [cache_type]\n", command);
}


int main(int argc, char **argv) {

    // not enough arguments
    if (argc < 3) {
        usage(argv[0]);
        return 0;
    }

    // create server fifo
    if (create_fifo(SERVER_FIFO) != 0) {
        printf("Error creating fifo %s\n", SERVER_FIFO);
        return 2;
    }

    // start the server (open files, create data structures, ...)
    Server * server = start_server(argv[1], atoi(argv[2]), NONE);

    Request request;
    int stop = 0, input = 0;
    ssize_t out = 0;


    while (stop == 0) {
        // open the server fifo
        input = open(SERVER_FIFO, O_RDONLY);
        if (input == -1) {
            perror("open()");
            break;
        }

        // receive request from client
        out = read(input, &request, sizeof(request));
        close(input);

        if (out == -1) {
            perror("read()");
            break;
        }

        // process request
        stop = process_request(server, &request);

    }

    // shut down the server (close files, free data structures, ...)
    shutdown_server(server);



    return 0;
}