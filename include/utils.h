#ifndef UTILS_H
#define UTILS_H



int create_fifo(const char *name);

int count_keyword(const char * path, const char * keyword);

char * join_paths(const char *folder, const char * file);

/**
 * @brief 
 * 
 * @param path 
 * @param keyword 
 * @return 0 if keyword exists in the file
 */
int keyword_exists(const char * path, const char * keyword);

#endif