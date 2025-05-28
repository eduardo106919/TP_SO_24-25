#ifndef FREE_LIST_H
#define FREE_LIST_H


#include <unistd.h>
#include <stdbool.h>


typedef struct free_list Free_List;


Free_List * fl_create(void);
void fl_destroy(Free_List * fl);

int fl_push(Free_List * fl, off_t position, unsigned id);
off_t fl_pop(Free_List * fl, unsigned *id);

unsigned fl_size(const Free_List * fl);
bool fl_is_empty(const Free_List * fl);
void fl_show(const Free_List * fl);



#endif