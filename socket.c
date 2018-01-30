#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "includes/socket.h"

typedef struct Socket {
	int socketFd, newSocketFd;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;

} Socket;


Socket * allocateSocket();
ResponseCode createSocket(Socket_t socket);
void setServerAddress(Socket_t socket, int port);
ResponseCode bindSocket(Socket_t socket);
ResponseCode getServerAddress(Socket_t socket, char * hostName, int port);
ResponseCode connectServer(Socket_t socket);



/*
 * Start the server side of the socket on PORT. 
 * Returns NOERR if succeed, or the error code if not.
 */
ResponseCode serverInit(Socket_t * socketAddr, int port) {

	Socket_t socket;
	ResponseCode resp;

	/* Allocate socket structure */
	socket = allocateSocket();

	/* Create socket */
	resp = createSocket(socket);
	if(resp != NOERR) {
		return resp; 
	}

	/* Set server address */
	setServerAddress(socket, port);

	/* Bind socket */
	resp = bindSocket(socket);
	if(resp != NOERR) {
		return resp; 
	}

	/* Listen for connections */
 	listen(socket->socketFd, NUMBERCONNECTIONS);

 	*socketAddr = socket;

 	return NOERR;
}



/*
 *
 */
ResponseCode waitForClient(Socket_t socket) {

 	int clientAddressLength = sizeof(socket->clientAddress);
 	
 	socket->newSocketFd = accept(socket->socketFd, (struct sockaddr*) &(socket->clientAddress), &clientAddressLength);

 	if(socket->newSocketFd == -1) {
 		return CONNERR;
 	}
 			//printf("New client connected: %s\n", inet_ntoa(clientAddress.sin_addr));
 	return NOERR;
}


/*
 *
 */
void closeCommunicationSocket(Socket_t socket) {
	close(socket->newSocketFd);
}



/*
 *
 */
int sendMsg(Socket_t socket, char * msg, int length) {
	return send(socket->newSocketFd,msg,length,0);
}



/*
 *
 */
int recvMsg(Socket_t socket, char * msg, int length) {
	return recv(socket->newSocketFd,msg,length,0);
}


/*
 * Allocate socket structure in memory.
 */
Socket * allocateSocket() {
	return (Socket *) malloc(sizeof(Socket));
} 


/*
 * Creates the socket. Saves the file descriptor in socket struct.
 * Returns NOERR if succeed, or SOCKERR if not.
 */
ResponseCode createSocket(Socket_t sock) {
	sock->socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock->socketFd < 0) {
		return SOCKERR;
	}
	return NOERR;
}


/*
 * Set the server address and port.
 */
void setServerAddress(Socket_t socket, int port) {

	/* Initialize server address with 0 */
	memset(&(socket->serverAddress), 0, sizeof(socket->serverAddress));

	/* Set server address */
	socket->serverAddress.sin_family = AF_INET;
	socket->serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
 	socket->serverAddress.sin_port = htons(port);
}


/*
 * Bind the socket.
 * Returns NOERR if succedd, or BINDERR if not.
 */
ResponseCode bindSocket(Socket_t socket) {
	if(bind(socket->socketFd, (struct sockaddr *) &(socket->serverAddress), sizeof(socket->serverAddress))) {
 		return BINDERR;
 	}
 	return NOERR;
}



/*
 *
 */
ResponseCode clientInit(Socket_t * socketAddr, char * hostName, int port) {
	Socket_t socket;
	ResponseCode resp;

	/* Allocate socket structure */
	socket = allocateSocket();

	/* Create socket */
	resp = createSocket(socket);
	if(resp != NOERR) {
		return resp; 
	}

	/* Solve communication problems */
	socket->newSocketFd = socket->socketFd;

	/* Initialize server address */
	resp = getServerAddress(socket, hostName, port);
	if(resp != NOERR) {
		return resp; 
	}
	
	/* Connect to server */
	resp = connectServer(socket);
	if(resp != NOERR) {
		return resp; 
	}

	*socketAddr = socket;
	
	return NOERR;
}



/*
 * 
 */
ResponseCode getServerAddress(Socket_t socket, char * hostName, int port) {
	struct hostent * hostNameS;

	memset(&(socket->serverAddress), 0, sizeof(socket->serverAddress));

	socket->serverAddress.sin_family = AF_INET;
	socket->serverAddress.sin_port = htons(port);

	/* Get host name */
	hostNameS = gethostbyname(hostName);
	if(hostNameS == 0) {
		return HOSTNAMEERR;
	}

	memcpy(&(socket->serverAddress.sin_addr), hostNameS->h_addr, hostNameS->h_length);

	return NOERR;
}



/*
 * 
 */
ResponseCode connectServer(Socket_t socket) {
	if( connect(socket->socketFd, (struct sockaddr *) &(socket->serverAddress), sizeof(socket->serverAddress)) < 0 ) {
		return CONNERR;
	}
	return NOERR;
}


/*
 *
 */
void closeSocket(Socket_t socket) {
	close(socket->socketFd);
}