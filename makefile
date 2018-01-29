makefile:

clean:
	-rm client installDb server

all:
	gcc -o installDb installDb.c list.c db.c sqlite/sqlite3.c -lpthread -ldl
	gcc -o server server.c socket.c db.c sqlite/sqlite3.c list.c -lpthread -ldl
	gcc -o client client.c socket.c