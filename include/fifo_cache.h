#ifndef FIFO_CACHE_H
#define FIFO_CACHE_H

#include "document.h"

typedef struct fifo FIFO_Cache;

void *fifoc_create(int cache_size, int source);
void fifoc_destroy(void *cache);
Document *fifoc_get_document(void *cache, int identifier);
void fifoc_show(const void *cache);

#endif