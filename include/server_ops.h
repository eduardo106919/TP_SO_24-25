/**
 * @file server_ops.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Server operation interface for the document management system
 *
 * Defines the core server operations including initialization, request processing,
 * and shutdown procedures. The server handles client requests and manages
 * document storage with optional caching.
 *
 */

#ifndef SERVER_OPS_H
#define SERVER_OPS_H

#include "cache.h"
#include "defs.h"

/**
 * @brief Opaque server structure
 *
 * Contains all server state including:
 * - Document storage
 * - Cache configuration
 */
typedef struct server Server;

/**
 * @brief Initializes and starts the document server
 *
 * @param document_folder Root directory for document storage
 * @param cache_size Maximum number of items in cache
 * @param type Cache replacement strategy (FIFO/RAND/LRU)
 * @return Pointer to initialized server instance
 * @retval NULL If initialization fails (invalid params or system error)
 * 
 * @note Cache type NONE disables the use of cache
 * @note Must be paired with shutdown_server()
 */
Server *start_server(const char *document_folder, int cache_size,
                     Cache_Type type);

/**
 * @brief Processes a client request and sends response
 *
 * Handles all request types defined in defs.h (INDEX, REMOVE, CONSULT, etc.)
 *
 * @param server Server instance
 * @param request Client request to process
 * @return Server control signal
 * @retval 0 Continue normal operation
 * @retval 1 Initiate graceful shutdown
 * @retval -1 Error occurred (logged internally)
 *
 * @note All responses are sent through CLIENT_FIFO
 */
int process_request(Server *server, const Request *request);

/**
 * @brief Shuts down the server and releases all resources
 *
 * @param server Server instance to shutdown
 *
 * @note Safe to call with NULL
 */
void shutdown_server(Server *server);

#endif /* SERVER_OPS_H */