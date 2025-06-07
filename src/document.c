
#include "document.h"
#include "defs.h"

#include <stdlib.h>
#include <string.h>

Document *create_document(const char *title, const char *authors,
                          const char *year, const char *path) {
    Document *doc = (Document *)calloc(1, sizeof(Document));
    if (doc == NULL) {
        return NULL;
    }

    strcpy(doc->title, title);
    strcpy(doc->authors, authors);
    strcpy(doc->year, year);
    strcpy(doc->path, path);

    return doc;
}

void destroy_document(Document *doc) {
    if (doc != NULL) {
        free(doc);
    }
}

Document *clone_document(const Document *doc) {
    if (doc == NULL) {
        return NULL;
    }

    return create_document(doc->title, doc->authors, doc->year, doc->path);
}

void show_document(const Document *doc) {
    if (doc != NULL) {
        printf("Title: %s\n", doc->title);
        printf("Authors: %s\n", doc->authors);
        printf("Year: %s\n", doc->year);
        printf("Path: %s\n", doc->path);
    }
}
