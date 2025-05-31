
#include "defs.h"
#include "document.h"

#include <stdlib.h>
#include <string.h>



static unsigned global_ids = 1;




Document * create_document(unsigned id, const char *title, const char *authors, const char *year, const char *path) {
    Document * doc = (Document *) calloc(1, sizeof(Document));
    if (doc == NULL) {
        return NULL;
    }

    if (id == 0) {
        id = global_ids++;
    }

    doc->id = id;

    strcpy(doc->title, title);
    strcpy(doc->authors, authors);
    strcpy(doc->year, year);
    strcpy(doc->path, path);

    return doc;
}

void destroy_document(Document * doc) {
    if (doc != NULL) {
        free(doc);
    }
}

Document * clone_document(const Document * doc) {
    if (doc == NULL) {
        return NULL;
    }

    Document * temp = (Document *) calloc(1, sizeof(Document));

    temp->id = doc->id;

    strcpy(temp->title, doc->title);
    strcpy(temp->authors, doc->authors);
    strcpy(temp->year, doc->year);
    strcpy(temp->path, doc->path);

    return temp;
}


unsigned get_document_id(const Document * doc) {
    if (doc != NULL) {
        return doc->id;
    }
    
    return 0;
}

void set_global_id(unsigned id) {
    global_ids = id;
}
