#include <stdlib.h>
#include <stdio.h>
#include "includes/gui.h"
//GUI
void menu(){
	printf("Welcome to the reservations system!\n");
	printf(" (1) Flight Status\n (2) Make a reservation\n (3) Remove a reservation\n (4) Manage Flights\n (5) Exit\n");
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

void flightStatus(){

}

void makeReservation(){

}

void removeReservation(){

}

void manageflights(){

}