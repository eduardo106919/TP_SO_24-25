#ifndef CACHE_H
#define CACHE_H

#include "document.h"



typedef struct cache Cache;


typedef enum {
    FIFO,
    RAND,
    LRU,
    NONE
} Cache_Type;


Cache * cache_start(int cache_size, Cache_Type type, int source);
void cache_destroy(Cache * cache);
Document * cache_get_document(Cache * cache, int identifier);
void show_cache(const Cache * cache);


#endif