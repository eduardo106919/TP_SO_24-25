#ifndef RAND_CACHE_H
#define RAND_CACHE_H

#include "document.h"

typedef struct rand RAND_Cache;

void *randc_create(int cache_size, int source);
void randc_destroy(void *cache);
Document *randc_get_document(void *cache, int identifier);
void randc_show(const void *cache);

#endif