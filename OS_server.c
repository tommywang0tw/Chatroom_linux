#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int main()
{
    int sock0;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t len;
    int sock_client, n;
    char buf[32];
    
    /* 製作 socket */
    sock0 = socket(AF_INET, SOCK_STREAM, 0);
    
    /* 設定 socket */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock0, (struct sockaddr*)&addr, sizeof(addr));
    printf("\t[Info] binding...\n");
    
    /* 設定成等待來自 TCP 用戶端的連線要求狀態 */
    listen(sock0, 5);
    printf("\t[Info] listening...\n");
    
    /* 接受來自 TCP 用戶端地連線要求*/
    printf("\t[Info] wait for connection...\n");
    len = sizeof(client);
    sock_client = accept(sock0, (struct sockaddr *)&client, &len);
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
    printf("\t[Info] Close client connection...\n");
    close(sock_client);
    
    /* 結束 listen 的 socket */
    printf("\t[Info] Close self connection...\n");
    close(sock0);
    return 0;
}  
