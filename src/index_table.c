
#include "index_table.h"

#include <stdlib.h>
#include <stdio.h>


#define INIT_SIZE 100


struct entry {
    char valid;
    off_t position;
};


typedef struct index_table {
    unsigned capacity;
    unsigned count;
    struct entry *table;
} Index_Table;


Index_Table * it_create(void) {
    Index_Table * it = (Index_Table *) calloc(1, sizeof(Index_Table));
    if (it == NULL) {
        return NULL;
    }

    it->capacity = INIT_SIZE;
    it->count = 0;
    it->table = (struct entry *) calloc(it->capacity, sizeof(struct entry));
    if (it->table == NULL) {
        free(it);
        return NULL;
    }

    // initialize the table with invalid entries
    for (unsigned i = 0; i < it->capacity; i++) {
        it->table[i].valid = 0;
    }

    return it;
}


void it_destroy(Index_Table * it) {
    if (it != NULL) {
        if (it->table != NULL) {
            free(it->table);
        }
        free(it);
    }
}


int it_add_entry(Index_Table *it, off_t position, unsigned id) {
    if (it == NULL) {
        return -1;
    }

    // table is full, or id is bigger
    if (it->count == it->capacity || id > it->capacity) {
        unsigned temp = it->capacity;

        it->capacity *= 2;
        if (id > it->capacity) {
            it->capacity = id + 1;
        }

        it->table = (struct entry *) realloc(it->table, it->capacity * sizeof(struct entry));
        if (it->table == NULL) {
            free(it);
            return -1;
        }

        for (; temp < it->capacity; temp++) {
            it->table[temp].valid = 0;
        }
    }

    if (it->table[id].valid == 0) {
        // add entry to table
        it->table[id].valid = 1;
        it->table[id].position = position;

        it->count++;
    }

    return 0;
}


off_t it_remove_entry(Index_Table *it, unsigned id) {
    if (it == NULL) {
        return -1;
    }

    if (id >= it->capacity) {
        return -1;
    }

    if (it->table[id].valid == 0) {
        return -1;
    }

    it->table[id].valid = 0;
    it->count--;
    
    return it->table[id].position;
}


unsigned it_size(const Index_Table *it) {
    return it == NULL ? 0 : it->count;
}


bool it_is_empty(const Index_Table *it) {
    return it != NULL && it->count == 0;
}


void it_show(const Index_Table *it) {
    if (it != NULL) {
        for (unsigned i = 0; i < it->capacity; i++) {
            printf("[%4u, %d, %ld]\n", i, it->table[i].valid, it->table[i].position);
        }
    }
}


Index_Table * it_upload(int file) {
    Index_Table * it = it_create();
    if (it == NULL) {
        return NULL;
    }

    unsigned size = 0;
    struct entry temp;

    // read the number of entries recorded
    ssize_t out = read(file, &size, sizeof(size));
    if (out == -1) {
        perror("read()");
        return it;
    }

    if (out == 0) {
        return it;
    }

    unsigned i = 0;
    // iterate over the recorded entries
    while (i < size && (out = read(file, &temp, sizeof(temp))) > 0) {
        if (temp.valid != 0) {
            it_add_entry(it, temp.position, i);
        }

        i++;
    }
    
    if (out == -1) {
        perror("read() 3");
    }

    return it;
}

void it_record(const Index_Table * it, int file) {
    if (it != NULL) {
        // record the capacity of the table in the file
        ssize_t out = write(file, &(it->capacity), sizeof(it->capacity));
        
        // record the table in the file
        out = write(file, it->table, it->capacity * sizeof(struct entry));

        if (out == -1) {
            perror("write()");
        }
    }
}