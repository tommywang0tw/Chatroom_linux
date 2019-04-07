#define BUFSIZE 255
#define USERNAMESIZE 20
#define MAX_CLIENTS 5
#define SERVER_IP "127.0.0.1"
typedef enum
{
    SET_USERNAME, 
    MESSAGE, 
    DISCONNECT, 
    CONNECT
} message_type;
typedef struct
{
    char msg[BUFSIZE];
    char username[USERNAMESIZE];
    message_type type;
} message;
typedef struct
{
    int socket;
    char username[USERNAMESIZE];
} client_info;
