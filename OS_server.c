#include "Chatroom.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int listener;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  socklen_t len;
  client_info client[MAX_CLIENTS];
  int fd_max;
  int i, j, n;
  message msg;
  // Initialize client array
  for (i = 0; i < MAX_CLIENTS; i++) {
    client[i].socket = 0;
    client[i].username[0] = '\0';
  }
  fd_set master;
  fd_set read_fds;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  /* Create socket */
  listener = socket(AF_INET, SOCK_STREAM, 0);

  /* socket setting */
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(12345);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("bind failed");
    exit(1);
  }
  /* set listener to listen for connection */
  if (listen(listener, 5) == -1) {
    perror("listen failed");
    exit(1);
  }
  printf("----------Chatroom----------\n");
  FD_SET(listener, &master);
  fd_max = listener;
  for (;;) {
    read_fds = master;
    if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(1);
    }
    /* new connection is coming */
    if (FD_ISSET(listener, &read_fds)) {
      len = sizeof(client_addr);
      for (i = 0; i < MAX_CLIENTS; i++) {
        if (client[i].socket == 0) {
          client[i].socket =
              accept(listener, (struct sockaddr *)&client_addr, &len);
          if (client[i].socket == -1) {
            perror("accept failed");
          } else {
            FD_SET(client[i].socket, &master);
            if (client[i].socket > fd_max) {
              fd_max = client[i].socket;
            }
          }
          break;
        }
      }
      if (i == MAX_CLIENTS)
        fprintf(stderr, "Chatroom is too full to accept new client\n");
    }
    // handle message from clients
    for (i = 0; i < MAX_CLIENTS; i++) {
      if (client[i].socket > 0 && FD_ISSET(client[i].socket, &read_fds)) {
        memset(&msg, 0, sizeof(msg));
        // client is disconnected or read error
        if ((n = read(client[i].socket, &msg, sizeof(msg))) <= 0) {
          if (n == 0) {
            close(client[i].socket);
            FD_CLR(client[i].socket, &master);
            client[i].socket = 0;
            if (client[i].username[0] != '\0') {
              printf("\t%s has left the chatroom!\n", client[i].username);
              msg.type = DISCONNECT;
              strcpy(msg.username, client[i].username);
              // notify other clients that a client just disconnected
              
              for (j = 0; j < MAX_CLIENTS; j++) {
                if (FD_ISSET(client[j].socket, &master)) {
                  if (write(client[j].socket, &msg, sizeof(message)) == -1)
                    perror("write failed");
                }
              }
              client[i].username[0] = '\0';
            }
          } else {
            perror("read failed");
          }
        } 
        // set username or normal message
        else {
          // set username
          if (msg.type == SET_USERNAME) {
            strcpy(client[i].username, msg.username);
            printf("\t%s has joined the chatroom!\n", client[i].username);
            for (j = 0; j < MAX_CLIENTS; j++) {
              // notify other clients that a new client just joined in
              if (j != i && FD_ISSET(client[j].socket, &master)) {
                if (write(client[j].socket, &msg, sizeof(msg)) == -1)
                  perror("write failed");
              }
            }
          }
          // normal message
          else {
            //print the message on the server
            printf("\t%s: %s", msg.username, msg.msg);
            // broadcast the message to all the clients except the one sent the
            // message
            for (j = 0; j < MAX_CLIENTS; j++) {
              if (j != i && FD_ISSET(client[j].socket, &master)) {
                if (write(client[j].socket, &msg, sizeof(msg)) == -1)
                  perror("write failed");
              }
            }
          }
        }
      }
    }
  }
  close(listener);
  return 0;
}
