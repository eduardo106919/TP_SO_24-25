/**
 * @file lru_cache.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief LRU (Least Recently Used) Cache Implementation
 *
 * Implements a cache that aproximates a LRU replacement policy, where the least recently accessed
 * document is evicted when the cache reaches capacity. 
 *
 */

#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "document.h"

/**
 * @brief Opaque LRU cache structure
 *
 * Contains the internal state.
 */
typedef struct lru LRU_Cache;

/**
 * @brief Creates a new LRU cache instance
 *
 * @param cache_size Maximum number of documents the cache can hold
 * @param source Open file descriptor for fetching documents on cache misses
 * @return Pointer to initialized LRU cache
 * @retval NULL If memory allocation fails or cache_size ≤ 0
 *
 * @note Allocates resources that must be freed with lruc_destroy()
 */
void *lruc_create(int cache_size, int source);

/**
 * @brief Destroys an LRU cache instance
 *
 * @param cache Cache instance to destroy
 *
 * @note Safe to call with NULL
 */
void lruc_destroy(void *cache);

/**
 * @brief Retrieves a document from the cache
 *
 * @param cache Cache instance to query
 * @param identifier Document ID to lookup
 * @return Pointer to cached document
 * @retval NULL If document not found in cache AND unavailable in source
 *
 * @note Returned document must be freed by caller
 */
Document *lruc_get_document(void *cache, int identifier);

/**
 * @brief Adds a document to the cache
 *
 * @param cache Cache instance to modify
 * @param identifier Document ID to store
 * @param doc Document to cache (a copy will be made)
 *
 * @note Makes a deep copy of the document
 */
void lruc_add_document(void *cache, int identifier, Document *doc);

/**
 * @brief Removes a specific document from the cache
 *
 * @param cache Cache instance to modify
 * @param identifier Document ID to remove
 *
 * @note No effect if document not in cache
 */
void lruc_remove_document(void *cache, int identifier);

/**
 * @brief Displays cache contents for debugging
 *
 * @param cache Cache instance to display
 *
 * @note No effect if cache is NULL
 */
void lruc_show(const void *cache);

#endif /* LRU_CACHE_H */