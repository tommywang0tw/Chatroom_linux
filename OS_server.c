#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define MAX_CLIENTS 4
int main()
{
    int listener;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t len;
    int sock_client[MAX_CLIENTS];
    int fd_max;
    int i, j, n;
    char buf[32];
    //Initialize client array
    for(i=0;i<MAX_CLIENTS;i++)
    {
        sock_client[i] = 0;
    }
    fd_set master;
    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    /* 製作 socket */
    listener = socket(AF_INET, SOCK_STREAM, 0);
    
    /* 設定 socket */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listener, (struct sockaddr*)&addr, sizeof(addr));
    printf("\t[Info] binding...\n");
    
    /* 設定成等待來自 TCP 用戶端的連線要求狀態 */
    listen(listener, 5);
    printf("\t[Info] listening...\n");
    
    FD_SET(listener, &master);
    fd_max = listener;
    for(;;)
    {
        read_fds = master;
        if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }
        if(FD_ISSET(listener, &read_fds))
        {
            len = sizeof(client);
            for(i=0;i<MAX_CLIENTS;i++)
            {
                if(sock_client[i] == 0)
                {
                    sock_client[i] = accept(listener, (struct sockaddr *)&client, &len);
                    if(sock_client[i] == -1)
                    {
                        perror("client: ");
                    }
                    else
                    {
                        FD_SET(sock_client[i], &master);
                        if(sock_client[i] > fd_max)
                        {
                            fd_max = sock_client[i];
                        }
                        printf("\t[Info] Receive connection from %s on socket %d\n", inet_ntoa(client.sin_addr), sock_client[i]);
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
            if(sock_client[i] > 0 && FD_ISSET(sock_client[i], &read_fds))
            {
                memset(buf, 0, sizeof(buf));
                if((n = read(sock_client[i], buf, sizeof(buf))) <= 0)
                {
                    if(n == 0)
                    {
                        printf("client %d hung up\n", sock_client[i]);
                    }
                    else
                    {
                        perror("read:");
                    }
                    close(sock_client[i]);
                    FD_CLR(sock_client[i], &master);
                }
                else
                {
                    for(j=0;j<=fd_max;j++)
                    {
                        if(j!=sock_client[i] && j!=listener && FD_ISSET(j, &master))
                        {
                            if(write(j, buf, n) == -1)
                            {
                                perror("write:");
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
