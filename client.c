// #include <stdlib.h>
#include <stdio.h>
// #include <sys/socket.h>
#include <netdb.h>
// #include <netinet/in.h>
// #include <memory.h>
// #include <errno.h>
// #include <sys/select.h>



typedef struct _test_struct{
    unsigned int a;
    unsigned int b;
} test_struct_t;


typedef struct _result_struct{
    unsigned int c;
} result_struct_t;


#define DEST_PORT 2000
#define SERVER_IP_ADDRESS "127.0.0.1"

test_struct_t clientData;
result_struct_t result;

void clientSetup(){
    int socketFD = 0;
    int recvBytes = 0;
    // int addressLength = sizeof(struct sockaddr);

    struct sockaddr_in destination;

    destination.sin_family = AF_INET;
    destination.sin_port = DEST_PORT;

    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP_ADDRESS);
    destination.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    connect(socketFD, (struct sockaddr *)&destination, sizeof(struct sockaddr));

PROMPT_USER:

    printf("Enter a: ");
    scanf("%u", &clientData.a);

    printf("Enter b: ");
    scanf("%u", &clientData.b);

    recvBytes = sendto(socketFD, &clientData, sizeof(test_struct_t), 0, (struct sockaddr *)&destination, sizeof(struct sockaddr));

    printf("Received %u:",recvBytes);
    
    socklen_t addressLength = sizeof(destination);

    recvBytes = recvfrom(socketFD, (char *)&result, sizeof(result_struct_t), 0, (struct sockaddr *)&destination, &addressLength);

    printf("Received %u:", recvBytes);

    printf("Result %u:", result.c);

    goto PROMPT_USER;
}

int main(){
    clientSetup();
}
