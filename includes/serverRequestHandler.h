#ifndef __SERVER_REQUEST_HANDLER_H__
#define __SERVER_REQUEST_HANDLER_H__

#include "socket.h"
#include "db.h"

void handleRequests(Socket_t socket, Db_t db);

#endif