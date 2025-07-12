#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <memory.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 2000;
int MAX_CLIENTS = 5;

int monitored_fd_set[6];
char dataBuffer[1024];

typedef struct _test_struct{
    unsigned int a;
    unsigned int b;
} test_struct_t;

test_struct_t test;

typedef struct _result_struct{
    unsigned int c;
} result_struct_t;


void initialize_monitor_fd_set(){
    
    for(int i = 0; i<MAX_CLIENTS; i++){
        monitored_fd_set[i] = -1;
    }
}

void add_to_monitor_fd_set(int socketFD){
    
    for(int i = 0; i<MAX_CLIENTS; i++){
        if (monitored_fd_set[i] != -1){
            continue;
        }

        monitored_fd_set[i] = socketFD;
        break;
    }
}

void remove_from_monitored_fd_set(int socketFD){
    
    for(int i = 0; i<MAX_CLIENTS; i++){
        if (monitored_fd_set[i] != socketFD){
            continue;
        }

        monitored_fd_set[i] = -1;
        break;
    }
}

void re_init_readfds(fd_set *fd_set_ptr){
        FD_ZERO(fd_set_ptr);
        for(int i = 0; i<MAX_CLIENTS; i++){
            if (monitored_fd_set[i] != -1){
                FD_SET(monitored_fd_set[i], fd_set_ptr);
            }
        }

}

int get_max_fd(){
    int max = -1;
    for(int i = 0; i<MAX_CLIENTS; i++){
        if (monitored_fd_set[i] > max){
            max = monitored_fd_set[i];
        }
    }

    return max;
}



void serverSetup(){
    // Initialization
    int masterSocketFD = 0;
    int recvBytes = 0;
    int addressLength = 0;
    int opt = 1;

    int communicationSocketFD = 0;
    fd_set readFDs;
    
    struct sockaddr_in serverAddress, clientAddress;

    initialize_monitor_fd_set();

    // Create master socket file descriptor
    if ((masterSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        printf("Socket creation failed");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = SERVER_PORT;
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    addressLength = sizeof(struct sockaddr);

    if( bind(masterSocketFD, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) == -1){
        printf("Socket bind failed");
        return;
    }

    if(listen(masterSocketFD, 5) < 0){
        printf("Listen failed");
        return;
    }

    add_to_monitor_fd_set(masterSocketFD);


    while(1){

        re_init_readfds(&readFDs);

        printf("Blocked on select system call! \n");

        select(get_max_fd() + 1, &readFDs, NULL, NULL, NULL);

        if( FD_ISSET(masterSocketFD, &readFDs)){
            printf("New Connection received");

            
            socklen_t addressLength = sizeof(clientAddress);

            communicationSocketFD = accept(masterSocketFD, (struct sockaddr *)&clientAddress, &addressLength);
            
            // communicationSocketFD = accept(masterSocketFD, (struct sockaddr *)&clientAddress, &addressLength);
            
            if(communicationSocketFD < 0){
                printf("Accept error : errno = \n", errno);
                exit(0);
            }

            add_to_monitor_fd_set(communicationSocketFD);
            
            printf("Connection accepted from client %s:%u", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
            
        }else{
            int communicationSocketFD = -1;
            for(int i = 0; i<MAX_CLIENTS; i++){
               if( FD_ISSET(monitored_fd_set[i], &readFDs)){
                
                communicationSocketFD = monitored_fd_set[i];
                
                memset(dataBuffer, 0, sizeof(dataBuffer));
                
                socklen_t addressLength = sizeof(clientAddress);
                
                recvBytes = recvfrom(communicationSocketFD, (char *)dataBuffer, sizeof(dataBuffer), 0, (struct sockaddr *)&clientAddress, &addressLength);

                printf("Server received %u bytes from client %s:%u", recvBytes, inet_ntoa(clientAddress.sin_addr), (unsigned int)ntohs(clientAddress.sin_port));

                if(recvBytes == 0){
                    close(communicationSocketFD);
                    remove_from_monitored_fd_set(communicationSocketFD);
                    break;
                }

                test_struct_t *clientData = (test_struct_t *)dataBuffer;

                if(clientData->a == 0 && clientData->b == 0){
                    close(communicationSocketFD);
                    remove_from_monitored_fd_set(communicationSocketFD);
                    printf("Server closed connection with client: %s:%u", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

                    break;
                }

                result_struct_t result;
                result.c = clientData->a + clientData->b;

                recvBytes = sendto(communicationSocketFD, (char *)&result, sizeof(result_struct_t), 0, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr));

                printf("Server sent %d bytes in reply to client", recvBytes);

               }
                
            }
        }
    }
}


int main(){
    serverSetup();
    return 0;
}
