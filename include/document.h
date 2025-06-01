#ifndef DOCUMENT_H
#define DOCUMENT_H



typedef struct document {
    unsigned id;
    char title[TITLE_SIZE];
    char authors[AUTHORS_SIZE];
    char year[YEAR_SIZE];
    char path[PATH_SIZE];
} Document;



Document * create_document(unsigned id, const char *title, const char *authors, const char *year, const char *path);
void destroy_document(Document * doc);
Document * clone_document(const Document * doc);

unsigned get_document_id(const Document * doc);

void set_global_id(unsigned id);

void show_document(const Document * doc);

#endif