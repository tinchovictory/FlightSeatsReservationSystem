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
	printf("\033[1;32m[Working]\033[0m\n");

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

	if(code == SOCKERR) {
		printf("\n\033[0;31mFatal error while creating socket.\033[0m\n");
	} else if(code == BINDERR) {
		printf("\n\033[0;31mFatal error while binding socket.\033[0m\n");
	} else {
		printf("\n\033[0;31mUnknown fatal error while initializing socket.\033[0m\n");
	}
	printf("\n\033[0;31mExiting...\033[0m\n");
	exit(1);
}

void checkDbError(DbCode code) {
	if(code == DB_OK) {
		return;
	}
	if(code == DB_CONNERR) {
		printf("\n\033[0;31mFatal error while connecting to the data base.\033[0m\n");
	} else {
		printf("\n\033[0;31mUnknown fatal error while connecting to the data base.\033[0m\n");
	}
	printf("\n\033[0;31mExiting...\033[0m\n");
	exit(1);
}

void handleNewConnection(Socket_t socket) {
	pthread_t thread;

	Socket_t newSocket = clientSocket(socket);

	if(pthread_create(&thread, NULL, newThread, newSocket) == 0) {
		pthread_detach(thread);
		printf("\033[0;32m*** New connection established ***\033[0m\n");
	}
}

void * newThread(void * data) {
	Socket_t socket = (Socket_t) data;

	handleRequests(socket, db);

	/* End socket */
	closeCommunicationSocket(socket);

	printf("\033[0;32m*** Connection closed ***\033[0m\n");
	return 0;
}