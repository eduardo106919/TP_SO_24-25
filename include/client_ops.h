/**
 * @file client_ops.h
 * @author Eduardo Freitas Fernandes (ef05238@gmail.com)
 * @brief Client operation utilities for the client-server system
 * 
 * This header provides functions for processing client operations, including
 * operation validation, request construction, and server reply handling.
 * These utilities are used by client applications to prepare requests and
 * interpret server responses.
 * 
 */

#ifndef CLIENT_OPS_H
#define CLIENT_OPS_H

#include "defs.h"

/**
 * @brief Determines the operation type from a string
 * 
 * Maps a string command to the corresponding Operation enum value.
 * 
 * @param opr String representation of the operation (e.g., "-a", "-c")
 * @retval Operation Corresponding operation enum value
 * @retval -1 If the string doesn't match any known operation
 * @note Comparison is case-sensitive
 */
Operation check_operation(const char *opr);

/**
 * @brief Constructs a Request from command line arguments
 * 
 * Parses command line arguments and fills the Request structure according
 * to the specified operation. Validates argument count and formats fields.
 * 
 * @param[out] request Pointer to the Request structure to fill (output parameter)
 * @param argc Argument count from main()
 * @param argv Argument vector from main()
 * @retval 0 Request was successfully constructed
 * @retval 1 Invalid arguments or operation type
 */
int define_request(Request *request, int argc, char **argv);

/**
 * @brief Displays server reply according to operation type
 * 
 * Formats and prints the server's response in an operation-specific manner.
 * 
 * @param op Operation type that was originally requested
 * @param reply Pointer to the server's reply data
 */
void show_reply(Operation op, const void *reply);

#endif /* CLIENT_OPS_H */