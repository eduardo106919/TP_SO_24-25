/**
 * @file utils.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Utility functions for file and path operations
 * 
 * This header file provides utility functions for creating named pipes (FIFOs),
 * counting keyword occurrences in files, joining paths, and checking for keyword
 * existence in files. These utilities are designed to support file processing
 * and inter-process communication tasks.
 * 
 */

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Creates a named pipe (FIFO) with the specified name
 * 
 * This function creates a FIFO special file with the given name. The FIFO will
 * have default permissions (read and write for owner, group, and others).
 * 
 * @param name The name/path of the FIFO to create
 * @retval 0 FIFO was successfully created
 * @retval 1 FIFO already exists
 * @retval 2 Error occurred during creation
 * @note The function uses mkfifo() system call internally
 */
int create_fifo(const char *name);

/**
 * @brief Counts occurrences of a keyword in a file
 * 
 * Uses system call execlp to count how many times the specified keyword appears
 * in the given file. The counting is line-based (counts lines containing the keyword).
 * 
 * @param path Path to the file to search
 * @param keyword The keyword to search for
 * @retval >=0 Number of lines containing the keyword
 * @retval -1 Error occurred during processing
 * @note This function executes an external process (like grep) via execlp
 */
int count_keyword(const char *path, const char *keyword);

/**
 * @brief Joins two path components into a single absolute path
 * 
 * Combines a folder path and a file name into a complete path, handling the
 * necessary path separator ('/') between them.
 * 
 * @param folder The folder path component
 * @param file The file name component
 * @retval char* Newly allocated string containing the joined path
 * @retval NULL Error occurred (invalid input or memory allocation failed)
 * @note The caller is responsible for freeing the returned memory
 */
char *join_paths(const char *folder, const char *file);

/**
 * @brief Checks if a keyword exists in a file
 * 
 * Searches the specified file for the presence of the given keyword. The search
 * is line-based (checks if any line contains the keyword).
 * 
 * @param path Path to the file to search
 * @param keyword The keyword to search for
 * @retval 0 Keyword was found in the file
 * @retval 1 Keyword was not found
 * @retval -1 Error occurred (file not accessible, etc.)
 * @note This is a faster alternative to count_keyword() when only existence matters
 */
int keyword_exists(const char *path, const char *keyword);

#endif /* UTILS_H */