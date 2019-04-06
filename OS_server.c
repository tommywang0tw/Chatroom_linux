#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "Chatroom.h"
typedef struct client_info
{
    int socket;
    char username[20];
} client_info;
int main()
{
    int listener;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t len;
    client_info client[MAX_CLIENTS];
    int fd_max;
    int i, j, n;
    char buf[BUFSIZE];
    char username[USERNAMESIZE];
    //Initialize client array
    for(i=0;i<MAX_CLIENTS;i++)
    {
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
    if(bind(listener, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind failed");
        exit(1);
    }    
    /* set listener to listen for connection */
    if(listen(listener, 5) == -1)
    {  
        perror("listen failed");
        exit(1);
    }
    printf("----------Chatroom----------\n");    
    FD_SET(listener, &master);
    fd_max = listener;
    for(;;)
    {
        read_fds = master;
        if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(1);
        }
        /* new connection is coming */
        if(FD_ISSET(listener, &read_fds))
        {
            len = sizeof(client_addr);
            for(i=0;i<MAX_CLIENTS;i++)
            {
                if(client[i].socket == 0)
                {
                    client[i].socket = accept(listener, (struct sockaddr *)&client_addr, &len);
                    if(client[i].socket == -1)
                    {
                        perror("accept failed");
                    }
                    else
                    {
                        FD_SET(client[i].socket, &master);
                        if(client[i].socket > fd_max)
                        {
                            fd_max = client[i].socket;
                        }
                    }
                break;
                }
            }
            if(i == MAX_CLIENTS)
                fprintf(stderr,"Chatroom is too full to accept new client\n");
        }
        for(i=0;i<=MAX_CLIENTS;i++)
        {
            //handle clients message
            if(client[i].socket > 0 && FD_ISSET(client[i].socket, &read_fds) && client[i].username[0] == '\0')
            {
                memset(username, 0, sizeof(username));
                if((n = read(client[i].socket, username, sizeof(username))) <= 0)
                {
                    if(n == 0)
                    {
                        close(client[i].socket);
                        FD_CLR(client[i].socket, &master);
                    }
                    else
                    {
                        perror("read failed");
                    }
                }
                else
                {
                    strcpy(client[i].username, username);
                    printf("\t%s just joined the chatroom!\n", client[i].username);
                }
            }
            else if(client[i].socket > 0 && FD_ISSET(client[i].socket, &read_fds))
            {
                memset(buf, 0, sizeof(buf));
                if((n = read(client[i].socket, buf, sizeof(buf))) <= 0)
                {
                    if(n == 0)
                    {
                        printf("\t%s left the chatroom!\n", client[i].username);
                    }
                    else
                    {
                        perror("read failed");
                    }
                    close(client[i].socket);
                    FD_CLR(client[i].socket, &master);
                }
                else
                {
                    for(j=0;j<=fd_max;j++)
                    {
                        if(j!=client[i].socket && j!=listener && FD_ISSET(j, &master))
                        {
                            
                            if(write(j, buf, n) == -1)
                                perror("write failed");
                            if(write(j, client[i].username, sizeof(client[i].username)) == -1)
                                perror("write failed");
                            strtok(buf, "\n");
                            printf("\t%s: %s\n", client[i].username, buf);
                        }
                    }
                }
            }
        }
    }
    close(listener);
    return 0;
}  
