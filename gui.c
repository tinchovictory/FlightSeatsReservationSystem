#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/gui.h"
#include "includes/clientRequestBuilder.h"
#include "includes/stringUtils.h"
#include "includes/list.h"
#include "includes/dbObj.h"

#define STRINGMAXLENGTH 50
#define ROWQUANTITY 3
#define DATE_LENGHT 11

typedef enum {SEAT_OK, SEAT_TAKEN, FULL_FLIGHT, SEAT_NOT_FOUND, FLIGHT_NOT_FOUND} CheckSeatResp;

void flow();
void printMenu();
void flightStatus();
void manageflights();
void makeReservation();
void manageReservations();
void removeReservation();
int flightsLoaded();
void printFlights();
void printFlightInfo(int flightNo);
FlightObj * getFlightObj(int flightNo);
void printDAV(int flightNo);
char * readString(char * buff, int buffSize);
int readInt();
char * readAndCpyStr();
char * readDate();
int readIntHelp();
int isValidDate(int day, int month, int year);
void clearBuff();
int checkFlight(int flightNo);
CheckSeatResp checkSeat(int flightNo, int seat);
int printSeatError(int seatResp);
int printReservations(int flightNo);
void printAllReservations(int flightNo);
int checkReservation(int flightNo, int reservNo);

static Socket_t  generalSocket;

void app(Socket_t  socket) {
	generalSocket = socket;
	flow();
}


void flow() {
	int selected;

	printf("\033[01;33m*** Welcome to the reservations system! ***\n");

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
				printf("\n\033[0;31mInvalid option, please try again.\n");
				break;
		}

		printMenu();
	}
	printf("\n\033[01;33m*** Goodbye! ***\n\n");
}

void printMenu() {
	printf("\n\033[01;33mPlease select an option:\n");
	printf("\033[1;35m (1) Flight Status\n (2) Make a reservation\n (3) Remove a reservation\n (4) Reservations\n (5) Manage Flights\n (6) Exit\n\033[0m");	
}

void flightStatus() {
	int input;

	if(!flightsLoaded()) {
		printf("\n\033[0;31mThere are no flights loaded yet.\n");
		return;
	}
	
	printf("\n\033[0;32mPlease enter the flight number or 'help' for getting the flights list.\n\033[0m");
	input = readIntHelp();
	
	/* Help section */
	if(input == -1){

		printFlights();

		printf("\n\n\033[0;32mPlease enter the flight number.\n\033[0m");
		input = readInt();
	}

	printFlightInfo(input);

	printDAV(input);
}

/* Handle add and remove flights */
void manageflights() {
	char c;
	int selectedMenu, flightNo;
	FlightObj * flight;
	char buff[STRINGMAXLENGTH] = {0};

	do {
		printf("\n\033[0;32m (1) Add\n (2) Remove\n\033[0m");	
	} while((selectedMenu = readInt()) != 1 && selectedMenu != 2);
	
	if(selectedMenu == 1) {
		/* Add flight */
		flight = malloc(sizeof(FlightObj));

		printf("\n\033[0;32mInsert flight number\n\033[0m");
		flight->flightNo = readInt();
		while(checkFlight(flight->flightNo)) {
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
			printf("\n\033[0;32mFlight %d added\n", flight->flightNo);
		} else {
			printf("\n\033[0;31mFail to add.\n");
		}

		free(flight);

	} else {
		/* Remove flight */
		printf("\n\033[0;32mPlease enter the flight number.\n\033[0m");
		flightNo = readInt();
		printFlightInfo(flightNo);
		deleteFlight(generalSocket, flightNo);
		printf("\n\033[0;32mDone\n");
	}
}


void makeReservation() {
	ReservationObj * reserv;
	int seatResp, done = 0;

	if(!flightsLoaded()) {
		printf("\n\033[0;31mThere are no flights loaded yet.\n");
		return;
	}

	reserv = malloc(sizeof(ReservationObj));

	printf("\n\033[0;32mList of available flights:\n\033[0m");
	printFlights();
	printf("\n\033[0;32mPlease select a flight.\n\033[0m");
	reserv->flightNo = readInt();
	/* Validate FlightNo */
	while(!checkFlight(reserv->flightNo)) {
		printf("\n\033[0;31mInvalid flight number, please try again.\n\033[0m");
		reserv->flightNo = readInt();
	}

	printf("\n\033[0;32mEnter name.\n\033[0m");
	reserv->name = readAndCpyStr();

	/* Most common error is taken seat, prevent it */
	do {
		printDAV(reserv->flightNo);
		printf("\n\033[0;32mEnter seat.\n\033[0m");
		reserv->seat = readInt();
		/* Validate seat */
		while((seatResp = checkSeat(reserv->flightNo, reserv->seat)) != SEAT_OK) {
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
			seatResp = checkSeat(reserv->flightNo, reserv->seat);
			if(printSeatError(seatResp)) {
				/* Fatal error, abort */
				printf("\n\033[0;31mFailed to add.\n");
				free(reserv);
				return;
			}
		}
	} while(!done);

	printDAV(reserv->flightNo);

	free(reserv);
}



void removeReservation() {
	int selectedMenu, reservNo, flightNo;
	

	do {
		printf("\n\033[0;32mDo you know your reservation number?\n- (1) Yes\n- (2) No\n- (3) Cancel\n");	
	} while((selectedMenu = readInt()) != 1 && selectedMenu != 2 && selectedMenu != 3);

	if(selectedMenu == 3) {
		return;
	}

	if(selectedMenu == 2) {
		printf("\033[0;32mEnter your Flight Number\n\033[0m");
		flightNo = readInt();
		/* Validate FlightNo */
		while(!checkFlight(flightNo)) {
			printf("\n\033[0;31mInvalid flight number, please try again.\n\033[0m");
			flightNo = readInt();
		}
		if(printReservations(flightNo)) {
			return;
		}
	}

	printf("\033[0;32mPlease enter the reservation number.\n\033[0m");
	reservNo = readInt();
	while(!checkReservation(flightNo, reservNo)) {
		printf("\n\033[0;31mInvalid reservation number, please try again.\n\033[0m");
		reservNo = readInt();
	}

	cancelReservation(generalSocket, reservNo);
	printf("\n\033[0;32mDone\n");

}


void manageReservations() {
	int flightNo;
	printf("\n\033[0;32mEnter your flight number\033[0m\n");
	flightNo = readInt();
	/* Validate FlightNo */
	while(!checkFlight(flightNo)) {
		printf("\n\033[0;31mInvalid flight number, please try again.\n\033[0m");
		flightNo = readInt();
	}
	printAllReservations(flightNo);
}

/* Check if there are any flights loaded in the db */
int flightsLoaded() {
	ListPtr list = getFlights(generalSocket);
	ListIteratorPtr iter;

	if(list == NULL) {
		return 0;
	}

	iter = listIterator(list);
	if(!iteratorHasNext(iter)) {
		freeList(list);
		return 0;
	}
	freeIterator(iter);
	freeList(list);
	return 1;
}


/* Print a list of all flights */
void printFlights() {
	ListPtr list;
	ListIteratorPtr iter;
	FlightObj * flight;

	list = getFlights(generalSocket);
	if(list == NULL) {
		printf("There are no flights loaded.\n");
		return;
	}

	flight = malloc(sizeof(FlightObj));
	iter = listIterator(list);

	printf("Flight No. | Departure -> Arrival | Date\n");
	while(iteratorHasNext(iter)) {
		iteratorGetNext(iter, flight);
		printf("\033[0m- %d | %s ->  %s | %s \n",flight->flightNo, flight->departure, flight->arrival, flight->date);
	}

	free(flight);
	freeIterator(iter);
	freeList(list);
}


void printFlightInfo(int flightNo) {
	FlightObj * flight = getFlightObj(flightNo);

	if(flight == NULL) {
		printf("\033[0;31mFlight %d doesn't exist.\n", flightNo);
		return;
	}
	printf("\033[0m- %d | %s ->  %s | %s \n",flight->flightNo, flight->departure, flight->arrival, flight->date);
	free(flight);
}

FlightObj * getFlightObj(int flightNo) {
	ListPtr list;
	ListIteratorPtr iter;
	FlightObj * flight;
	int done = 0;

	list = getFlights(generalSocket);
	if(list == NULL) {
		return NULL;
	}

	iter = listIterator(list);
	flight = malloc(sizeof(FlightObj));

	while(!done && iteratorHasNext(iter)) {
		iteratorGetNext(iter, flight);
		if(flight->flightNo == flightNo) {
			done = 1;
		}
	}

	freeIterator(iter);
	freeList(list);

	if(!done) {
		free(flight);
		return NULL;
	}
	return flight;
}

int checkFlight(int flightNo) {
	FlightObj * flight = getFlightObj(flightNo);
	if(flight == NULL) {
		return 0;
	}
	free(flight);
	return 1;
}

CheckSeatResp checkSeat(int flightNo, int seat) {
	ListPtr seatsList;
	ListIteratorPtr iter;
	FlightSeatObj * seatObj;
	FlightObj * flightData;
	int done = 0, count = 0;

	if(seat <= 0) {
		return SEAT_NOT_FOUND;
	}

	flightData = getFlightObj(flightNo);
	if(flightData == NULL) {
		return FLIGHT_NOT_FOUND;
	}

	if(flightData->seats < seat) {
		return SEAT_NOT_FOUND;
	}

	seatsList = getSeats(generalSocket, flightNo);
	if(seatsList == NULL) {
		return FLIGHT_NOT_FOUND;
	}
	iter = listIterator(seatsList);
	seatObj = malloc(sizeof(FlightSeatObj));

	while(iteratorHasNext(iter)) {
		iteratorGetNext(iter, seatObj);
		if(seatObj->seat == seat) {
			done = 1;
		}
		count++;
	}

	free(seatObj);
	freeIterator(iter);
	freeList(seatsList);

	if(count == flightData->seats) {
		free(flightData);
		return FULL_FLIGHT;
	}
	free(flightData);
	if(done) {
		return SEAT_TAKEN;
	}
	return SEAT_OK;
}

void printDAV(int flightNo) {
	ListPtr seatsList;
	ListIteratorPtr iter;
	FlightSeatObj * seat;
	FlightObj * flightData;
	int i, seatAdded = 1;

	flightData = getFlightObj(flightNo);
	if(flightData == NULL) {
		return;
	}

	seatsList = getSeats(generalSocket, flightNo);
	if(seatsList == NULL) {
		return;
	}
	iter = listIterator(seatsList);
	seat = malloc(sizeof(FlightSeatObj));

	printf("\n- DAV for flight %d | %d seats -\n", flightNo, flightData->seats);

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
			printf("\033[0;31m [%d%s]", i+1, (i+1 < 10) ? " " : "");
			seatAdded = 1;
		} else {
			printf("\033[0;32m [%d%s]", i+1, (i+1 < 10) ? " " : "");
		}
	}	
	printf("\n");

	free(seat);
	free(flightData);
	freeIterator(iter);
	freeList(seatsList);
}


char * readString(char * buff, int buffSize) {
	int i = 0;
	char c;

	memset(buff, 0, buffSize);
	while(i < buffSize && (c = getchar()) != '\n') {
		buff[i] = c;
		i++;
	}

	if(i == buffSize) {
		clearBuff();
	}
	
	return buff;
}

int readInt() {
	char buff[STRINGMAXLENGTH] = {0};
	int done = 0, resp = 0;

	while(!done) {
		readString(buff, STRINGMAXLENGTH - 1);
		done = sscanf(buff, "%d", &resp);
		if(!done) {
			printf("%s is not a number... Try again!\n", buff);
		}
	}

	return resp;
}

char * readAndCpyStr() {
	char buff[STRINGMAXLENGTH] = {0};
	char * resp;

	readString(buff, STRINGMAXLENGTH -1);
	resp = calloc(strlen(buff) + 1, sizeof(char));
	strcpy(resp, buff);

	return resp;
}

char * readDate() {
	char buff[STRINGMAXLENGTH] = {0};
	char * resp;
	int done = 0, day, month, year;

	while(!done) {
		readString(buff, STRINGMAXLENGTH - 1);
		done = sscanf(buff, "%d/%d/%d", &day, &month, &year);

		if(done) {
			done = isValidDate(day, month, year);
		}

		if(!done ) {
			printf("%s is not a valid date... Try again!\n", buff);
		}
	}

	resp = calloc(DATE_LENGHT, sizeof(char));
	sprintf(resp, "%s%d/%s%d/%d", day < 10 ? "0" : "", day, month < 10 ? "0" : "", month, year);
	return resp;
}

int readIntHelp() {
	char buff[STRINGMAXLENGTH] = {0};
	int done = 0, resp = 0;

	while(!done) {
		readString(buff, STRINGMAXLENGTH - 1);
		if(strcmp(buff, "help") == 0) {
			done = 1;
			resp = -1;
		} else {
			done = sscanf(buff, "%d", &resp);	
		}
		if(!done) {
			printf("%s is not a number... Try again or press Help!\n", buff);
		}
	}
	return resp;
}

int isValidDate(int day, int month, int year) {
	if(year < 2018 || year > 2030) {
		return 0;
	}
	if(month < 1 || month > 12) {
		return 0;
	}
	if(day < 1 || day > 31) {
		return 0;
	}
	if(month == 2 && day > 29) {
		return 0;
	}
	if(month == 2 && day > 28 && !(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) {
		return 0;
	}
	if(day > 30 && (month == 4 || month == 6 || month == 9 || month == 11)) {
		return 0;
	}
	return 1;
}


void clearBuff() {
	char c;
	while((c = getchar()) != '\n' && c != EOF);
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
	reserv = malloc(sizeof(ReservationObj));

	printf("Reservation No. | Name | State | Seat\n");
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

	printf("Reservation No. | Name | State | Seat\n");
	while(iteratorHasNext(iter)) {
		iteratorGetNext(iter, reserv);
		if(strcmp(reserv->state, "Active") == 0) {
			printf("\033[0;32m- %d | %s | %s | %d \n",reserv->reservationNo, reserv->name, reserv->state, reserv->seat);
		} else {
			printf("\033[0;31m- %d | %s | %s | %d \n\033[0m",reserv->reservationNo, reserv->name, reserv->state, reserv->seat);
		}
	}

	free(reserv);
	freeIterator(iter);
	freeList(list);
}


int checkReservation(int flightNo, int reservNo) {
	ListPtr list;
	ListIteratorPtr iter;
	ReservationObj * reserv;
	int done = 0;

	list = getReservations(generalSocket, flightNo);
	if(list == NULL) {
		return 0;
	}

	iter = listIterator(list);
	reserv = malloc(sizeof(ReservationObj));

	while(!done && iteratorHasNext(iter)) {
		iteratorGetNext(iter, reserv);
		if(reserv->reservationNo == reservNo) {
			done = 1;
		}
	}

	free(reserv);
	freeIterator(iter);
	freeList(list);
	
	if(!done) {
		return 0;		
	}
	return 1;
}
