
#include "cache.h"
#include "defs.h"
#include "server_ops.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *command) {
    printf("Usage:\n");
    printf("%s document_folder cache_size [-g] [cache_type]\n", command);
}

int main(int argc, char **argv) {

    srand(time(0));

    // not enough arguments
    if (argc < 3) {
        usage(argv[0]);
        return 0;
    }

    // determine the cache type
    Cache_Type type = NONE;
    if (strcmp(argv[argc - 1], "FIFO") == 0) {
        type = FIFO;
    } else if (strcmp(argv[argc - 1], "RAND") == 0) {
        type = RAND;
    } else if (strcmp(argv[argc - 1], "LRU") == 0) {
        type = LRU;
    }

    // turn off debugging messages
    if ((strcmp(argv[argc - 1], "-g") == 0) || (strcmp(argv[argc - 2], "-g") == 0)) {
        int trash = open("/dev/null", O_WRONLY);
        if (trash == -1) {
            perror("open()");
            return 1;
        }

        dup2(trash, 1);
        close(trash);
    }

    // create server fifo
    if (create_fifo(SERVER_FIFO) != 0) {
        printf("Error creating fifo %s\n", SERVER_FIFO);
        return 2;
    }

    // start the server (open files, create data structures, ...)
    Server *server = start_server(argv[1], atoi(argv[2]), type);

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