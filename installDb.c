#include <stdio.h>
#include <stdlib.h>
#include "includes/db.h"

void checkDbError(DbCode code);

int main(void) {
	Db_t db;
	DbCode dbCode;

	/* Create a db connection */
	dbCode = dbInit(&db);
	checkDbError(dbCode);

	/* Install Db */
	dbCode = installDb(db);
	checkDbError(dbCode);

	/* Close connection */
	dbClose(db);

	printf("\033[1;32m[Installed]\n");
	return 0;

}


 void checkDbError(DbCode code) {
	if(code == DB_OK) {
		return;
	}

	printf("Db fail\n");
	exit(1);
}
