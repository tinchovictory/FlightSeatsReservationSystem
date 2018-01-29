#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/socket.h"
#include "includes/gui.h"

#define PORT 8001

int addToStr(char * ptr, char * str);//TESTING


void checkError(ResponseCode code);
void clientApp(Socket_t  socket);


int main(int argc, char * argv[]) {

	Socket_t socket;
	ResponseCode resp;

	/* Create client socket */
	resp = clientInit(&socket, "localhost", PORT);
	checkError(resp);

	printf("Connection established to server\n");

	menu();
	// For testing sockets:  clientApp(socket);

	return 0;
}




/*
 *
 */
void checkError(ResponseCode code) {
	if(code == NOERR) {
		return;
	}

	printf("Failed to connect, please check if the server is working as expected!\n");
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

  //n=strlen(msg1);
  //msg1[n-1] = 0;
	char * ptr = msg1 +1;

	memset(msg1, 0, 1000);
	msg1[0] = '0';
	ptr += 1 + addToStr(ptr, "125");
	ptr += 1 + addToStr(ptr, "Buenos Aires");
	ptr += 1 + addToStr(ptr, "JFK");
	ptr += 1 + addToStr(ptr, "100");
	ptr += 1 + addToStr(ptr, "50");
	ptr += 1 + addToStr(ptr, "31/01/2018");

/*
int flightNo;
	char * departure;
	char * arrival;
	int price;
	int seats;
	char * date;
*/


  sendMsg(socket,msg1,1000);

  n=recvMsg(socket,msg2,1000);

  printf("Receive message from  server:%s\n",msg2[0] == '0' ? "OK" : "ERR");
 }


	/***** END TESTING*****/
}

int addToStr(char * ptr, char * str) {
	int i = 0;
	while(str[i]) {
		ptr[i] = str[i];
		i++;
	}
	return i;
}