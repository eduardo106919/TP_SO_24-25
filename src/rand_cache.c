
#include "rand_cache.h"
#include "defs.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief A randomized cache for storing documents and their identifiers.
 *
 * This structure implements a cache that uses a **random eviction policy**.
 * Documents are stored in an array alongside their corresponding identifiers.
 * If an identifier is set to `-1`, the corresponding cache entry is considered **invalid**.
 * In the case of a cache miss, documents are retrieved from a file using a provided file descriptor.
 */
typedef struct rand {
    /**
     * @brief Array of cached documents.
     *
     * Each entry corresponds to a document currently stored in the cache.
     */
    Document *documents;

    /**
     * @brief Array of document identifiers corresponding to each cached document.
     *
     * Each identifier maps to the document at the same index in the `documents` array.
     * A value of `-1` indicates that the slot is unused or invalid.
     */
    int *identifiers;

    /**
     * @brief Maximum number of documents the cache can hold.
     */
    int size;

    /**
     * @brief File descriptor for the source document file.
     *
     * Used to retrieve documents from disk in the event of a cache miss.
     */
    int source;
} RAND_Cache;


void *randc_create(int cache_size, int source) {
    if (cache_size < 0 || source < 0) {
        return NULL;
    }

    RAND_Cache *cache = (RAND_Cache *)calloc(1, sizeof(RAND_Cache));
    if (cache == NULL) {
        return NULL;
    }

    cache->size = cache_size;
    cache->source = source;

    cache->documents = (Document *)calloc(cache->size, sizeof(Document));
    if (cache->documents == NULL) {
        free(cache);
        return NULL;
    }

    memset(cache->documents, 0, cache->size * sizeof(Document));

    cache->identifiers = (int *)calloc(cache->size, sizeof(int));
    if (cache->identifiers == NULL) {
        free(cache->documents);
        free(cache);
        return NULL;
    }

    // ids also work as valid bits, -1 is invalid
    memset(cache->identifiers, -1, cache->size * sizeof(int));

    return cache;
}

void randc_destroy(void *cache) {
    if (cache != NULL) {
        RAND_Cache *rc = (RAND_Cache *)cache;

        if (rc->documents != NULL) {
            free(rc->documents);
        }

        if (rc->identifiers != NULL) {
            free(rc->identifiers);
        }

        free(rc);
    }
}

Document *randc_get_document(void *cache, int identifier) {
    if (cache == NULL && identifier < 0) {
        return NULL;
    }

    RAND_Cache *rc = (RAND_Cache *)cache;

    printf("[CACHE INFO] searching in memory for %d\n", identifier);

    int i;
    // search the document in the cache
    for (i = 0; i < rc->size && rc->identifiers[i] != identifier; i++)
        ;

    // document is not in cache
    if (i == rc->size) {
        // get BLOCK_SIZE documents from metadata.bin

        printf("[CACHE INFO] going to disk for %d\n", identifier);

        Document *docs = (Document *)calloc(BLOCK_SIZE, sizeof(Document));
        if (docs == NULL) {
            return NULL;
        }

        // go to the right position
        lseek(rc->source, identifier * sizeof(Document), SEEK_SET);

        // read the documents from disk
        ssize_t out = read(rc->source, docs, BLOCK_SIZE * sizeof(Document));
        if (out == -1) {
            perror("read()");
            return NULL;
        }

        Document *result = clone_document(docs);

        // number of documents read, less or equal than BLOCK_SIZE
        int temp_size = out / sizeof(Document);

        srand(time(0));

        int rand_position = rand() % rc->size;

        // place the documents in a random place
        // will replace documents, whether the cache is full or not!!!
        for (int i = 0; i < temp_size; i++) {
            memcpy(rc->documents + rand_position, docs + i, sizeof(docs[i]));
            rc->identifiers[rand_position] = identifier + i;
            rand_position = (rand_position + 1) % rc->size;
        }

        return result;
    }

    return clone_document(rc->documents + i);
}

void randc_add_document(void *cache, int identifier, Document * doc) {
    if (cache != NULL && identifier >= 0 && doc != NULL) {
        RAND_Cache *rc = (RAND_Cache *)cache;

        int i = 0;
        // search for empty positions
        for (; i < rc->size && rc->identifiers[i] != -1; i++);

        int position = i;
        // cache is full, replace one document in a random place
        if (position == rc->size) {
            srand(time(0));
            position = rand() % rc->size;
        }

        // place the document in the position
        memcpy(rc->documents + position, doc, sizeof(Document));
        rc->identifiers[position] = identifier;
    }
}


void randc_remove_document(void *cache, int identifier) {
    if (cache != NULL && identifier >= 0) {
        RAND_Cache *rc = (RAND_Cache *)cache;

        int i = 0;
        // search for the document
        for (; i < rc->size && rc->identifiers[i] != identifier; i++);

        if (i < rc->size) {
            // mark the position as invalid
            rc->identifiers[i] = -1;
        }
    }
}


void randc_show(const void *cache) {
    if (cache != NULL) {
        RAND_Cache *rc = (RAND_Cache *)cache;

        printf("\n- RAND CACHE [capacity: %d]\n", rc->size);
        printf("[INDEX, IDENTIFIER]\n");

        for (int i = 0; i < rc->size; i++) {
            printf("[%3d, %5d]\n", i, rc->identifiers[i]);
        }
    }
}
