#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "includes/serverUi.h"

#define STR_LENGTH 30
#define TRUE 1

void * serverUi(void * data);
void clearBuff();
void handleReq(char * buff);
void showUsers();
void showHelp();

int usersCount;
int totalUsers;

void uiInit() {
	pthread_t thread;
	if(pthread_create(&thread, NULL, serverUi, NULL) == 0) {
		pthread_detach(thread);
	}

	usersCount = 0;
	totalUsers = 0;
}

void * serverUi(void * data) {
	char c;
	char buff[STR_LENGTH + 1] = {0};
	int i;
	while(TRUE) {
		i = 0;
		while(i < STR_LENGTH && (c = getchar()) != '\n') {
			buff[i++] = c;
		}
		buff[i] = 0;
		if(i == STR_LENGTH) {
			clearBuff();
		}

		handleReq(buff);
	}

}

void handleReq(char * buff) {
	if(strcmp("quit", buff) == 0) {
		exit(0);
	}else if(strcmp("users", buff) == 0) {
		showUsers();
	}else if(strcmp("help", buff) == 0) {
		showHelp();
	} else {
		printf("\x1B[31mInvalid command...\033[0m\n");
	}
}

void showUsers() {
	printf("\n\033[01;33mShowing connected users...\033[0m\n");
	printf("\x1B[32mOnline users: %d\033[0m\n", usersCount);
	printf("\x1B[31mOffline users: %d\033[0m\n", totalUsers - usersCount);
	printf("\033[1;35mTotal users: %d\033[0m\n", totalUsers);
}

void showHelp() {
	printf("\n\033[01;33mAvailable options:\033[0m\n");
	printf("\033[1;35musers\033[0m\n");
	printf("\033[1;35mquit\033[0m\n");
}

void addUser() {
	usersCount++;
	totalUsers++;
}

void removeUser() {
	usersCount--;
}

void clearBuff() {
	char c;
	while((c = getchar()) != '\n' && c != EOF);
}