#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "defs.h"


typedef struct document {
    char title[TITLE_SIZE];
    char authors[AUTHORS_SIZE];
    char year[YEAR_SIZE];
    char path[PATH_SIZE];
} Document;


Document * create_document(const char *title, const char *authors, const char *year, const char *path);
void destroy_document(Document * doc);
Document * clone_document(const Document * doc);
void show_document(const Document * doc);

#endif