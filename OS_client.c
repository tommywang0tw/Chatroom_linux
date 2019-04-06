#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Chatroom.h"
int main()
{
    struct sockaddr_in server;
    int sock;
    char *msg = "Message back!\n";
    char buf[BUFSIZE];
    char username[USERNAMESIZE];
    char other_username[USERNAMESIZE];
    int n;
    fd_set file_descriptors;
    /* Create a socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    /* 127.0.0.1 is the IP address of local host */
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr.s_addr);
    
    /* connect to the server */
    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("connect failed: ");
        exit(1);
    }
    printf("Connect successfully!\n");
    printf("Please enter your username\n (1 ~ %d characters): ", USERNAMESIZE);
    for(;;)
    {
        memset(username, 0, sizeof(username));
        fgets(username, sizeof(username), stdin);
        if(username[0] == '\n')
        {
            printf("Enter at least one character!\n");
            printf("Please enter again: ");
            continue;
        }
        break;
    }
    strtok(username, "\n");
    /* Send username to server */
    if(write(sock, username, sizeof(username)) == -1)
        perror("write failed");
    printf("----------Start Talking to Your Friends!----------\n");
    for(;;)
    {
        /* Initialize file descriptor set*/
        FD_ZERO(&file_descriptors);
        FD_SET(STDIN_FILENO, &file_descriptors);
        FD_SET(sock, &file_descriptors);
        fflush(stdin);
        if(select(sock+1, &file_descriptors, NULL, NULL, NULL) < 0)
        {
            perror("select:");
            exit(1);
        }
        if(FD_ISSET(STDIN_FILENO, &file_descriptors))
        {
            //handle user input
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf), stdin);
            if(write(sock, username, sizeof(username)) == -1)
                perror("write failed");
            if(write(sock, buf, sizeof(buf)) == -1)
                perror("write failed");
            printf("\tyou: %s", buf);
        }
        if(FD_ISSET(sock, &file_descriptors))
        {
            //handle server input
            memset(buf, 0, sizeof(buf));
            if((n = read(sock, buf, sizeof(buf))) <= 0)
            {
                if(n == 0)
                {
                    printf("The server is disconnected\n");
                    return 0;
                }
                else
                {
                    perror("read failed: ");
                }
            }
            else
            {
                memset(other_username, 0, sizeof(other_username));
                if(read(sock, other_username, sizeof(other_username)) == -1)
                    perror("read failed");
                printf("\t%s: %s", other_username, buf);
            }
        }
    }
    close(sock);
    return 0;
}
