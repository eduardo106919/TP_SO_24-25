/**
 * @file index_table.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Compact bit-based index table for tracking file validity
 *
 * Implements a space-efficient index table where each bit in a character array
 * represents the validity status of a corresponding file. This provides:
 * - O(1) validity checks
 * - Minimal memory usage (1 bit per file)
 * - Compact disk storage format
 *
 * @note All create/destroy operations should be paired:
 *       - it_create() must be matched with it_destroy()
 *       - it_upload() must be matched with it_destroy()
 *
 * @example Basic usage:
 * @code
 * Index_Table *it = it_create();
 * it_add_entry(it, 42);    // Mark file ID 42 as valid
 * 
 * if (it_entry_is_valid(it, 42)) {
 *     // File 42 is valid
 * }
 * 
 * it_remove_entry(it, 42); // Mark file ID 42 as invalid
 * it_destroy(it);
 * @endcode
 */

#ifndef INDEX_TABLE_H
#define INDEX_TABLE_H

#include <stdbool.h>
#include <unistd.h>

/**
 * @brief Opaque structure for bit-based index table
 *
 * Maintains a compact array where each bit represents a file's validity:
 * - 1 = valid/exists
 * - 0 = invalid/removed
 * Provides constant-time operations for validity checks and updates.
 */
typedef struct index_table Index_Table;

/**
 * @brief Creates a new empty index table
 *
 * @return Pointer to a newly allocated index table
 * @retval NULL if memory allocation fails
 *
 * @note Initializes all bits to 0 (invalid)
 * @note Must be paired with it_destroy()
 */
Index_Table *it_create(void);

/**
 * @brief Destroys an index table and releases all resources
 *
 * @param it Pointer to the index table to destroy
 *
 * @note Safe to call with NULL (no operation performed)
 */
void it_destroy(Index_Table *it);

/**
 * @brief Marks a file ID as valid in the index
 *
 * @param it Pointer to the index table
 * @param id File ID to mark as valid
 * @return Operation status
 * @retval 0 on success
 * @retval -1 on invalid input (NULL it or negative id)
 * @retval 1 on allocation errors
 *
 * @note Sets the corresponding bit to 1
 * @note Automatically expands storage if needed
 */
int it_add_entry(Index_Table *it, int id);

/**
 * @brief Marks a file ID as invalid in the index
 *
 * @param it Pointer to the index table
 * @param id File ID to mark as invalid
 * @return Document identifier
 * @retval -1 on invalid input or document not found
 *
 * @note Sets the corresponding bit to 0
 */
int it_remove_entry(Index_Table *it, int id);

/**
 * @brief Checks if a file ID is marked valid
 *
 * @param it Pointer to the index table
 * @param id File ID to check
 * @return Validity status
 * @retval true if the file is valid (bit is 1)
 * @retval false if invalid, not found, or NULL table
 */
bool it_entry_is_valid(const Index_Table *it, int id);

/**
 * @brief Returns the number of valid files in the table
 *
 * @param it Pointer to the index table
 * @return Count of valid files (bits set to 1)
 * @retval 0 if it is NULL or no valid files
 */
unsigned it_size(const Index_Table *it);

/**
 * @brief Checks if the table has any valid files
 *
 * @param it Pointer to the index table
 * @return Table emptiness status
 * @retval true if NULL, empty, or no valid files
 * @retval false if at least one valid file exists
 */
bool it_is_empty(const Index_Table *it);

/**
 * @brief Debug function to print table contents
 *
 * @param it Pointer to the index table
 *
 * @note Prints binary representation of the bit array
 * @note No effect if it is NULL
 */
void it_show(const Index_Table *it);

/**
 * @brief Loads an index table from a file
 *
 * @param file Open file descriptor to read from
 * @return Pointer to the loaded index table
 * @retval NULL on read error or invalid input
 *
 * @note Must be paired with it_destroy()
 */
Index_Table *it_upload(int file);

/**
 * @brief Saves an index table to a file
 *
 * @param it Pointer to the index table to save
 * @param file Open file descriptor to write to
 * @return Operation status
 * @retval 0 on success
 * @retval -1 on failure
 *
 * @note Writes compact binary format (bits packed into bytes)
 */
int it_record(const Index_Table *it, int file);

/**
 * @brief Retrieves array of all valid file IDs
 *
 * @param it Pointer to the index table
 * @return Dynamically allocated array of valid IDs
 * @retval NULL if no valid IDs or allocation failed
 *
 * @note Caller must free the returned array
 * @note Array is terminated with -1
 */
int *it_get_valid_ids(const Index_Table *it);

#endif