#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "socket.h"

#define PORT 8000


void checkError(ResponseCode code);
void clientApp(Socket_t  socket);


int main(int argc, char * argv[]) {

	Socket_t socket;
	ResponseCode resp;

	/* Create client socket */
	resp = clientInit(&socket, "localhost", PORT);
	checkError(resp);

	printf("Connection established to server\n");


	clientApp(socket);

	return 0;
}



/*
 *
 */
void checkError(ResponseCode code) {
	if(code == NOERR) {
		return;
	}

	printf("Fail\n");
	exit(1);
}



/*
 *
 */
void clientApp(Socket_t socket) {
	/***** TESTING *****/

char msg1[1000] = {0};
char msg2[1000] = {0};
int n;

while(1)
 {
  printf("Enter message to send to server:");
  fgets(msg1,1000,stdin);
  if(msg1[0]=='#')
   break;

  n=strlen(msg1);
  msg1[n-1] = 0;
  sendMsg(socket,msg1,n);

  n=recvMsg(socket,msg2,1000);

  printf("Receive message from  server:%s\n",msg2);
 }


	/***** END TESTING*****/
}