CC = gcc
all: client server
client: OS_client.c Chatroom.h
	$(CC) OS_client.c -o client

server: OS_server.c Chatroom.h
	$(CC) OS_server.c -o server

clean:
	rm -rf client server
