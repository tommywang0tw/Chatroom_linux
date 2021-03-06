#include "Chatroom.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  struct sockaddr_in server;
  int sock;
  char myUsername[USERNAMESIZE];
  message msg;
  int n;
  fd_set file_descriptors;
  /* Create a socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  /* Use arguments to specify server IP and port */
  if(argc != 3)
  {
    fprintf(stderr, "Please specify server IP and port: %s <IP> <port>\n", argv[0]);
    exit(1);
  }
  server.sin_family = AF_INET;
  /* argv[2] is the port */ 
  server.sin_port = htons(atoi(argv[2]));
  /* argv[1] is the IP address of the server */
  inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr);


  printf("Please enter your username\n (1 ~ %d characters): ", USERNAMESIZE);
  for (;;) {
    memset(myUsername, 0, sizeof(myUsername));
    fgets(myUsername, sizeof(myUsername), stdin);
    if (myUsername[0] == '\n') {
      printf("Enter at least one character!\n");
      printf("Please enter again: ");
      continue;
    }
    break;
  }
  strtok(myUsername, "\n");
  memset(&msg, 0, sizeof(message));
  strcpy(msg.username, myUsername);
  msg.type = SET_USERNAME;
  
  /* connect to the server */
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
    perror("connect failed: ");
    exit(1);
  }
  printf("Connect successfully!\n");
  /* Send username to server */
  if (write(sock, &msg, sizeof(message)) == -1)
    perror("write failed");
  printf("Type \"/quit\" for exit\n");
  printf("----------Start Talking to Your Friends!----------\n");
  for (;;) {
    /* Initialize file descriptor set*/
    FD_ZERO(&file_descriptors);
    FD_SET(STDIN_FILENO, &file_descriptors);
    FD_SET(sock, &file_descriptors);
    fflush(stdin);
    if (select(sock + 1, &file_descriptors, NULL, NULL, NULL) < 0) {
      perror("select:");
      exit(1);
    }
    if (FD_ISSET(STDIN_FILENO, &file_descriptors)) {
      /* handle user input */
      memset(&msg, 0, sizeof(message));
      fgets(msg.msg, sizeof(msg.msg), stdin);
      // "/quit" cmd to disconnect from server
      if(strcmp(msg.msg, "/quit\n") == 0)
      {
        printf("Goodbye! %s\n", myUsername);
        break;
      }
      // Normal messgae
      strcpy(msg.username, myUsername);
      msg.type = MESSAGE;
      printf("\t%s(you): %s", msg.username, msg.msg);
      if (write(sock, &msg, sizeof(message)) == -1)
        perror("write failed");
    }
    if (FD_ISSET(sock, &file_descriptors)) {
      // handle server input
      memset(&msg, 0, sizeof(message));
      if ((n = read(sock, &msg, sizeof(message))) <= 0) {
        if (n == 0) {
          printf("Server disconnected\n");
          return 0;
        } else {
          perror("read failed");
        }
      } else {
        if (msg.type == DISCONNECT) {
          printf("\t[Info]%s has left the chatroom!\n", msg.username);
        } else if (msg.type == SET_USERNAME) {
          printf("\t[Info]%s has joined the chatroom!\n", msg.username);
        } else {
          printf("\t%s: %s", msg.username, msg.msg);
        }
      }
    }
  }
  close(sock);
  return 0;
}
