
#include "lru_cache.h"
#include "defs.h"

#include <stdlib.h>
#include <string.h>


/**
 * @brief Represents a Least Recently Used (LRU) cache for Documents.
 *
 * This cache stores Documents with their identifiers and uses
 * approximate LRU eviction, based on reference bits.
 */
typedef struct lru {
    /**
     * @brief Array of documents currently stored in the cache.
     * 
     */
    Document *documents;

    /**
     * @brief Array of identifiers corresponding to each document.
     * 
     */
    int *identifiers;

    /**
     * @brief Reference bits array indicating if a position was searched previously, 
     * used to approximate LRU behavior.
     * 
     */
    char *ref_bits;

    /**
     * @brief Number of entries the cache can hold.
     * 
     */
    int size;

    /**
     * @brief File descriptor used to retrieve data on a cache miss.
     * 
     */
    int source;

    /**
     * @brief Position index to start the next search or eviction attempt.
     * 
     */
    int back;
} LRU_Cache;


void *lruc_create(int cache_size, int source) {
    if (cache_size < 0 || source < 0) {
        return NULL;
    }

    LRU_Cache *lru = (LRU_Cache *)calloc(1, sizeof(LRU_Cache));
    if (lru == NULL) {
        return NULL;
    }

    lru->size = cache_size;
    lru->source = source;
    lru->back = 0;

    lru->documents = (Document *)calloc(lru->size, sizeof(Document));
    if (lru->documents == NULL) {
        free(lru);
        return NULL;
    }

    memset(lru->documents, 0, lru->size * sizeof(Document));

    lru->identifiers = (int *)calloc(lru->size, sizeof(int));
    if (lru->identifiers == NULL) {
        free(lru->documents);
        free(lru);
        return NULL;
    }

    memset(lru->identifiers, -1, lru->size * sizeof(int));

    lru->ref_bits = (char *)calloc(lru->size, sizeof(char));
    if (lru->ref_bits == NULL) {
        free(lru->documents);
        free(lru->ref_bits);
        free(lru);
        return NULL;
    }

    memset(lru->ref_bits, 0, lru->size * sizeof(char));

    return lru;
}

void lruc_destroy(void *cache) {
    if (cache != NULL) {
        LRU_Cache *lru = (LRU_Cache *)cache;

        if (lru->documents != NULL) {
            free(lru->documents);
        }

        if (lru->identifiers != NULL) {
            free(lru->identifiers);
        }

        if (lru->ref_bits != NULL) {
            free(lru->ref_bits);
        }

        free(lru);
    }
}

Document *lruc_get_document(void *cache, int identifier) {
    if (cache == NULL || identifier < 0) {
        return NULL;
    }

    LRU_Cache *lru = (LRU_Cache *)cache;

    printf("[CACHE INFO] searching in memory for %d\n", identifier);

    int i;
    // search the document in the cache
    for (i = 0; i < lru->size && lru->identifiers[lru->back] != identifier;
         i++) {
        // set reference bit to 0
        lru->ref_bits[lru->back] = 0;
        lru->back = (lru->back + 1) % lru->size;
    }

    // document is not in cache
    if (i == lru->size) {
        // get BLOCK_SIZE documents from metadata.bin

        printf("[CACHE INFO] going to disk for %d\n", identifier);

        Document *docs = (Document *)calloc(BLOCK_SIZE, sizeof(Document));
        if (docs == NULL) {
            return NULL;
        }

        // go to the right position
        lseek(lru->source, identifier * sizeof(Document), SEEK_SET);

        // read the documents from disk
        ssize_t out = read(lru->source, docs, BLOCK_SIZE * sizeof(Document));
        if (out == -1) {
            perror("read()");
            return NULL;
        }

        Document *result = clone_document(docs);

        // number of documents read, less or equal than BLOCK_SIZE
        int temp_size = out / sizeof(Document);

        int j = 0;
        // fill the cache with a new block
        for (i = 0; i < lru->size && j < temp_size; i++) {
            // fill the empty slots first
            if (lru->identifiers[i] == -1) {
                memcpy(lru->documents + i, docs + j, sizeof(docs[j]));
                lru->identifiers[i] = identifier + j;
                lru->ref_bits[i] = 1;
                j++;
            }
        }

        // place the remaining in places with reference bits to 0
        for (i = 0; i < lru->size && j < temp_size; i++) {
            if (lru->ref_bits[i] == 0) {
                memcpy(lru->documents + i, docs + j, sizeof(docs[j]));
                lru->identifiers[i] = identifier + j;
                lru->ref_bits[i] = 1;
                j++;
            }
        }

        // when the previous options don't work, place it at the front
        for (i = 0; i < lru->size && j < temp_size; i++) {
            memcpy(lru->documents + i, docs + j, sizeof(docs[j]));
            lru->identifiers[i] = identifier + j;
            lru->ref_bits[i] = 1;
            j++;
        }

        return result;
    }
    
    lru->ref_bits[lru->back] = 1;

    return clone_document(lru->documents + lru->back);
}


void lruc_add_document(void *cache, int identifier, Document * doc) {
    if (cache != NULL && identifier >= 0 && doc != NULL) {
        LRU_Cache *lru = (LRU_Cache *)cache;

        int i = 0;
        // search for empty positions
        for (; i < lru->size && lru->identifiers[i] != -1; i++);

        int position = i;
        if (position == lru->size) {
            // cache is full, search for reference bits to 0
            // does one more iteration, if the cache has no ref bit to 0, the last one will be 0
            for (i = 0; i <= lru->size && lru->ref_bits[lru->back] != 0; i++) {
                lru->ref_bits[lru->back] = 0;
                lru->back = (lru->back + 1) % lru->size;
            }

            position = lru->back;
        }

        // place the document
        memcpy(lru->documents + position, doc, sizeof(Document));
        lru->identifiers[position] = identifier;
        lru->ref_bits[position] = 1;
    }
}

void lruc_remove_document(void *cache, int identifier) {
    if (cache != NULL && identifier >= 0) {
        LRU_Cache *lru = (LRU_Cache *)cache;

        int i = 0;
        // search for the document
        for (; i < lru->size && lru->identifiers[i] != identifier; i++);

        if (i < lru->size) {
            // found the document
            lru->identifiers[i] = -1;
        }
    }
}


void lruc_show(const void *cache) {
    if (cache != NULL) {
        LRU_Cache *lru = (LRU_Cache *)cache;

        printf("\n- LRU CACHE [capacity: %d]\n", lru->size);
        printf("[INDEX, REF_BIT, IDENTIFIER]\n");

        for (int i = 0; i < lru->size; i++) {
            printf("[%3d, %d, %5d]\n", i, lru->ref_bits[i],
                   lru->identifiers[i]);
        }
    }
}
