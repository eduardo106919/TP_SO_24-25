
#include "client_ops.h"
#include "document.h"

#include <string.h>


Operation check_operation(const char *opr) {
    if (strlen(opr) != 2) {
        return -1;
    }

    Operation result = -1;

    switch (opr[1]) {
    case 'a':
        result = INDEX;
        break;
    case 'd':
        result = REMOVE;
        break;
    case 'c':
        result = CONSULT;
        break;
    case 'l':
        result = COUNT_WORD;
        break;
    case 's':
        result = LIST_WORD;
        break;
    case 'f':
        result = SHUTDOWN;
        break;
    default:
        break;
    }

    return result;
}

int define_request(Request *request, int argc, char **argv) {
    request->operation = check_operation(argv[1]);
    if (request->operation == -1) {
        return 1;
    }

    switch (request->operation) {
    case INDEX:
        if (argc != 6) {
            return 1;
        }

        strcpy(request->title, argv[2]);
        strcpy(request->authors, argv[3]);
        strcpy(request->year, argv[4]);
        strcpy(request->path, argv[5]);

        break;
    case REMOVE:
        if (argc != 3) {
            return 1;
        }

        strcpy(request->title, argv[2]);

        break;
    case CONSULT:
        if (argc != 3) {
            return 1;
        }

        strcpy(request->title, argv[2]);

        break;
    case COUNT_WORD:
        if (argc != 4) {
            return 1;
        }

        strcpy(request->title, argv[2]);
        strcpy(request->authors, argv[3]);

        break;
    case LIST_WORD:
        if (argc != 3 && argc != 4) {
            return 1;
        }

        strcpy(request->title, argv[2]);
        if (argc == 4) {
            strcpy(request->authors, argv[3]);
        } else {
            strcpy(request->authors, "1\0");
        }

        break;
    default:
        break;
    }

    request->client = getpid();
    return 0;
}


void show_reply(Operation op, const void * reply) {

    switch (op) {
    case INDEX:
        printf("Document %u indexed\n", * (unsigned *) reply);

        break;
    case REMOVE:
        int identifier = * (int *) reply;

        if (identifier == -1) {
            printf("Document was not found\n");
        } else {
            printf("Index entry %u deleted\n", identifier);
        }

        break;
    case CONSULT:
        Document * doc = (Document *) reply;
        char *not_found_msg = "Document was not found";
        if (strcmp(doc->title, not_found_msg) == 0) {
            printf("%s\n", not_found_msg);
        } else {
            show_document(doc);
        }

        break;
    case COUNT_WORD:

        int count = * (int *) reply;

        if (count == -1) {
            printf("Document was not found\n");
        } else {
            printf("Count: %d\n", count);
        }
        break;
    case LIST_WORD:

        printf("IDs: %s\n", (char *) reply);

        break;
    default:
        break;
    }

}
