#ifndef CLIENT_OPS_H
#define CLIENT_OPS_H

#include "defs.h"

Operation check_operation(const char *opr);
int define_request(Request *request, int argc, char **argv);

void show_reply(Operation op, const void * reply);



#endif