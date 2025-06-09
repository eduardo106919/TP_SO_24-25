/**
 * @file rand_cache.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Random Replacement (RAND) Cache Implementation
 * 
 * Implements a cache with random replacement policy where eviction candidates are
 * selected uniformly at random when space is needed.
 * 
 */

#ifndef RAND_CACHE_H
#define RAND_CACHE_H

#include "document.h"

/**
 * @brief Opaque RAND cache structure
 * 
 * Contains the internal state.
 */
typedef struct rand RAND_Cache;

/**
 * @brief Creates a new RAND cache instance
 * 
 * @param cache_size Maximum number of documents the cache can hold
 * @param source Open file descriptor for fetching documents on cache misses
 * @return Pointer to initialized RAND cache
 * @retval NULL If memory allocation fails or cache_size ≤ 0
 * 
 * @note Allocates resources that must be freed with randc_destroy()
 */
void *randc_create(int cache_size, int source);

/**
 * @brief Destroys a RAND cache instance
 * 
 * @param cache Cache instance to destroy
 * 
 * @note Safe to call with NULL
 */
void randc_destroy(void *cache);

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
Document *randc_get_document(void *cache, int identifier);

/**
 * @brief Adds a document to the cache
 * 
 * @param cache Cache instance to modify
 * @param identifier Document ID to store
 * @param doc Document to cache (a copy will be made)
 * 
 * @note May evict a random document if cache is full
 * @note Uses uniform random selection for eviction
 * @note Makes a deep copy of the document
 */
void randc_add_document(void *cache, int identifier, Document *doc);

/**
 * @brief Removes a specific document from the cache
 * 
 * @param cache Cache instance to modify
 * @param identifier Document ID to remove
 * 
 * @note No effect if document not in cache
 */
void randc_remove_document(void *cache, int identifier);

/**
 * @brief Displays cache contents for debugging
 * 
 * @param cache Cache instance to display
 * 
 * @note No effect if cache is NULL
 */
void randc_show(const void *cache);

#endif /* RAND_CACHE_H */