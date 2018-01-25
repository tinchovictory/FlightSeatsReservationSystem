#include <stdio.h>
#include <stdlib.h>

#include "socket.h"

#define PORT 8000
#define TRUE 1

void checkError(ResponseCode code);
void handleNewConnection(Socket_t socket);
void handleClientRequests(Socket_t socket);


int main(int argc, char * argv[]) {

	Socket_t socket;
	ResponseCode resp;

	/* Create server socket */
	resp = serverInit(&socket, PORT);
	checkError(resp);

	/* Wait for new client */
	while(TRUE) {
		resp = waitForClient(socket);
		checkError(resp);

		/* Fork process */
		handleNewConnection(socket);
	}

 	return 0;
}



void checkError(ResponseCode code) {
	if(code == NOERR) {
		return;
	}

	printf("Fail\n");
	exit(1);
}

void handleNewConnection(Socket_t socket) {

	printf("*** New connection established ***\n");
	
	int childPid = fork();

	if(childPid == 0) {
		/* Code for child process */

		handleClientRequests(socket);

		printf("*** Connection closed ***\n");
		exit(0);

	} else {
		/* Code for parent process */

		/* End socket */
		closeCommunicationSocket(socket);
	}
}

void handleClientRequests(Socket_t socket) {




/***** TESTING *****/

int n;
char msg[1000] = {0};

while(1)
   {
    n=recvMsg(socket,msg,1000);
    if(n==0)
    {
     break;
    }
    msg[n]=0;
    sendMsg(socket,msg,n);

    printf("Receive and set:%s\n",msg);
   }

/********* END TESTING *************/





	/* End socket */
 	closeCommunicationSocket(socket);
}