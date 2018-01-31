#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/gui.h"
#include "includes/socket.h"
#include "includes/clientRequestBuilder.h"
#include "includes/stringUtils.h"
#define STRINGMAXLENGTH 30
#define ROWQUANTITY 3
//GUI

static Socket_t  generalSocket;
static int RESERVATIONID = 0;
void app(Socket_t  socket){

	generalSocket = socket;
	flow();


}
void flow(){
	printf("\033[01;33mWelcome to the reservations system!\n");
	printf("\033[1;35m (1) Flight Status\n (2) Make a reservation\n (3) Remove a reservation\n (4) Manage Flights\n (5) Exit\n\033[0m");
	char selected = getchar(); 
	switch(selected){
		case '1': flightStatus();
			break;
		case '2': makeReservation();
			break;
		case '3': removeReservation();
			break;
		case '4': manageflights();
			break;
		case '5': exit(0); 
			break;	
	}	
}

void printDAV(int flightNo){
	ListPtr seatsList = getSeats(generalSocket, flightNo);
	ListIteratorPtr iter = listIterator(seatsList);
	FlightObj * flightData = getFlightObj(flightNo);
	int quantity = flightData->seats, current = 0;
	int planeSeats[quantity];
	printf("- DAV for flight number %d | %d seats -\n", flightNo, flightData->seats);
	while(iteratorHasNext(iter)){
		FlightSeatObj * seat = malloc(sizeof(FlightSeatObj));
		iteratorGetNext(iter, seat);
		planeSeats[seat->seat-1] = 1;
	}
	while(current <= quantity){
		for (int i = 0; i < ROWQUANTITY; ++i)
		{
			if(planeSeats[current] == 1)
				printf("\033[0;31m [%d%s]",current, (current<10)?" ":"");
			else
				printf("\033[0;32m [%d%s]",current, (current<10)?" ":"");	
			current++;
		}
		printf("   ");
		for (int i = 0; i < ROWQUANTITY; ++i)
		{
			if(planeSeats[current] == 1)
				printf("\033[0;31m [%d%s]",current, (current<10)?" ":"");
			else
				printf("\033[0;32m [%d%s]",current, (current<10)?" ":"");	
			current++;
		}
		printf(" \n");
	}
}
ListIteratorPtr getFlightsIterator(){
	ListPtr flightsList = getFlights(generalSocket);
	if(flightsList == NULL) {
		printf("\033[0;31mFailed to get flights\n");
	}
	ListIteratorPtr flightIterator = listIterator(flightsList);
	if(!iteratorHasNext(flightIterator)) {
		printf("Failed.\n");
		return NULL;
	}
	return flightIterator;
}
char * getFlightInfo(int flightNo){
	FlightObj * flight = malloc(sizeof(FlightObj));
	ListIteratorPtr flightIterator = getFlightsIterator();
	while(iteratorHasNext(flightIterator) && flight->flightNo != flightNo) {
			iteratorGetNext(flightIterator, flight);
	}
	freeIterator(flightIterator);
	char * ret = malloc(STRINGMAXLENGTH*sizeof(char));
	if(flight->flightNo == flightNo){
		sprintf(ret, "- %d | %s ->  %s | %s | $ %d | %d seats \n",flight->flightNo, flight->departure, flight->arrival, flight->date,  flight->price,  flight->seats);
		return ret;
	}
	else{
		sprintf(ret, "\033[0;31mFlight doesn't exist.\n");
		return ret;	
	}
}
FlightObj * getFlightObj(int flightNo){
	FlightObj * flight = malloc(sizeof(FlightObj));
	ListIteratorPtr flightIterator = getFlightsIterator();
	while(iteratorHasNext(flightIterator) && flight->flightNo != flightNo) {
			iteratorGetNext(flightIterator, flight);
	}
	freeIterator(flightIterator);

	if(flight->flightNo == flightNo)
		return flight;
	return NULL;		
}
void listFlights(){
	ListIteratorPtr iter = getFlightsIterator();
	FlightObj * flight = malloc(sizeof(FlightObj));
	while(iteratorHasNext(iter)){
		iteratorGetNext(iter, flight);
		printf("- %d | %s ->  %s | %s | $ %d | %d seats \n",flight->flightNo, flight->departure, flight->arrival, flight->date,  flight->price,  flight->seats);
	}
	freeIterator(iter);
}
void flightStatus(){
	ListIteratorPtr flightIterator = getFlightsIterator();
	FlightObj * flight = malloc(sizeof(FlightObj));
	char * input = malloc(5*sizeof(char));
	
	printf("\033[0;32mPlease enter the flight number or 'help' for getting the flights list.\n\033[0m");
	scanf("%s", input);
	
	//Help section	
	if(strcmp(input, "help") == 0){
		while(iteratorHasNext(flightIterator)) {
			iteratorGetNext(flightIterator, flight);
			printf("\033[0m- %d | %s ->  %s | %s \n",flight->flightNo, flight->departure, flight->arrival, flight->date);
		}

		printf("\033[0;32mNow, please enter the flight number.\n\033[0m");
		scanf("%s", input);

	}
	int flightNo = strToInt(input);
	printf("%s\n", getFlightInfo(flightNo));
	printDAV(flightNo);	
	free(flight);
	
	//IMPRIMIR PLANO DEL AVION CON LOS ASIENTOS LIBRES Y OCUPADOS

	//flow();
}

void manageflights(){
	printf("\033[0;32m (1) Add\n (2) Remove\n\033[0m");
	int selected;
	scanf("%d", &selected);
	printf("%d\n", selected);
	FlightObj * object = malloc(sizeof(FlightObj));
	int flightNumberAsked;
	char * dep = malloc(STRINGMAXLENGTH*sizeof(char)), * arr= malloc(STRINGMAXLENGTH*sizeof(char)), * date = malloc(STRINGMAXLENGTH*sizeof(char));
	switch(selected){
		case 1: 
				printf("\033[0;32mInsert new flight number\n\033[0m");
				scanf("%d", &object->flightNo);
				printf("\033[0;32mInsert new departure\n\033[0m");
				scanf("%s", dep);
				object->departure = dep;
				printf("\033[0;32mInsert new arrival\n\033[0m");
				scanf("%s", arr);
				object->arrival = arr;
				printf("\033[0;32mInsert new price\n\033[0m");
				scanf("%d", &object->price);
				printf("\033[0;32mInsert new seats\n\033[0m");
				scanf("%d", &object->seats);
				printf("\033[0;32mInsert new date\n\033[0m");
				scanf("%s", date);
				object->date = date;
				if(addFlight(generalSocket, object) ){
					printf("\033[0;32mFlight %d added\n", object->flightNo);
				} else {
					printf("\033[0;31mFail to add.\n");
				}
				break;
		case 2: 
				printf("\033[0;32mPlease enter the flight number.\n\033[0m");
				scanf("%d", &flightNumberAsked);
				printf("%s\n", getFlightInfo(flightNumberAsked));
				if(getFlightObj(flightNumberAsked) == NULL) return;
				deleteFlight(generalSocket, flightNumberAsked);
				break;
	}
	printf("\033[0;32mDone.\n");
	//flow();
}


void makeReservation(){
	int flightNumberSelected, seat;
	char * name = malloc(STRINGMAXLENGTH*sizeof(char)), * state = malloc(STRINGMAXLENGTH*sizeof(char));
	ReservationObj * reservation;

	printf("\033[0;32mPlease select a flight.\n\033[0m");
	listFlights();
	
	scanf("%d", &flightNumberSelected);
	printf("\033[0;32mEnter name.\n\033[0m");
	scanf("%s",name);
	printf("\033[0;32mEnter state.\n\033[0m");
	scanf("%s",state);
	printf("\033[0;32mEnter seat.\n\033[0m");
	scanf("%d", &seat);

	reservation = malloc(sizeof(ReservationObj));

	reservation->reservationNo = RESERVATIONID++;
	reservation->name = name;
	reservation->flightNo = flightNumberSelected;
	reservation->state = state;
	reservation->seat = seat;
	bookFlight(generalSocket, reservation);
	printf("\033[0;32mDone.\n");
}

void removeReservation(){
	int selected, reservationNumberAsked, flightNumberAsked;
	printf("\033[0;32mDo you know your reservation number?\n- (1) Yes\n- (2) No\n");
	scanf("%d", &selected);		
	switch(selected){
		case 1:
				printf("\033[0;32mPlease enter the reservation number.\n\033[0m");
				scanf("%d", &reservationNumberAsked);
				break;
		case 2: 
				printf("\033[0;32mEnter your Flight Number\n\033[0m");
				scanf("%d", &flightNumberAsked);
				ListPtr reservations = getReservations(generalSocket, flightNumberAsked);
				ListIteratorPtr iter = listIterator(reservations);
				ReservationObj * reservation = malloc(sizeof(ReservationObj));
				if(!iteratorHasNext(iter)){
					printf("\033[0;31mThere aren't reservations for this flight.\n");
					return;
				}
				while(iteratorHasNext(iter)){
					iteratorGetNext(iter, reservation);
					printf("- %d | %s | %s | %d \n", reservation->reservationNo, reservation->name, reservation->state, reservation->seat);
				}
				printDAV(flightNumberAsked);
				printf("\033[0;32mNow, enter your reservation number\n\033[0m");
				scanf("%d", &reservationNumberAsked);
				break;
		default:
				printf("\033[0;31mWrong input.\n");
				exit(0);
				break;
	}
	cancelReservation(generalSocket, reservationNumberAsked);
	printf("\033[0;32mDone.\n");
	return;
}

