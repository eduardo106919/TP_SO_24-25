

#include "cache.h"
#include "fifo_cache.h"
#include "lru_cache.h"
#include "rand_cache.h"

#include <stdlib.h>

typedef struct cache {
    Cache_Type type;
    void *cache;
    void *(*create)(int, int);
    void (*destroy)(void *);
    Document *(*get_doc)(void *, int);
    void (*add_doc)(void *, int, Document *);
    void (*remove_doc)(void *, int);
    void (*show)(const void *);
} Cache;

Cache *cache_start(int cache_size, Cache_Type type, int source) {
    Cache *cache = (Cache *)calloc(1, sizeof(Cache));
    if (cache == NULL) {
        return NULL;
    }

    switch (type) {
        case FIFO:
            cache->create = &fifoc_create;
            cache->destroy = &fifoc_destroy;
            cache->get_doc = &fifoc_get_document;
            cache->add_doc = &fifoc_add_document;
            cache->remove_doc = &fifoc_remove_document;
            cache->show = &fifoc_show;

            break;
        case RAND:
            cache->create = &randc_create;
            cache->destroy = &randc_destroy;
            cache->get_doc = &randc_get_document;
            cache->add_doc = &randc_add_document;
            cache->remove_doc = &randc_remove_document;
            cache->show = &randc_show;

            break;
        case LRU:
            cache->create = &lruc_create;
            cache->destroy = &lruc_destroy;
            cache->get_doc = &lruc_get_document;
            cache->add_doc = &lruc_add_document;
            cache->remove_doc = &lruc_remove_document;
            cache->show = &lruc_show;

            break;
        default:
            free(cache);
            return NULL;
    }

    cache->type = type;
    cache->cache = cache->create(cache_size, source);

    return cache;
}

void cache_destroy(Cache *cache) {
    if (cache != NULL) {

        cache->destroy(cache->cache);

        free(cache);
    }
}

Document *cache_get_document(Cache *cache, int identifier) {
    if (cache == NULL || identifier < 0) {
        return NULL;
    }

    return cache->get_doc(cache->cache, identifier);
}

void cache_add_document(Cache *cache, int identifier, Document * doc) {
    if (cache != NULL && identifier >= 0 && doc != NULL) {
        cache->add_doc(cache->cache, identifier, doc);
    }
}


void cache_remoce_document(Cache *cache, int identifier) {
    if (cache != NULL && identifier >= 0) {
        cache->remove_doc(cache->cache, identifier);
    }
}


void show_cache(const Cache *cache) {
    if (cache != NULL) {
        cache->show(cache->cache);
    }
}
