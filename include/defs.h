/**
 * @file defs.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Global definitions for client-server communication system
 * 
 * This header contains shared definitions between client and server components,
 * including FIFO names, file paths, buffer sizes, request operations, and the
 * request structure format. These definitions ensure consistency in inter-process
 * communication.
 * 
 */

#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <unistd.h>

/* Named pipe (FIFO) paths */
#define SERVER_FIFO "tmp/server_fifo"    /**< Server's receiving FIFO path */
#define CLIENT_FIFO "tmp/client_fifo"    /**< Client's receiving FIFO path */

/* Data storage files */
#define STORAGE_FILE "tmp/metadata.bin"          /**< Main data storage file */
#define CONTROL_FILE "tmp/metadata_control.bin"  /**< Control file for synchronization */

/* Logging file */
#define REQUESTS_LOG "tmp/requests.log"  /**< Server request log file path */

/* System constants */
#define BLOCK_SIZE 8                     /**< Basic I/O block size in bytes */

/* Field size definitions */
#define TITLE_SIZE 200   /**< Maximum length for title field (including null terminator) */
#define AUTHORS_SIZE 200 /**< Maximum length for authors field (including null terminator) */
#define YEAR_SIZE 4      /**< Size for year field (format YYYY + null terminator) */
#define PATH_SIZE 64     /**< Maximum length for file path field */

/**
 * @brief Enumeration of supported operations
 * 
 * Defines all possible request types that clients can send to the server.
 */
typedef enum {
    INDEX,          /**< Index a new document */
    REMOVE,         /**< Remove a document from index */
    CONSULT,        /**< Consult document metadata */
    COUNT_WORD,     /**< Count occurrences of a word in a document */
    LIST_WORD,      /**< List documents containing a word */
    SHUTDOWN,       /**< Graceful server shutdown */
    KILL,           /**< Tell the server, child work is done */
} Operation;

/**
 * @brief Request structure for client-server communication
 * 
 * This structure represents the complete request format sent from clients to
 * the server through the FIFO. All fields must be properly filled according
 * to the operation type.
 */
typedef struct {
    pid_t client;               /**< Client process ID for response routing */
    Operation operation;        /**< Requested operation type */
    char title[TITLE_SIZE];     /**< Document title field, Document ID, Keyword */
    char authors[AUTHORS_SIZE]; /**< Document authors field, Keyword, Number of processes */
    char year[YEAR_SIZE];       /**< Document publication year */
    char path[PATH_SIZE];       /**< Document file path */
} Request;

#endif /* DEFS_H */