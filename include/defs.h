#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <unistd.h>


#define SERVER_FIFO "tmp/server_fifo"
#define CLIENT_FIFO "tmp/client_fifo"

#define STORAGE_FILE "tmp/metadata.bin"
#define CONTROL_FILE "tmp/metadata_control.bin"

#define TITLE_SIZE 200
#define AUTHORS_SIZE 200
#define YEAR_SIZE 4
#define PATH_SIZE 64


typedef enum {
    INDEX,
    REMOVE,
    CONSULT,
    COUNT_WORD,
    LIST_WORD,
    SHUTDOWN,
    KILL,
    NONE
} Operation;



typedef struct {
    pid_t client;
    Operation operation;
    char title[TITLE_SIZE];
    char authors[AUTHORS_SIZE];
    char year[YEAR_SIZE];
    char path[PATH_SIZE];
} Request;



typedef struct {
    Operation operation;
    char response[BUFSIZ];
    char valid;
} Reply;



#endif