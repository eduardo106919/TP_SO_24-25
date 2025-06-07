/**
 * @file index_table.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Index table implementation for managing file positions in a linear
 * array.
 *
 * The index table maintains a dynamic array of file positions with associated
 * identifiers, providing efficient O(1) access to positions by ID. This is
 * particularly useful for file systems or database implementations that need
 * direct access to stored records.
 *
 * @note All create/destroy operations should be paired:
 *       - it_create() must be matched with it_destroy()
 *       - it_upload() must be matched with it_destroy()
 *
 * @example Index_Table usage:
 * @code
 * Index_Table *it = it_create();
 * it_add_entry(it, 1024, 1);  // Add position 1024 with ID 1
 * it_add_entry(it, 2048, 2);  // Add position 2048 with ID 2
 *
 * off_t pos = it_remove_entry(it, 1);  // Retrieve position for ID 1
 *
 * it_destroy(it);  // Clean up
 * @endcode
 *
 */

#ifndef INDEX_TABLE_H
#define INDEX_TABLE_H

#include <stdbool.h>
#include <unistd.h>

/**
 * @brief Opaque structure representing an Index Table
 *
 * The Index_Table maintains a dynamic array mapping numeric identifiers
 * to file positions, allowing direct access by identifier.
 */
typedef struct index_table Index_Table;

/**
 * @brief Creates a new empty Index Table
 *
 * @return Pointer to a newly allocated Index Table
 * @retval NULL if memory allocation fails
 *
 * @note Must be paired with it_destroy() to avoid memory leaks
 */
Index_Table *it_create(void);

/**
 * @brief Destroys an Index Table and releases all resources
 *
 * @param it Pointer to the Index Table to destroy
 *
 * @note Safe to call with NULL (no operation performed)
 * @note Should be called exactly once for each created/uploaded table
 */
void it_destroy(Index_Table *it);

/**
 * @brief Adds or updates a file position with the given identifier
 *
 * @param it Pointer to the Index Table
 * @param position File offset to store
 * @param id Associated identifier for the position
 * @return Operation status
 * @retval 0 on success
 * @retval -1 on invalid input (NULL it)
 *
 * @note If the id already exists, the position will be updated
 */
int it_add_entry(Index_Table *it, int id);

/**
 * @brief Removes and returns the file position for the given identifier
 *
 * @param it Pointer to the Index Table
 * @param id Identifier to look up
 * @return The associated file position
 * @retval -1 if it is NULL or id not found
 *
 * @note The entry is marked as invalid in the table
 */
int it_remove_entry(Index_Table *it, int id);

int it_entry_is_valid(const Index_Table *it, int id);

/**
 * @brief Returns the current number of entries in the table
 *
 * @param it Pointer to the Index Table
 * @return Number of entries in the table
 * @retval 0 if it is NULL or table is empty
 */
unsigned it_size(const Index_Table *it);

/**
 * @brief Checks if the Index Table contains any entries
 *
 * @param it Pointer to the Index Table
 * @return Table emptiness status
 * @retval true if it is NULL or table is empty
 * @retval false if table contains entries
 */
bool it_is_empty(const Index_Table *it);

/**
 * @brief Debug function to print Index Table contents
 *
 * @param it Pointer to the Index Table
 *
 * @note Output format is implementation-dependent
 * @note No effect if it is NULL
 */
void it_show(const Index_Table *it);

/**
 * @brief Loads an Index Table from a file
 *
 * @param file Open file descriptor to read from
 * @return Pointer to the loaded Index Table
 * @retval NULL on read error or invalid input
 *
 * @note Must be paired with it_destroy()
 */
Index_Table *it_upload(int file);

/**
 * @brief Saves an Index Table to a file
 *
 * @param it Pointer to the Index Table to save
 * @param file Open file descriptor to write to
 *
 * @note No effect if it or file is invalid
 */
void it_record(const Index_Table *it, int file);

int *it_get_valid_ids(const Index_Table *it);

#endif