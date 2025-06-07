
#include "index_table.h"
#include "document.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 4
#define SET_SIZE 8

typedef struct index_table {
    unsigned capacity;
    unsigned count;
    char *table;
} Index_Table;

Index_Table *it_create(void) {
    Index_Table *it = (Index_Table *)calloc(1, sizeof(Index_Table));
    if (it == NULL) {
        return NULL;
    }

    it->capacity = INIT_SIZE;
    it->count = 0;

    it->table = (char *)calloc(it->capacity, sizeof(char));
    if (it->table == NULL) {
        free(it);
        return NULL;
    }

    // initialize the table with invalid entries
    memset(it->table, 0, it->capacity * sizeof(char));

    return it;
}

void it_destroy(Index_Table *it) {
    if (it != NULL) {
        if (it->table != NULL) {
            free(it->table);
        }
        free(it);
    }
}

int it_add_entry(Index_Table *it, int id) {
    if (it == NULL || id < 0) {
        return -1;
    }

    unsigned set = id / SET_SIZE;
    unsigned entry = id % SET_SIZE;

    // table is full or set is larger than capacity
    if ((it->capacity * SET_SIZE) == it->count || set >= it->capacity) {

        // grow 1.5 times
        unsigned new_capacity = it->capacity * 2;

        if (set >= new_capacity) {
            new_capacity = set + 1;
        }

        // allocate more space
        char *other = (char *)realloc(it->table, new_capacity * sizeof(char));
        if (other == NULL) {
            return 1;
        }

        // set the new entries to 0
        memset(other + it->capacity, 0, it->capacity * sizeof(char));

        it->table = other;
        it->capacity = new_capacity;
    }

    // turn the bit to 1
    it->table[set] |= (1 << entry);
    it->count++;

    return 0;
}

int it_remove_entry(Index_Table *it, int id) {
    if (it == NULL) {
        return -1;
    }

    unsigned set = id / SET_SIZE;
    if (set >= it->capacity) {
        return -1;
    }

    unsigned entry = id % SET_SIZE;

    // check if the bit is set
    if ((it->table[set] >> entry) & 1) {
        // turn the bit to 0
        it->table[set] &= ~(1 << entry);
        return id;
    }

    return -1;
}

int it_entry_is_valid(const Index_Table *it, int id) {
    if (it == NULL) {
        return 0;
    }

    unsigned set = id / SET_SIZE;
    if (set >= it->capacity) {
        return 0;
    }

    unsigned entry = id % SET_SIZE;

    // check if the bit is set
    return (it->table[set] >> entry) & 1;
}

unsigned it_size(const Index_Table *it) { return it == NULL ? 0 : it->count; }

bool it_is_empty(const Index_Table *it) { return it != NULL && it->count == 0; }

void it_show(const Index_Table *it) {
    if (it != NULL) {

        printf("\n- INDEX TABLE [capacity: %5u, count: %5u]\n",
               it->capacity * SET_SIZE, it->count);
        printf("[INDEX, VALID, POSITION]\n");

        unsigned j = 0;
        for (unsigned i = 0; i < it->capacity; i++) {
            for (j = 0; j < SET_SIZE; j++) {
                printf("[%5u, %u, %8ld]\n", i * SET_SIZE + j,
                       (it->table[i] >> j) & 1,
                       (i * SET_SIZE + j) * sizeof(Document));
            }
        }
    }
}

Index_Table *it_upload(int file) {
    Index_Table *it = it_create();
    if (it == NULL) {
        return NULL;
    }

    // read the number of entries recorded
    ssize_t out = read(file, &(it->capacity), sizeof(it->capacity));
    if (out == -1) {
        perror("read()");
        return it;
    }

    if (out == 0) {
        return it;
    }

    // read the number of valid documents
    out = read(file, &(it->count), sizeof(it->count));

    it->table = (char *)realloc(it->table, it->capacity * sizeof(char));

    // read the valid bits into the table
    out = read(file, it->table, it->capacity * sizeof(char));

    if (out == -1) {
        perror("read()");
    }

    return it;
}

void it_record(const Index_Table *it, int file) {
    if (it != NULL) {
        // record the capacity of the table in the file
        ssize_t out = write(file, &(it->capacity), sizeof(it->capacity));

        // record the number of valid documents
        out = write(file, &(it->count), sizeof(it->count));

        // record the table in the file
        out = write(file, it->table, it->capacity * sizeof(char));

        if (out == -1) {
            perror("write()");
        }
    }
}

int *it_get_valid_ids(const Index_Table *it) {
    if (it == NULL) {
        return NULL;
    }

    if (it->count == 0) {
        return NULL;
    }

    int *result = (int *)calloc(it->count, sizeof(int));
    if (result == NULL) {
        return NULL;
    }

    unsigned i, j, m = 0;
    for (i = 0; i < it->capacity; i++) {
        for (j = 0; j < SET_SIZE; j++) {
            // entry is valid
            if ((it->table[i] >> j) & 1) {
                result[m++] = i * SET_SIZE + j;
            }
        }
    }

    return result;
}