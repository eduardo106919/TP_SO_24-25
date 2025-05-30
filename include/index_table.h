#ifndef INDEX_TABLE_H
#define INDEX_TABLE_H

#include <unistd.h>
#include <stdbool.h>


typedef struct index_table Index_Table;


Index_Table * it_create(void);
void it_destroy(Index_Table * it);

int it_add_entry(Index_Table *it, off_t position, unsigned id);
off_t it_remove_entry(Index_Table *it, unsigned id);

unsigned it_size(const Index_Table *it);
bool it_is_empty(const Index_Table *it);
void it_show(const Index_Table *it);

Index_Table * it_upload(int file);
void it_record(const Index_Table * it, int file);


#endif