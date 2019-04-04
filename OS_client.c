#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int main()
{
    struct sockaddr_in server;
    int sock;
    char *msg = "Message back!\n";
    char buf[32];
    int n;
    
    /* 製作 socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    /* 準備連線端指定用的 struct 資料 */
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    
    /* 127.0.0.1 是 localhost 本機位址 */
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);
    
    /* 與 server 端連線 */
    connect(sock, (struct sockaddr *)&server, sizeof(server));
    
    /* 從伺服器接受資料 */
    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));
    
    printf("\t[Info] Receive %d bytes: %s\n", n, buf);
    
    /*Send message to server*/
    n = write(sock, msg, strlen(msg));
    printf("\t[info] Send %d bytes: %s\n", n, msg);
    
    /*Send a message typed by user to server*/
    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf), stdin);
    n = write(sock, buf, sizeof(buf));
    printf("\t[Info] Send %d bytes: %s\n", n, buf);

    /* 結束 socket */
    close(sock);
    return 0;
}
