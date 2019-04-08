Chatroom
========
Chatroom - a simple linux command C program for group chat.

Written by Tommy, Eric.

run make

Then run: ./server <port>

and for clients: ./client <IP> <port>


For example:

If we want to build up a server with opened port 3045, we use this cmd:

./server 3045

Then, if clients are connecting to the server of local host, use:

./client 127.0.0.1 3045

Note: 127.0.0.1 can be replaced by the IP address of the server if it's

not local host
