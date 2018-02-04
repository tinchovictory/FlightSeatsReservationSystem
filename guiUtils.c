#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/guiUtils.h"
#include "includes/stringUtils.h"
#include "includes/list.h"
#include "includes/clientRequestBuilder.h"


/* Check if there are any flights loaded in the db */
int flightsLoaded(Socket_t socket) {
	ListPtr list = getFlights(socket);
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

FlightObj * getFlightObj(Socket_t socket, int flightNo) {
	ListPtr list;
	ListIteratorPtr iter;
	FlightObj * flight;
	int done = 0;

	list = getFlights(socket);
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



int checkFlight(Socket_t socket, int flightNo) {
	FlightObj * flight = getFlightObj(socket, flightNo);
	if(flight == NULL) {
		return 0;
	}
	free(flight);
	return 1;
}

CheckSeatResp checkSeat(Socket_t socket, int flightNo, int seat) {
	ListPtr seatsList;
	ListIteratorPtr iter;
	FlightSeatObj * seatObj;
	FlightObj * flightData;
	int done = 0, count = 0;

	if(seat <= 0) {
		return SEAT_NOT_FOUND;
	}

	flightData = getFlightObj(socket, flightNo);
	if(flightData == NULL) {
		return FLIGHT_NOT_FOUND;
	}

	if(flightData->seats < seat) {
		return SEAT_NOT_FOUND;
	}

	seatsList = getSeats(socket, flightNo);
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
			printf("\033[0;31m%s is not a number... Try again!\n\033[0m", buff);
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
			printf("\033[0;31m%s is not a valid date... Try again!\n\033[0m", buff);
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
			printf("\033[0;31m%s is not a number... Try again or press Help!\n\033[0m", buff);
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


int checkReservation(Socket_t socket, int flightNo, int reservNo) {
	ListPtr list;
	ListIteratorPtr iter;
	ReservationObj * reserv;
	int done = 0;

	list = getReservations(socket, flightNo);
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
