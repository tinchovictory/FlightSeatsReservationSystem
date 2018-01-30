#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/gui.h"
#include "includes/socket.h"
#include "includes/clientRequestBuilder.h"
#include "includes/stringUtils.h"
#define STRINGMAXLENGTH 30

//GUI

static Socket_t  generalSocket;

void app(Socket_t  socket){

	generalSocket = socket;
	flow();


}
void flow(){
	printf("\033[01;33mWelcome to the reservations system!\n");
	printf("\033[0;33m (1) Flight Status\n (2) Make a reservation\n (3) Remove a reservation\n (4) Manage Flights\n (5) Exit\n");
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

void flightStatus(){
	ListIteratorPtr flightIterator = getFlightsIterator();
	FlightObj * flight = malloc(sizeof(FlightObj));
	char * input = malloc(5*sizeof(char));
	
	printf("\033[0;32mPlease enter the flight number or 'help' for getting the flights list.\n");
	scanf("%s", input);
	
	//Help section	
	if(strcmp(input, "help") == 0){
		while(iteratorHasNext(flightIterator)) {
			iteratorGetNext(flightIterator, flight);
			printf("- %d | %s ->  %s | %s \n",flight->flightNo, flight->departure, flight->arrival, flight->date);
		}

		printf("\033[0;32mNow, please enter the flight number.\n");
		scanf("%s", input);

	}
	int flightNo = strToInt(input);
	printf("%s\n", getFlightInfo(flightNo));	
	free(flight);
	
	//IMPRIMIR PLANO DEL AVION CON LOS ASIENTOS LIBRES Y OCUPADOS

	//flow();
}

void manageflights(){
	printf(" (1) Add\n (2) Remove\n");
	int selected;
	scanf("%d", &selected);
	printf("%d\n", selected);
	FlightObj * object = malloc(sizeof(FlightObj));
	int flightNumberAsked;
	char * dep = malloc(STRINGMAXLENGTH*sizeof(char)), * arr= malloc(STRINGMAXLENGTH*sizeof(char)), * date = malloc(STRINGMAXLENGTH*sizeof(char));
	switch(selected){
		case 1: 
				printf("Insert new flight number\n");
				scanf("%d", &object->flightNo);
				printf("Insert new departure\n");
				scanf("%s", dep);
				object->departure = dep;
				printf("Insert new arrival\n");
				scanf("%s", arr);
				object->arrival = arr;
				printf("Insert new price\n");
				scanf("%d", &object->price);
				printf("Insert new seats\n");
				scanf("%d", &object->seats);
				printf("Insert new date\n");
				scanf("%s", date);
				object->date = date;
				if(addFlight(generalSocket, object) ){
					printf("Flight %d added\n", object->flightNo);
				} else {
					printf("Fail to add.\n");
				}
				break;
		case 2: 
				printf("\033[0;32mPlease enter the flight number.\n");
				scanf("%d", &flightNumberAsked);
				printf("%s\n", getFlightInfo(flightNumberAsked));
				if(getFlightObj(flightNumberAsked) == NULL) return;
				deleteFlight(generalSocket, flightNumberAsked);
				break;
	}
	printf("Done.\n");
	//flow();
}


void makeReservation(){
	printf("\033[0;32m”Please select a flight.\n");
	//Print Flights
}

void removeReservation(){
	int reservationNumberAsked;
	printf("\033[0;32m”Please enter the reservation number.\n");
	//scanf("%d", reservationNumberAsked);
	//Remove Reservation

}

