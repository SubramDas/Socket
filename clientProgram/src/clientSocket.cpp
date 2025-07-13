#include "clientSocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

ClientSocket::ClientSocket() {
    socketFD = -1;
    memset(&serverAddr, 0, sizeof(serverAddr));
}

ClientSocket::~ClientSocket() {
    if (socketFD != -1) {
        close(socketFD);
    }
}

bool ClientSocket::connectToServer(const string& address, uint16_t port) {
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socketFD == -1) {
        throw std::runtime_error("Socket creation failed: " + string(strerror(errno)));
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    hostent* host = gethostbyname(address.c_str());

    if (!host) {
        throw runtime_error("Hostname resolution failed for: " + address);
    }

    serverAddr.sin_addr = *(reinterpret_cast<in_addr*>(host->h_addr_list[0]));

    if (connect(socketFD, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Connection to server failed: " + std::string(strerror(errno)));
    }

    return true;
}

bool ClientSocket::sendData(const TestStruct& data) {
    ssize_t sentBytes = send(socketFD, &data, sizeof(TestStruct), 0);

    if (sentBytes != sizeof(TestStruct)) {
        throw runtime_error("Failed to send full TestStruct data: " + std::string(strerror(errno)));
    }

    cout << "Sent " << sentBytes << " bytes.\n";
    return true;
}

bool ClientSocket::receiveData(ResultStruct& result) {
    ssize_t recvBytes = recv(socketFD, &result, sizeof(ResultStruct), 0);
    if (recvBytes <= 0) {
        throw runtime_error("Failed to receive ResultStruct from server: " + std::string(strerror(errno)));
    }

    cout << "Received " << recvBytes << " bytes.\n";
    return true;
}
