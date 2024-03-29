#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
#include "includes/socket.h"
#include "includes/db.h"
#include "includes/serverRequestHandler.h"
#include "includes/dbSincronization.h"
#include "includes/serverUi.h"

#define TRUE 1

void checkError(ResponseCode code);
void checkDbError(DbCode code);
void handleNewConnection(Socket_t socket);
void * newThread(void * data);
int validateParameters(int argc, char * argv[]);

Db_t db;

int main(int argc, char * argv[]) {
	Socket_t socket;
	ResponseCode resp;
	DbCode dbCode;
	int port = validateParameters(argc, argv);

	printf("\033[1;32m[Working on port %d]\033[0m\n", port);

	/* Create server socket */
	resp = serverInit(&socket, port);
	checkError(resp);

	/* Create a db connection */
	dbCode = dbInit(&db);
	checkDbError(dbCode);

	/* Initialize sincro */
	sincroInit();

	/* Ui init */
	uiInit();

	/* Wait for new client */
	while(TRUE) {
		resp = waitForClient(socket);
		checkError(resp);

		handleNewConnection(socket);
	}

	closeSocket(socket);
	dbClose(db);
	sincroDestroy();

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
	addUser();
}

void * newThread(void * data) {
	Socket_t socket = (Socket_t) data;

	handleRequests(socket, db);

	/* End socket */
	closeCommunicationSocket(socket);

	printf("\033[0;32m*** Connection closed ***\033[0m\n");
	removeUser();
	return 0;
}

int validateParameters(int argc, char * argv[]) {
	int port;
	if(argc != 2) {
		printf("\n\033[0;31mInvalid parameters, usage: server <port>\033[0m\n");
		exit(1);
	}
	
	if(sscanf(argv[1], "%d", &port) == 0) {
		printf("\n\033[0;31mParameter port should be an integer\033[0m\n");
		exit(1);
	}
	return port;
}