#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/socket.h"
#include "includes/gui.h"

#include "includes/clientRequestBuilder.h"


void checkError(ResponseCode code);
int validateParameters(int argc, char * argv[]);


int main(int argc, char * argv[]) {

	int port;
	port = validateParameters(argc, argv);

	Socket_t socket;
	ResponseCode resp;

	/* Create client socket */
	resp = clientInit(&socket, argv[1], port);
	checkError(resp);

	app(socket);

	return 0;
}


int validateParameters(int argc, char * argv[]) {
	int port;
	if(argc != 3) {
		printf("\n\033[0;31mInvalid parameters, usage: client <hostname> <port>\nIf you are running in the same computer as the server, use hostname: localhost\033[0m\n");
		exit(1);
	}
	
	if(sscanf(argv[2], "%d", &port) == 0) {
		printf("\n\033[0;31mParameter port should be an integer\033[0m\n");
		exit(1);
	}
	return port;
}


/*
 *
 */
void checkError(ResponseCode code) {
	if(code == NOERR) {
		return;
	}

	printf("\n\033[0;31mFailed to connect, please check if the server is working as expected!\n\033[0m");
	exit(1);
}