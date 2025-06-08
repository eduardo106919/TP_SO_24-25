/**
 * @file document.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Document management interface for the client-server system
 * 
 * Defines the Document structure and operations for creating, managing,
 * and displaying document metadata. This interface handles the core
 * data type used throughout the system.
 * 
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "defs.h"

/**
 * @brief Document metadata structure
 * 
 * Contains all metadata fields for documents managed by the system.
 * Field sizes are defined by constants from defs.h.
 */
typedef struct document {
    char title[TITLE_SIZE];     /**< Document title */
    char authors[AUTHORS_SIZE]; /**< Document authors */
    char year[YEAR_SIZE];       /**< Publication year */
    char path[PATH_SIZE];       /**< Filesystem path to document */
} Document;

/**
 * @brief Creates a new Document instance
 * 
 * Allocates and initializes a new Document with the given metadata.
 * All string fields are copied to the new structure.
 * 
 * @param title Document title
 * @param authors Document authors
 * @param year Publication year
 * @param path Filesystem path to document
 * @retval Document* Pointer to newly created Document
 * @retval NULL If memory allocation fails or inputs are invalid
 * @note The returned pointer must be freed with destroy_document()
 */
Document *create_document(const char *title, const char *authors,
                          const char *year, const char *path);

/**
 * @brief Destroys a Document instance
 * 
 * Deallocates memory used by a Document created with create_document().
 * 
 * @param doc Document to destroy (pointer is invalid after call)
 * @note Safe to call with NULL (no operation performed)
 */
void destroy_document(Document *doc);

/**
 * @brief Creates a deep copy of a Document
 * 
 * Allocates a new Document with identical contents to the source.
 * 
 * @param doc Document to clone
 * @retval Document* New copy of the Document
 * @retval NULL If memory allocation fails or input is NULL
 * @note The clone must eventually be destroyed with destroy_document()
 */
Document *clone_document(const Document *doc);

/**
 * @brief Displays document metadata
 * 
 * Prints formatted document information to standard output.
 * 
 * @param doc Document to display
 */
void show_document(const Document *doc);

#endif /* DOCUMENT_H */