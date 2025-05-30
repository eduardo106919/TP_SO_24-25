
#include "utils.h"


#include <stdio.h>
#include <sys/stat.h>



int create_fifo(const char *name) {
    struct stat st;
    // check if fifo exists
    if (stat(name, &st) == 0) {
        printf("fifo %s already exists\n", name);
        return 1;
    }

    // create fifo
    if (mkfifo(name, 0600) == -1) {
        perror("mkfifo()");
        return 2;
    }

    return 0;
}

