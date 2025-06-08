
#include "free_list.h"
#include "document.h"

#include <stdio.h>
#include <stdlib.h>


/**
 * @brief Node structure for the free list
 * 
 * Represents a single available document slot in storage.
 * Forms the building block of the Free_List linked list.
 */
struct link {
    int id;                 /**< Document Identifier */
    struct link *next;      /**< Pointer to the next node */
};

/**
 * @brief Free list management structure
 * 
 * Maintains a linked list of available document slots in storage.
 * Provides O(1) insertion/removal from the head of the list.
 */
typedef struct free_list {
    struct link *head;      /**< Pointer to the head of the list */
    unsigned size;          /**< Number of elements on the list */
} Free_List;

static struct link *create_link(int id) {
    struct link *node = (struct link *)calloc(1, sizeof(struct link));
    if (node == NULL) {
        return NULL;
    }

    node->id = id;
    node->next = NULL;

    return node;
}

Free_List *fl_create(void) {
    Free_List *fl = (Free_List *)calloc(1, sizeof(Free_List));
    if (fl == NULL) {
        return NULL;
    }

    fl->head = NULL;
    fl->size = 0;

    return fl;
}

void fl_destroy(Free_List *fl) {
    if (fl != NULL) {
        struct link *temp = fl->head, *other = NULL;

        // free every node
        while (temp != NULL) {
            other = temp->next;
            free(temp);
            temp = other;
        }

        free(fl);
    }
}

int fl_push(Free_List *fl, int id) {
    if (fl == NULL || id < 0)
        return -1;

    struct link *new_node = create_link(id);
    if (new_node == NULL) {
        return 1;
    }

    // add the new node to the head
    new_node->next = fl->head;
    fl->head = new_node;
    fl->size++;

    return 0;
}

int fl_pop(Free_List *fl) {
    if (fl == NULL) {
        return -1;
    }

    struct link *temp = fl->head;
    if (temp == NULL) {
        // empty list
        return -1;
    }

    int result = temp->id;
    temp = temp->next;

    // remove the head of the list
    free(fl->head);
    fl->head = temp;
    fl->size--;

    return result;
}

unsigned fl_size(const Free_List *fl) { return fl == NULL ? 0 : fl->size; }

bool fl_is_empty(const Free_List *fl) { return fl == NULL || fl->size == 0; }

void fl_show(const Free_List *fl) {
    if (fl != NULL) {
        struct link *temp = fl->head;

        printf("\n- FREE LIST [size: %3u]\n", fl->size);
        while (temp != NULL) {
            printf("[%4d, %5ld]\n", temp->id, temp->id * sizeof(Document));
            temp = temp->next;
        }
    }
}

Free_List *fl_upload(int file) {
    Free_List *fl = fl_create();
    if (fl == NULL) {
        return NULL;
    }

    unsigned size = 0;

    // read the number of links
    ssize_t out = read(file, &size, sizeof(size));
    if (out == -1) {
        perror("read()");
        return fl;
    }

    if (out == 0) {
        return fl;
    }

    int temp;
    unsigned i = 0;

    // iterate over the recorded links
    while (i < size && (out = read(file, &temp, sizeof(temp))) > 0) {
        // add the id to the free list
        if (fl_push(fl, temp) != 0) {
            return fl;
        }

        i++;
    }

    if (out == -1) {
        perror("read() 2");
    }

    return fl;
}

void fl_record(const Free_List *fl, int file) {
    if (fl != NULL) {
        // record the size of the list in the file
        ssize_t out = write(file, &(fl->size), sizeof(fl->size));

        struct link *temp = fl->head;

        // record every id from the list in the file
        while (temp != NULL && out > 0) {
            out = write(file, &(temp->id), sizeof(temp->id));

            temp = temp->next;
        }

        if (out == -1) {
            perror("write()");
        }
    }
}