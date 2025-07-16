#include "clientSocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h> // gethostbyname()
#include <arpa/inet.h> // IPPROTO_TCP
#include <sys/socket.h>

using namespace std;

ClientSocket::ClientSocket() {
    
    // Socket not created yet
    socketFD = -1; 
    memset(&serverAddr, 0, sizeof(serverAddr));
}

ClientSocket::~ClientSocket() {

    if (socketFD != -1) {
        close(socketFD);
    }
}

void ClientSocket:: createSocket(){
    
    // AF_INET - ipv4 IPaddress family
    // SOCK_STREAM - connection oriented, for connectionless SOCK_DGRAM
    // IPPROTO_TCP - TCP 
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socketFD == -1) {
        throw runtime_error("Socket creation failed: " + string(strerror(errno)));
    }
}

void ClientSocket:: hostResolution(const string& IPaddress){
    // getting all host details like name, address, address length, etc
    hostent* host = gethostbyname(IPaddress.c_str());

    if (!host) {
        throw runtime_error("Hostname resolution failed for: " + IPaddress);
    }

    // Converts the pointer to the first IP address in the list to a pointer to an in_addr structure
    serverAddr.sin_addr = *(reinterpret_cast<in_addr*>(host->h_addr_list[0]));
}

void ClientSocket:: connectSocketFD(){
    // getting all host details like name, address, address length, etc
    if (connect(socketFD, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        throw std::runtime_error("Connection to server failed: " + std::string(strerror(errno)));
    }
}

void ClientSocket::connectToServer(const string& IPaddress, uint16_t port) {

    createSocket();

    serverAddr.sin_family = AF_INET;
    // htons is a function that converts a 16-bit integer from host byte order to network byte order.
    serverAddr.sin_port = htons(port);

    hostResolution(IPaddress);

    connectSocketFD();
}

void ClientSocket::sendData(const TestStruct& data) {
    // signed integer type that can represent the number of bytes sent or an error code.
    ssize_t sentBytes = send(socketFD, &data, sizeof(TestStruct), 0);

    if (sentBytes != sizeof(TestStruct)) {
        throw runtime_error("Failed to send full TestStruct data: " + string(strerror(errno)));
    }

    cout << "Sent " << sentBytes << " bytes.\n";
}

void ClientSocket::receiveData(ResultStruct& result) {
    ssize_t recvBytes = recv(socketFD, &result, sizeof(ResultStruct), 0);
    cout<<recvBytes<<endl;
    if (recvBytes <= 0) {
        throw runtime_error("Failed to receive Result from server: ");
    }

    cout << "Received " << recvBytes << " bytes.\n";
}
