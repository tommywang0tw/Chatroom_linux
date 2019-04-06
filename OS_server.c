#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int main()
{
    int listener;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t len;
    int sock_client;
    int fd_max;
    int i, j, n;
    char buf[32];
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
        /*
        len = sizeof(client);
        sock_client = accept(listener, (struct sockaddr *)&client, &len);
        printf("\t[Info] Receive connection from %s on socket %d\n", inet_ntoa(client.sin_addr), sock_client);        
        */
        read_fds = master;
        printf("before select\n");
        if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }
        printf("Pass select\n");
        for(i=0;i<=fd_max;i++)
        {
            if(FD_ISSET(i, &master))
            {
                len = sizeof(client);
                sock_client = accept(listener, (struct sockaddr *)&client, &len);
                if(sock_client == -1)
                {
                    perror("accept");
                }
                else
                {
                    FD_SET(sock_client, &master);
                    if(sock_client > fd_max)
                    {
                        fd_max = sock_client;
                    }
                    printf("\t[Info] Receive connection from %s on socket %d\n", inet_ntoa(client.sin_addr), sock_client);
                }
            }
            else
            {
                if(FD_ISSET(i, &master))
                {
                    if((n = read(i, buf, sizeof(buf))) <= 0)
                    {
                        if(n == 0)
                        {
                            printf("socket %d hung up.\n", i);
                        }   
                        else
                        {
                            perror("read");
                        }
                        close(i);
                        FD_CLR(i, &master);
                    }
                    else
                    {
                        for(j=0; j<=fd_max;j++)
                        {
                            if(j!=i && j!=listener)
                            {
                                if(write(j, buf, n) == -1)
                                {
                                    perror("write");
                                }
                            }
                        }
                    }   
                }
            }
        } 
    }
        
    /* 接受來自 TCP 用戶端地連線要求*/
    
    printf("\t[Info] wait for connection...\n");
    len = sizeof(client);
    sock_client = accept(listener, (struct sockaddr *)&client, &len);
    printf("\t[Info] Testing...\n");
    char *paddr_str = inet_ntoa(client.sin_addr);
    printf("\t[Info] Receive connection from %s...\n", paddr_str);
    
    /* 傳送 5 個字元 */
    
    printf("\t[Info] Say hello back...\n");
    write(sock_client, "HELLO\n", 6);
    
    /*Get message from client*/
    
    memset(buf, 0, sizeof(buf));
    printf("\t[Info] Get message from client...\n");
    n = read(sock_client, buf, sizeof(buf));    
    printf("\t[Info] Receive %d bytes: %s\n", n, buf);
    
    /*Get another message*/
    
    memset(buf, 0, sizeof(buf));
    printf("\t[Info] Get message from client...\n");
    n = read(sock_client, buf, sizeof(buf));    
    printf("\t[Info] Receive %d bytes: %s\n", n, buf);
    
    /* 結束 TCP 對話 */
    /*
    printf("\t[Info] Close client connection...\n");
    close(sock_client);
    */
    /* 結束 listen 的 socket */
    printf("\t[Info] Close self connection...\n");
    close(listener);
    return 0;
}  
