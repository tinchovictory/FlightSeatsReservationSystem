#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/gui.h"
#include "includes/clientRequestBuilder.h"
#include "includes/stringUtils.h"
#include "includes/list.h"
#include "includes/dbObj.h"
#include "includes/guiUtils.h"

#define ROWQUANTITY 3

void flow();
void printMenu();
void flightStatus();
void manageflights();
void makeReservation();
void manageReservations();
void removeReservation();
void printFlights();
void printFlightInfo(int flightNo);
void printDAV(int flightNo);
int printSeatError(int seatResp);
int printReservations(int flightNo);
void printAllReservations(int flightNo);
int readValidFlight();
int readValidReservation(int flightNo);

static Socket_t  generalSocket;

void app(Socket_t  socket) {
	generalSocket = socket;
	flow();
}


void flow() {
	int selected;

	printf("\n\n\033[01;33m*** Welcome to the reservations system! ***\033[0m\n");

	printMenu();

	while( (selected = readInt()) != 6) {
		switch(selected){
			case 1: 
				flightStatus();
				break;
			case 2: 
				makeReservation();
				break;
			case 3: 
				removeReservation();
				break;
			case 4:
				manageReservations();
				break;
			case 5: 
				manageflights();
				break;
			default:
				printf("\n\033[0;31mInvalid option, please try again.\033[0m\n");
				break;
		}

		printMenu();
	}
	printf("\n\033[01;33m*** Goodbye! ***\033[0m\n\n");
}

void printMenu() {
	printf("\n\033[01;33mPlease select an option:\n");
	printf("\033[1;35m (1) Flight Status\n (2) Make a reservation\n (3) Remove a reservation\n (4) Reservations\n (5) Manage Flights\n (6) Exit\n\033[0m");	
}

void flightStatus() {
	int input;

	if(!flightsLoaded(generalSocket)) {
		printf("\n\033[0;31mThere are no flights loaded yet.\n\033[0m");
		return;
	}
	
	printFlights();
	printf("\n\033[0;32mPlease enter the flight number.\n\033[0m");
	input = readValidFlight();
	
	printFlightInfo(input);

	printDAV(input);
}

/* Handle add and remove flights */
void manageflights() {
	char c;
	int selectedMenu, flightNo, flightsLoad;
	FlightObj * flight;
	char buff[STRINGMAXLENGTH] = {0};

	flightsLoad = flightsLoaded(generalSocket);

	do {
		printf("\n\033[0;32m (1) Add\n\033[0m");
		if(flightsLoad) {
			printf("\033[0;32m (2) Remove\n\033[0m");
		}
		printf("\033[0;32m (3) Cancel\n\033[0m");
	} while((selectedMenu = readInt()) != 1 && (selectedMenu != 2 || !flightsLoad) && selectedMenu != 3);

	if(selectedMenu == 3) {
		return;
	}
	
	if(selectedMenu == 1) {
		/* Add flight */
		flight = malloc(sizeof(FlightObj));

		printf("\n\033[0;32mInsert flight number\n\033[0m");
		flight->flightNo = readInt();
		while(checkFlight(generalSocket, flight->flightNo)) {
			printf("\n\033[0;31mFlight %d alredy exist, please insert an other flight number.\n\033[0m", flight->flightNo);
			flight->flightNo = readInt();
		}

		printf("\n\033[0;32mInsert departure\n\033[0m");
		flight->departure = readAndCpyStr();

		printf("\n\033[0;32mInsert arrival\n\033[0m");
		flight->arrival = readAndCpyStr();

		printf("\n\033[0;32mInsert price\n\033[0m");
		flight->price = readInt();
		
		printf("\n\033[0;32mInsert seats\n\033[0m");
		flight->seats = readInt();

		printf("\n\033[0;32mInsert date (dd/mm/yyyy)\n\033[0m");
		flight->date = readDate();

		if(addFlight(generalSocket, flight)) {
			printf("\n\033[0;32mFlight %d added\n\033[0m", flight->flightNo);
		} else {
			printf("\n\033[0;31mFail to add.\n\033[0m");
		}

		free(flight);

	} else {
		/* Remove flight */
		printFlights();

		printf("\n\033[0;32mPlease enter the flight number.\n\033[0m");
		flightNo = readValidFlight();

		printFlightInfo(flightNo);
		deleteFlight(generalSocket, flightNo);
		printf("\n\033[0;32mDone\n\033[0m");
	}
}


void makeReservation() {
	ReservationObj * reserv;
	int seatResp, done = 0;

	if(!flightsLoaded(generalSocket)) {
		printf("\n\033[0;31mThere are no flights loaded yet.\n\033[0m");
		return;
	}

	reserv = malloc(sizeof(ReservationObj));

	printFlights();
	printf("\n\033[0;32mPlease select a flight.\n\033[0m");
	reserv->flightNo = readValidFlight();

	printf("\n\033[0;32mEnter name.\n\033[0m");
	reserv->name = readAndCpyStr();

	/* Most common error is taken seat, prevent it */
	do {
		printDAV(reserv->flightNo);
		printf("\n\033[0;32mEnter seat.\n\033[0m");
		reserv->seat = readInt();

		/* Validate seat */
		while((seatResp = checkSeat(generalSocket, reserv->flightNo, reserv->seat)) != SEAT_OK) {
			if(seatResp == SEAT_TAKEN) {
				printDAV(reserv->flightNo);
			}
			if(printSeatError(seatResp)) {
				/* Fatal error */
				free(reserv);
				return;
			}
			reserv->seat = readInt();
		}

		/* Save to db */
		if(bookFlight(generalSocket, reserv)) {
			/* Success */
			printf("\n\033[0;32mReservation added.\n\033[0m");
			done = 1;
		} else {
			/* Fail, check common error: seat taken */
			seatResp = checkSeat(generalSocket, reserv->flightNo, reserv->seat);
			if(printSeatError(seatResp)) {
				/* Fatal error, abort */
				printf("\n\033[0;31mFailed to add.\n\033[0m");
				free(reserv);
				return;
			}
		}
	} while(!done);

	printDAV(reserv->flightNo);

	free(reserv);
}



void removeReservation() {
	int reservNo, flightNo;

	if(!flightsLoaded(generalSocket)) {
		printf("\n\033[0;31mThere are no flights loaded yet.\n\033[0m");
		return;
	}

	printFlights();
	printf("\n\033[0;32mEnter your Flight Number\n\033[0m");
	flightNo = readValidFlight();

	printf("\n\033[0;32mList of reservations:\n\033[0m");
	if(printReservations(flightNo)) {
		return;
	}

	printf("\n\033[0;32mPlease enter the reservation number.\n\033[0m");
	reservNo = readValidReservation(flightNo);

	cancelReservation(generalSocket, reservNo);
	printf("\n\033[0;32mDone\n\033[0m");

}


void manageReservations() {
	int flightNo;

	if(!flightsLoaded(generalSocket)) {
		printf("\n\033[0;31mThere are no flights loaded yet.\n\033[0m");
		return;
	}

	printFlights();
	printf("\n\033[0;32mEnter your flight number\033[0m\n");
	flightNo = readValidFlight();
	printAllReservations(flightNo);
}


/* Print a list of all flights */
void printFlights() {
	ListPtr list;
	ListIteratorPtr iter;
	FlightObj * flight;

	list = getFlights(generalSocket);
	if(list == NULL) {
		printf("\033[0;31mThere are no flights loaded.\n\033[0m");
		return;
	}

	iter = listIterator(list);
	if(!iteratorHasNext(iter)) {
		printf("\033[0;31mThere are no flights loaded.\n\033[0m");
		return;
	}
	flight = malloc(sizeof(FlightObj));

	printf("\n\033[0;32mList of available flights:\n\033[0m");

	printf("\n\033[01;33mFlight No. | Departure -> Arrival | Date\n\033[0m");
	while(iteratorHasNext(iter)) {
		iteratorGetNext(iter, flight);
		printf("\033[0m- %d | %s ->  %s | %s \n\033[0m",flight->flightNo, flight->departure, flight->arrival, flight->date);
	}

	free(flight);
	freeIterator(iter);
	freeList(list);
}


void printFlightInfo(int flightNo) {
	FlightObj * flight = getFlightObj(generalSocket, flightNo);

	if(flight == NULL) {
		printf("\033[0;31mFlight %d doesn't exist.\n\033[0m", flightNo);
		return;
	}
	printf("\n\033[01;33mFlight - %d | %s ->  %s | %s \n",flight->flightNo, flight->departure, flight->arrival, flight->date);
	free(flight);
}



void printDAV(int flightNo) {
	ListPtr seatsList;
	ListIteratorPtr iter;
	FlightSeatObj * seat;
	FlightObj * flightData;
	int i, seatAdded = 1;

	flightData = getFlightObj(generalSocket, flightNo);
	if(flightData == NULL) {
		return;
	}

	seatsList = getSeats(generalSocket, flightNo);
	if(seatsList == NULL) {
		return;
	}
	iter = listIterator(seatsList);
	seat = malloc(sizeof(FlightSeatObj));

	printf("\n\033[01;33m--- DAV for flight %d | %d seats ---\n\033[0m", flightNo, flightData->seats);

	for(i = 0; i < flightData->seats; i++) {
		if(seatAdded && iteratorHasNext(iter)) {
			iteratorGetNext(iter, seat);
			seatAdded = 0;
		}

		if((i % (2*ROWQUANTITY)) == 0) {
			printf("\n");
		} else if(i % ROWQUANTITY == 0) {
			printf("   ");
		}

		if(!seatAdded && seat->seat == i+1) {
			printf("\033[0;31m [%d%s]\033[0m", i+1, (i+1 < 10) ? " " : "");
			seatAdded = 1;
		} else {
			printf("\033[0;32m [%d%s]\033[0m", i+1, (i+1 < 10) ? " " : "");
		}
	}	
	printf("\n");

	free(seat);
	free(flightData);
	freeIterator(iter);
	freeList(seatsList);
}




int printSeatError(int seatResp) {
	switch(seatResp) {
		case SEAT_NOT_FOUND:
			printf("\n\033[0;31mInvalid seat number, please try again.\n\033[0m");
			break;
		case SEAT_TAKEN:
			printf("\n\033[0;31mIt seems that the seat is already taken, please try again.\n\033[0m");
			break;
		case FULL_FLIGHT:
			printf("\n\033[0;31mSorry, the flight is already full.\n\033[0m");
			return 1;
		case FLIGHT_NOT_FOUND:
			printf("\n\033[0;31mSorry, the flight is not available any more.\n\033[0m");
			return 1;
		default:
			printf("\n\033[0;31mUnknown error.\n\033[0m");
			return 1;
	}
	return 0;
}


int printReservations(int flightNo) {
	ListPtr list;
	ListIteratorPtr iter;
	ReservationObj * reserv;

	list = getReservations(generalSocket, flightNo);
	if(list == NULL) {
		printf("\033[0;31mThere are no reservations for flight %d\n\033[0m", flightNo);
		return 1;
	}

	iter = listIterator(list);

	if(!iteratorHasNext(iter)) {
		printf("\033[0;31mThere are no reservations for flight %d\n\033[0m", flightNo);
		return 1;
	}

	reserv = malloc(sizeof(ReservationObj));

	printf("\n\033[01;33mReservation No. | Name | State | Seat\n\033[0m");
	while(iteratorHasNext(iter)) {
		iteratorGetNext(iter, reserv);
		printf("\033[0m- %d | %s |  %s | %d \n",reserv->reservationNo, reserv->name, reserv->state, reserv->seat);
	}

	free(reserv);
	freeIterator(iter);
	freeList(list);
	return 0;
}


void printAllReservations(int flightNo) {
	ListPtr list;
	ListIteratorPtr iter;
	ReservationObj * reserv;

	list = getCanReservations(generalSocket, flightNo);
	if(list == NULL) {
		printf("\n\033[0;31mThere are no reservations for flight %d\n\033[0m", flightNo);
		return;
	}

	iter = listIterator(list);

	if(!iteratorHasNext(iter)) {
		printf("\n\033[0;31mThere are no reservations for flight %d\n\033[0m", flightNo);
		freeList(list);
		return;
	}

	reserv = malloc(sizeof(ReservationObj));

	printf("\n\033[01;33mReservation No. | Name | State | Seat\n\033[0m");
	while(iteratorHasNext(iter)) {
		iteratorGetNext(iter, reserv);
		if(strcmp(reserv->state, "Active") == 0) {
			printf("\033[0;32m- %d | %s | %s | %d \n\033[0m",reserv->reservationNo, reserv->name, reserv->state, reserv->seat);
		} else {
			printf("\033[0;31m- %d | %s | %s | %d \n\033[0m",reserv->reservationNo, reserv->name, reserv->state, reserv->seat);
		}
	}

	free(reserv);
	freeIterator(iter);
	freeList(list);
}

int readValidFlight() {
	int input = readInt();
	while(!checkFlight(generalSocket, input)) {
		printf("\n\033[0;31mInvalid flight number, please try again.\n\033[0m");
		input = readInt();
	}
	return input;
}

int readValidReservation(int flightNo) {
	int reservNo = readInt();
	while(!checkReservation(generalSocket, flightNo, reservNo)) {
		printf("\n\033[0;31mInvalid reservation number, please try again.\n\033[0m");
		reservNo = readInt();
	}
	return reservNo;
}