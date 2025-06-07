
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

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

int count_keyword(const char *path, const char *keyword) {
    if (path == NULL || keyword == NULL) {
        return -1;
    }

    // open the comunication channels
    int fildes[2];
    if (pipe(fildes) == -1) {
        perror("pipe()");
        return -1;
    }

    pid_t proc = fork();

    if (proc == -1) {
        /* error code */
        perror("fork()");
        return -1;
    } else if (proc == 0) {
        /* child code */

        // close reading side of the pipe
        close(fildes[0]);

        // redirect stdout to the writing side of the pipe
        dup2(fildes[1], 1);
        close(fildes[1]);

        // count the keyword
        execlp("grep", "grep", "-c", keyword, path, NULL);

        perror("grep -c");
        _exit(127);
    }

    close(fildes[1]);

    char buffer[10];
    // receive the count
    ssize_t out = read(fildes[0], buffer, sizeof(buffer));
    close(fildes[0]);
    if (out == -1) {
        perror("read()");
        return -1;
    }

    // wait for the child process
    if (waitpid(proc, NULL, 0) == -1) {
        perror("waitpid()");
        return -1;
    }

    return atoi(buffer);
}

char *join_paths(const char *folder, const char *file) {
    if (folder == NULL || file == NULL) {
        return NULL;
    }

    size_t folder_len = strlen(folder);
    size_t file_len = strlen(file);
    size_t total = folder_len + file_len;

    char result[total + 10];

    if (folder[folder_len - 1] != '/') {
        sprintf(result, "%s/%s", folder, file);
    } else {
        sprintf(result, "%s%s", folder, file);
    }

    return strdup(result);
}

int keyword_exists(const char *path, const char *keyword) {
    if (path == NULL || keyword == NULL) {
        return -1;
    }

    int status = -1, out = -1;
    pid_t proc = fork();

    switch (proc) {
        case -1:
            /* error code */
            perror("fork()");
            return -1;
        case 0:
            /* child code */

            execlp("grep", "grep", "-q", keyword, path, NULL);

            perror("execlp()");
            _exit(127);
        default:
            /* parent code */

            proc = waitpid(proc, &status, 0);

            if (WIFEXITED(status) == 1) {
                out = WEXITSTATUS(status);
            }

            break;
    }

    return out;
}