#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "document.h"

typedef struct lru LRU_Cache;

void *lruc_create(int cache_size, int source);
void lruc_destroy(void *cache);
Document *lruc_get_document(void *cache, int identifier);
void lruc_show(const void *cache);

#endif