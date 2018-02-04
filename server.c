#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
#include "includes/socket.h"
#include "includes/db.h"
#include "includes/serverRequestHandler.h"

#define PORT 8000
#define TRUE 1

void checkError(ResponseCode code);
void checkDbError(DbCode code);
void handleNewConnection(Socket_t socket);
void * newThread(void * data);

Db_t db;

int main(int argc, char * argv[]) {
	printf("\r Working\n");

	Socket_t socket;
	ResponseCode resp;
	//Db_t db;
	DbCode dbCode;

	/* Create server socket */
	resp = serverInit(&socket, PORT);
	checkError(resp);

	/* Create a db connection */
	dbCode = dbInit(&db);
	checkDbError(dbCode);

	/* Wait for new client */
	while(TRUE) {
		resp = waitForClient(socket);
		checkError(resp);

		handleNewConnection(socket);
	}

	closeSocket(socket);
	dbClose(db);

 	return 0;
}



void checkError(ResponseCode code) {
	if(code == NOERR) {
		return;
	}

	printf("Fail\n");
	exit(1);
}

void checkDbError(DbCode code) {
	if(code == DB_OK) {
		return;
	}

	printf("Db fail\n");
	exit(1);
}

void handleNewConnection(Socket_t socket) {
	pthread_t thread;

	Socket_t newSocket = clientSocket(socket);

	if(pthread_create(&thread, NULL, newThread, newSocket) == 0) {
		pthread_detach(thread);
		printf("*** New connection established ***\n");
	}
}

void * newThread(void * data) {
	Socket_t socket = (Socket_t) data;

	handleRequests(socket, db);

	/* End socket */
	closeCommunicationSocket(socket);

	printf("*** Connection closed ***\n");
	return 0;
}