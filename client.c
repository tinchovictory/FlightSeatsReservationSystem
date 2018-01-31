#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/socket.h"
#include "includes/gui.h"

#include "includes/clientRequestBuilder.h"

#define PORT 8000


void checkError(ResponseCode code);
void clientApp(Socket_t  socket);


void printReservations(Socket_t socket, int flightNo);//TESTING
void printCanReservations(Socket_t socket, int flightNo);//TESTING
void printSeats(Socket_t socket, int flightNo);//TESTING


int main(int argc, char * argv[]) {

	Socket_t socket;
	ResponseCode resp;

	/* Create client socket */
	resp = clientInit(&socket, "localhost", PORT);
	checkError(resp);

	printf("Connection established to server\n");

	app(socket);
	 //clientApp(socket);

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
/*	char * ptr = msg1 +1;

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

/*

  sendMsg(socket,msg1,1000);

  n=recvMsg(socket,msg2,1000);

  printf("Receive message from  server:%s\n",msg2[0] == '0' ? "OK" : "ERR");
 */

/* Add flight *
	FlightObj * flight = malloc(sizeof(FlightObj));
	flight->flightNo = 101;
	flight->departure = "New York";
	flight->arrival = "London";
	flight->price = 1000;
	flight->seats = 50;
	flight->date = "31/01/2018";


	if(addFlight(socket, flight)) {
		printf("Flight %d added\n", flight->flightNo);
	} else {
		printf("Fail to add\n");
	}

	free(flight);

/* Show flights */

/*	if(deleteFlight(socket, 0)) {
		printf("Flight was removed\n");
	}

*/
	ListPtr flightsList = getFlights(socket);
	if(flightsList == NULL) {
		printf("Fail to get flights\n");
	}

	ListIteratorPtr flightIterator = listIterator(flightsList);
	FlightObj * flight = malloc(sizeof(FlightObj));
	if(!iteratorHasNext(flightIterator)) {
		printf("aca\n");
	}
	while(iteratorHasNext(flightIterator)) {
		iteratorGetNext(flightIterator, flight);
		printf("Flight: %d, from: %s, to: %s\n",flight->flightNo, flight->departure, flight->arrival);
	}
	free(flight);
	freeIterator(flightIterator);
	freeList(flightsList);

	printf("done\n");
/* Book flight */
/*
	ReservationObj * reserv = malloc(sizeof(ReservationObj));
	reserv->flightNo = 101;
	reserv->name = "Martin Victory";
	reserv->seat = "1E";

	if(bookFlight(socket, reserv)) {
		printf("Reservation Added\n");
	}

	cancelReservation(socket, 1);
*/
/*
	printf("\n*** Reservations for flight 101 ***\n\n");

	printReservations(socket, 101);

	printf("\n*** Candelled Reservations for flight 101 ***\n\n");

	printCanReservations(socket, 101);
*/

	printf("\n*** Sold seats for flight 101 ***\n\n");
	printSeats(socket, 101);
}


	/***** END TESTING*****/
}

void printReservations(Socket_t socket, int flightNo) {
	ListPtr list = getReservations(socket, flightNo);
	if(list == NULL) {
		printf("Fail to get reservations\n");
	}

	ListIteratorPtr iterator = listIterator(list);
	ReservationObj * reserv = malloc(sizeof(ReservationObj));
	if(!iteratorHasNext(iterator)) {
		printf("aca\n");
	}
	while(iteratorHasNext(iterator)) {
		iteratorGetNext(iterator, reserv);
		printf("Reservation: %d, Flight: %d, Name: %s, Seat: %d\n", reserv->reservationNo, reserv->flightNo, reserv->name, reserv->seat);
	}
	free(reserv);
	freeIterator(iterator);
	freeList(list);
}

void printCanReservations(Socket_t socket, int flightNo) {
	ListPtr list = getCanReservations(socket, flightNo);
	if(list == NULL) {
		printf("Fail to get reservations\n");
	}

	ListIteratorPtr iterator = listIterator(list);
	ReservationObj * reserv = malloc(sizeof(ReservationObj));
	if(!iteratorHasNext(iterator)) {
		printf("aca\n");
	}
	while(iteratorHasNext(iterator)) {
		iteratorGetNext(iterator, reserv);
		printf("Reservation: %d, Flight: %d, Name: %s, Seat: %d\n", reserv->reservationNo, reserv->flightNo, reserv->name, reserv->seat);
	}
	free(reserv);
	freeIterator(iterator);
	freeList(list);
}

void printSeats(Socket_t socket, int flightNo) {
	ListPtr list = getSeats(socket, flightNo);
	if(list == NULL) {
		printf("Fail to get seats\n");
	}

	ListIteratorPtr iterator = listIterator(list);
	FlightSeatObj * seat = malloc(sizeof(FlightSeatObj));
	if(!iteratorHasNext(iterator)) {
		printf("aca\n");
	}
	while(iteratorHasNext(iterator)) {
		iteratorGetNext(iterator, seat);
		printf("Flight: %d, Seat: %d\n", seat->flightNo, seat->seat);
	}
	free(seat);
	freeIterator(iterator);
	freeList(list);
}