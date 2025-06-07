
#include "fifo_cache.h"
#include "defs.h"

#include <stdlib.h>
#include <string.h>

typedef struct fifo {
    Document *documents;
    int *identifiers;
    int back;
    int size;
    int source;
} FIFO_Cache;

void *fifoc_create(int cache_size, int source) {
    if (cache_size < 0 || source < 0) {
        return NULL;
    }

    FIFO_Cache *cache = (FIFO_Cache *)calloc(1, sizeof(FIFO_Cache));
    if (cache == NULL) {
        return NULL;
    }

    cache->size = cache_size;

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

    cache->back = 0;
    cache->source = source;

    return cache;
}

void fifoc_destroy(void *cache) {
    if (cache != NULL) {
        FIFO_Cache *fifo = (FIFO_Cache *)cache;
        if (fifo->documents != NULL) {
            free(fifo->documents);
        }

        if (fifo->identifiers != NULL) {
            free(fifo->identifiers);
        }

        free(fifo);
    }
}

Document *fifoc_get_document(void *cache, int identifier) {
    if (cache == NULL || identifier < 0) {
        return NULL;
    }

    FIFO_Cache *fifo = (FIFO_Cache *)cache;

    printf("[CACHE INFO] searching in memory for %d\n", identifier);

    int i;
    // search the document in the cache
    for (i = 0; i < fifo->size && fifo->identifiers[i] != identifier; i++)
        ;

    // document is not in cache
    if (i == fifo->size) {
        // get BLOCK_SIZE documents from metadata.bin

        printf("[CACHE INFO] going to disk for %d\n", identifier);

        Document *docs = (Document *)calloc(BLOCK_SIZE, sizeof(Document));
        if (docs == NULL) {
            return NULL;
        }

        // go to the right position
        lseek(fifo->source, identifier * sizeof(Document), SEEK_SET);

        // read the documents from disk
        ssize_t out = read(fifo->source, docs, BLOCK_SIZE * sizeof(Document));
        if (out == -1) {
            perror("read()");
            return NULL;
        }

        Document *result = clone_document(docs);

        // number of documents read, less or equal than BLOCK_SIZE
        int temp_size = out / sizeof(Document);

        // fill the cache with a new block
        for (i = 0; i < temp_size; i++) {
            memcpy(fifo->documents + fifo->back, docs + i, sizeof(docs[i]));
            fifo->identifiers[fifo->back] = identifier + i;
            fifo->back = (fifo->back + 1) % fifo->size;
        }

        return result;
    }

    return clone_document(fifo->documents + i);
}

void fifoc_show(const void *cache) {
    if (cache != NULL) {
        FIFO_Cache *fifo = (FIFO_Cache *)cache;

        printf("\n- FIFO CACHE [capacity: %d]\n", fifo->size);
        printf("[INDEX, IDENTIFIER]\n");

        for (int i = 0; i < fifo->size; i++) {
            printf("[%3d, %5d]\n", i, fifo->identifiers[i]);
        }
    }
}