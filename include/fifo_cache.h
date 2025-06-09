/**
 * @file fifo_cache.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief FIFO (First-In-First-Out) cache implementation
 * 
 * Implements a cache with FIFO replacement policy where the oldest inserted
 * document is evicted when the cache reaches capacity.
 * 
 */

#ifndef FIFO_CACHE_H
#define FIFO_CACHE_H

#include "document.h"

/**
 * @brief Opaque FIFO cache structure
 * 
 * Contains the internal state of the FIFO cache
 */
typedef struct fifo FIFO_Cache;

/**
 * @brief Creates a new FIFO cache instance
 * 
 * @param cache_size Maximum number of documents the cache can hold
 * @param source File descriptor for the file source
 * @return Pointer to initialized FIFO cache
 * @retval NULL If memory allocation fails or cache_size ≤ 0
 * 
 * @note Allocates resources that must be freed with fifoc_destroy()
 */
void *fifoc_create(int cache_size, int source);

/**
 * @brief Destroys a FIFO cache instance
 * 
 * @param cache Cache instance to destroy
 * 
 * @note Safe to call with NULL
 * @note Frees all cached documents and internal structures
 */
void fifoc_destroy(void *cache);

/**
 * @brief Retrieves a document from the cache
 * 
 * @param cache Cache instance to query
 * @param identifier Document ID to lookup
 * @return Pointer to cached document
 * @retval NULL If document not found or invalid cache
 */
Document *fifoc_get_document(void *cache, int identifier);

/**
 * @brief Adds a document to the cache
 * 
 * @param cache Cache instance to modify
 * @param identifier Document ID to store
 * @param doc Document to cache (a copy will be made)
 * 
 * @note May evict the oldest document if cache is full
 * @note Makes a deep copy of the document
 */
void fifoc_add_document(void *cache, int identifier, Document *doc);

/**
 * @brief Removes a document from the cache
 * 
 * @param cache Cache instance to modify
 * @param identifier Document ID to remove
 * 
 * @note No effect if document not in cache
 */
void fifoc_remove_document(void *cache, int identifier);

/**
 * @brief Displays cache contents for debugging
 * 
 * @param cache Cache instance to display
 * 
 * @note No effect if cache is NULL
 */
void fifoc_show(const void *cache);

#endif /* FIFO_CACHE_H */