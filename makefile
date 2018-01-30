makefile:

clean:
	-rm client installDb server
clean_client:
	-rm client

all:
	gcc -o installDb installDb.c list.c db.c sqlite/sqlite3.c -lpthread -ldl
	gcc -o server server.c socket.c db.c list.c stringUtils.c serverRequestHandler.c sqlite/sqlite3.c -lpthread -ldl
	gcc -o client client.c socket.c stringUtils.c list.c clientRequestBuilder.c gui.c
	./installDb

client:
	gcc -o client client.c socket.c stringUtils.c list.c clientRequestBuilder.c gui.c
