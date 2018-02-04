#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "includes/dbSincronization.h"

pthread_mutex_t mutex;

int sincroInit() {
	if(pthread_mutex_init(&mutex, NULL) != 0) {
		/* Mutex init failed */
		return 0;
	}
	return 1;
}

void sincroDestroy() {
	pthread_mutex_destroy(&mutex);
}

void startCritical() {
	pthread_mutex_lock(&mutex);
}

void endCritical() {
	pthread_mutex_unlock(&mutex);
}