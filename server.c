#include <stdio.h>
#include <stdlib.h>

#include "includes/socket.h"
#include "includes/db.h"

#define PORT 8000
#define TRUE 1

void checkError(ResponseCode code);
void checkDbError(DbCode code);
void handleNewConnection(Socket_t socket);
void handleClientRequests(Socket_t socket);

int main(int argc, char * argv[]) {
	printf("\r Working\n");

	Socket_t socket;
	ResponseCode resp;
	Db_t db;
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

		/* Fork process */
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