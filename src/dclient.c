
#include "defs.h"
#include "utils.h"
#include "client_ops.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


static void usage(const char *command) {
    printf("Usage:\n");
    printf("%s -a 'title' 'authors' 'year' 'path'\n", command);
    printf("%s -d 'key'\n", command);
    printf("%s -c 'key'\n", command);
    printf("%s -l 'key' 'keyword'\n", command);
    printf("%s -s 'keyword' [nr_processes]\n", command);
    printf("%s -f\n", command);
}


int main(int argc, char **argv) {
    // no arguments
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    Request request;
    // validate user input
    if (define_request(&request, argc, argv) != 0) {
        printf("Invalid input\n");
        usage(argv[0]);
        return 1;
    }

    char fifo_name[50];
    sprintf(fifo_name, "%s_%u", CLIENT_FIFO, getpid());

    // create client fifo
    if (create_fifo(fifo_name) != 0) {
        printf("Error creating fifo %s\n", fifo_name);
        return 2;
    }

    // open the server fifo
    int server = open(SERVER_FIFO, O_WRONLY);
    if (server == -1) {
        perror("open()");
        return 2;
    }

    // send request to server
    ssize_t out = write(server, &request, sizeof(request));
    close(server);

    if (out == -1) {
        perror("write()");
        return 2;
    }

    int client = open(fifo_name, O_RDONLY);
    if (client == -1) {
        perror("open()");
        return 2;
    }

    Reply reply;

    // receive response from server
    out = read(client, &reply, sizeof(reply));
    close(client);

    if (out == -1) {
        perror("read()");
        return 2;
    }

    unlink(fifo_name);


    // show response to user
    show_reply(&reply);

    

    return 0;
}