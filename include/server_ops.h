#ifndef SERVER_OPS_H
#define SERVER_OPS_H

#include "defs.h"
#include "cache.h"


typedef struct server Server;

Server * start_server(const char *document_folder, int cache_size, Cache_Type type);
int process_request(Server * server, const Request *request);
void shutdown_server(Server * server);


#endif