/**
 * @file cache.h
 * @brief Cache abstraction layer for document management system
 *
 * Provides a unified interface for multiple cache replacement strategies.
 * Acts as a facade that delegates operations to specific cache implementations
 * (FIFO, RAND, LRU) based on user selection at initialization.
 *
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @date [Creation or Last Modification Date]
 * @version 1.0
 */

#ifndef CACHE_H
#define CACHE_H

#include "document.h"

/**
 * @brief Opaque cache structure
 *
 * Contains the cache instance and strategy-specific implementation details.
 * The actual structure varies based on the selected cache type.
 */
typedef struct cache Cache;

/**
 * @brief Supported cache replacement strategies
 */
typedef enum {
    FIFO,   /**< First-In-First-Out replacement */
    RAND,   /**< Random replacement */
    LRU,    /**< Least Recently Used replacement */
    NONE    /**< No caching (disables cache) */
} Cache_Type;

/**
 * @brief Initializes the cache with specified strategy
 *
 * Creates and configures the appropriate cache implementation based on type.
 *
 * @param cache_size Maximum number of documents in cache
 * @param type Replacement strategy to use
 * @param source File descriptor to get documents from
 * @return Cache instance pointer
 * @retval NULL If initialization fails (invalid parameters or allocation error)
 *
 * @note Actual implementation is strategy-specific
 * @note Allocates resources that must be freed with cache_destroy()
 */
Cache *cache_start(int cache_size, Cache_Type type, int source);

/**
 * @brief Releases all cache resources
 *
 * @param cache Cache instance to destroy
 *
 * @note Safe to call with NULL
 * @note Properly cleans up strategy-specific resources
 */
void cache_destroy(Cache *cache);

/**
 * @brief Retrieves a document from cache
 *
 * @param cache Cache instance
 * @param identifier Document ID to lookup
 * @return Pointer to cached document
 * @retval NULL If not found or invalid cache
 *
 * @note Delegates to strategy-specific implementation
 */
Document *cache_get_document(Cache *cache, int identifier);

/**
 * @brief Adds a document to cache
 *
 * @param cache Cache instance
 * @param identifier Document ID to cache
 * @param doc Document to store
 *
 * @note Makes a copy of the document
 */
void cache_add_document(Cache *cache, int identifier, Document *doc);

/**
 * @brief Removes a document from cache
 *
 * @param cache Cache instance
 * @param identifier Document ID to remove
 *
 * @note No effect if document not in cache
 * @note Handles strategy-specific cleanup if needed
 */
void cache_remove_document(Cache *cache, int identifier);

/**
 * @brief Displays cache contents for debugging
 *
 * @param cache Cache instance to display
 *
 * @note Output format varies by cache type
 * @note Shows strategy-specific metadata
 */
void show_cache(const Cache *cache);

#endif /* CACHE_H */