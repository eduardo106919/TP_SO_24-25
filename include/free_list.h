/**
 * @file free_list.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Free list implementation for managing available space in a file
 * system.
 *
 * The free list maintains a linked list of available (free) identifiers
 * corresponding to a position in a file, enabling efficient space management
 * for file systems or storage systems.
 *
 * @note All create/destroy operations should be paired:
 *       - fl_create() must be matched with fl_destroy()
 *       - fl_upload() must be matched with fl_destroy()
 *
 * @example Free_List usage:
 * @code
 * Free_List *fl = fl_create();
 * fl_push(fl, 1);  // Add ID 1
 * fl_push(fl, 2);  // Add ID 2
 *
 * unsigned id = fl_pop(fl);  // Retrieve next available identifier
 *
 * fl_destroy(fl);  // Clean up
 * @endcode
 *
 */

#ifndef FREE_LIST_H
#define FREE_LIST_H

#include <stdbool.h>
#include <unistd.h>

/**
 * @brief Opaque structure representing a Free List
 *
 * The Free_List maintains a collection of available file positions
 * and their associated identifiers in a LIFO (stack) order.
 */
typedef struct free_list Free_List;

/**
 * @brief Creates a new empty Free List
 *
 * @return Pointer to a newly allocated Free List
 * @retval NULL if memory allocation fails
 *
 * @note Must be paired with fl_destroy() to avoid memory leaks
 */
Free_List *fl_create(void);

/**
 * @brief Destroys a Free List and releases all resources
 *
 * @param fl Pointer to the Free List to destroy
 *
 * @note Safe to call with NULL (no operation performed)
 * @note Should be called exactly once for each created/uploaded list
 */
void fl_destroy(Free_List *fl);

/**
 * @brief Adds a file identifier to the Free List
 *
 * @param fl Pointer to the Free List
 * @param id File identifier to mark as available
 * @return Operation status
 * @retval 0 on success
 * @retval 1 on memory allocation error
 * @retval -1 on invalid input (NULL fl or invalid id)
 */
int fl_push(Free_List *fl, int id);

/**
 * @brief Removes and returns the next available file identifier
 *
 * @param fl Pointer to the Free List
 * @return The available file identifier
 * @retval -1 if fl is NULL, or list is empty
 *
 * @note The returned identifier is removed from the free list
 */
int fl_pop(Free_List *fl);

/**
 * @brief Returns the current number of available identifiers
 *
 * @param fl Pointer to the Free List
 * @return Number of available identifiers
 * @retval 0 if fl is NULL or list is empty
 */
unsigned fl_size(const Free_List *fl);

/**
 * @brief Checks if the Free List contains any available identifiers
 *
 * @param fl Pointer to the Free List
 * @return List emptiness status
 * @retval true if fl is NULL or list is empty
 * @retval false if list contains identifiers
 */
bool fl_is_empty(const Free_List *fl);

/**
 * @brief Debug function to print Free List contents
 *
 * @param fl Pointer to the Free List
 *
 * @note Output format is implementation-dependent
 * @note No effect if fl is NULL
 */
void fl_show(const Free_List *fl);

/**
 * @brief Loads a Free List from a file
 *
 * @param file Open file descriptor to read from
 * @return Pointer to the loaded Free List
 * @retval NULL on read error or invalid input
 *
 * @note Must be paired with fl_destroy()
 */
Free_List *fl_upload(int file);

/**
 * @brief Saves a Free List to a file
 *
 * @param fl Pointer to the Free List to save
 * @param file Open file descriptor to write to
 *
 * @note No effect if fl or file is invalid
 */
void fl_record(const Free_List *fl, int file);

#endif