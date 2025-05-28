#include "free_list.h"

#include <stdlib.h>
#include <stdio.h>


struct link {
    off_t position;
    unsigned id;
    struct link * next;
};

typedef struct free_list {
    struct link * head;
    unsigned size;
} Free_List;


static struct link * create_link(off_t position, unsigned id) {
    struct link * node = (struct link *) calloc(1, sizeof(struct link));
    if (node == NULL) {
        return NULL;
    }

    node->id = id;
    node->position = position;
    node->next = NULL;

    return node;
}

Free_List * fl_create(void) {
    Free_List * fl = (Free_List *) calloc(1, sizeof(Free_List));
    if (fl == NULL) {
        return NULL;
    }

    fl->head = NULL;
    fl->size = 0;

    return fl;
}


void fl_destroy(Free_List * fl) {
    if (fl != NULL) {
        struct link * temp = fl->head, *other = NULL;

        while (temp != NULL) {
            other = temp->next;
            free(temp);
            temp = other;
        }

        free(fl);
    }
}


int fl_push(Free_List * fl, off_t position, unsigned id) {
    if (fl == NULL)
        return -1;

    struct link * new_node = create_link(position, id);
    if (new_node == NULL) {
        return 1;
    }

    new_node->next = fl->head;
    fl->head = new_node;
    fl->size++;

    return 0;
}


off_t fl_pop(Free_List * fl, unsigned *id) {
    if (fl == NULL || id == NULL) {
        return -1;
    }

    struct link * temp = fl->head;
    if (temp == NULL) {
        // empty list
        return -1;
    }

    off_t result = temp->position;
    *id = temp->id;
    temp = temp->next;
    
    free(fl->head);
    fl->head = temp;
    fl->size--;
    
    return result;
}


unsigned fl_size(const Free_List * fl) {
    return fl == NULL ? 0 : fl->size;
}


bool fl_is_empty(const Free_List * fl) {
    return fl == NULL || fl->size == 0;
}


void fl_show(const Free_List * fl) {
    if (fl != NULL) {
        struct link * temp = fl->head;

        while (temp != NULL) {
            printf("[%4u, %5ld]\n", temp->id, temp->position);
            temp = temp->next;
        }
    }
}

