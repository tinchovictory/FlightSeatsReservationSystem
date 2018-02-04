#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/socket.h"
#include "includes/gui.h"

#include "includes/clientRequestBuilder.h"

#define PORT 8000


void checkError(ResponseCode code);


int main(int argc, char * argv[]) {

	Socket_t socket;
	ResponseCode resp;

	/* Create client socket */
	resp = clientInit(&socket, "localhost", PORT);
	checkError(resp);

	app(socket);

	return 0;
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