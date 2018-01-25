#ifndef __SOCKETS__
#define __SOCKETS__

#define NUMBERCONNECTIONS 10

typedef struct Socket * Socket_t;

typedef enum {NOERR, SOCKERR, BINDERR, CONNERR, HOSTNAMEERR} ResponseCode;

ResponseCode serverInit(Socket_t * socket, int port);

ResponseCode clientInit(Socket_t * socket, char * hostName, int port);

ResponseCode waitForClient(Socket_t socket);

void closeCommunicationSocket(Socket_t socket);

int sendMsg(Socket_t socket, char * msg, int length);

int recvMsg(Socket_t socket, char * msg, int length);

void closeSocket(Socket_t socket);

#endif