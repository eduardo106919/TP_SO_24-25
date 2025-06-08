
#include "cache.h"
#include "fifo_cache.h"
#include "lru_cache.h"
#include "rand_cache.h"

#include <stdlib.h>


/**
 * @brief Polymorphic cache container structure
 * 
 * Implements the Strategy pattern for cache replacement algorithms.
 * Contains function pointers to the concrete cache implementation
 * matching the selected replacement strategy.
 */
typedef struct cache {
    Cache_Type type;        /**< Active replacement strategy */
    void *cache;            /**< Opaque pointer to strategy-specific cache instance */
    
    /**
     * @brief Strategy constructor function pointer
     * @param size Cache capacity
     * @param source Data source identifier
     * @return Pointer to initialized cache instance
     */
    void *(*create)(int size, int source);
    
    /**
     * @brief Strategy destructor function pointer
     * @param cache Concrete cache instance to destroy
     */
    void (*destroy)(void *cache);
    
    /**
     * @brief Document retrieval function pointer
     * @param cache Concrete cache instance
     * @param id Document identifier to lookup
     * @return Retrieved document or NULL
     */
    Document *(*get_doc)(void *cache, int id);
    
    /**
     * @brief Document insertion function pointer
     * @param cache Concrete cache instance
     * @param id Document identifier to store
     * @param doc Document to cache
     */
    void (*add_doc)(void *cache, int id, Document *doc);
    
    /**
     * @brief Document removal function pointer
     * @param cache Concrete cache instance
     * @param id Document identifier to remove
     */
    void (*remove_doc)(void *cache, int id);
    
    /**
     * @brief Debug display function pointer
     * @param cache Concrete cache instance to display
     */
    void (*show)(const void *cache);
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


void cache_remove_document(Cache *cache, int identifier) {
    if (cache != NULL && identifier >= 0) {
        cache->remove_doc(cache->cache, identifier);
    }
}


void show_cache(const Cache *cache) {
    if (cache != NULL) {
        cache->show(cache->cache);
    }
}
