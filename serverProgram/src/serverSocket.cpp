#include "serverSocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

using namespace std;

SocketServer::SocketServer(int port, int maxClients)
    : port_(port), maxClients_(maxClients), masterSocketFD_(-1), clientSockets_(maxClients + 1, -1) {
    memset(&serverAddr_, 0, sizeof(serverAddr_));
}

SocketServer::~SocketServer() {
    if (masterSocketFD_ != -1){
        close(masterSocketFD_);
    }

    for (int fd : clientSockets_) {
        if (fd != -1){
            close(fd);
        }
    }
}

void SocketServer::initializeMonitorFDs() {
    fill(clientSockets_.begin(), clientSockets_.end(), -1);
}

void SocketServer::addClient(int socketFD) {
    for (int& fd : clientSockets_) {
        if (fd == -1) {
            fd = socketFD;
            break;
        }
    }
}

void SocketServer::removeClient(int socketFD) {
    for (int& fd : clientSockets_) {
        if (fd == socketFD) {
            fd = -1;
            break;
        }
    }
}

void SocketServer::refreshFDSet(fd_set* readFDs) {
    FD_ZERO(readFDs);
    for (int fd : clientSockets_) {
        if (fd != -1) FD_SET(fd, readFDs);
    }
}

int SocketServer::getMaxFD() const {
    int maxFD = -1;
    for (int fd : clientSockets_) {
        if (fd > maxFD) maxFD = fd;
    }
    return maxFD;
}

void SocketServer::start() {
    initializeMonitorFDs();

    masterSocketFD_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (masterSocketFD_ < 0) {
        throw runtime_error("Socket creation failed: " + string(strerror(errno)));
    }

    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(port_);
    serverAddr_.sin_addr.s_addr = INADDR_ANY;

    if (bind(masterSocketFD_, reinterpret_cast<sockaddr*>(&serverAddr_), sizeof(serverAddr_)) < 0) {
        throw runtime_error("Bind failed: " + string(strerror(errno)));
        
    }

    if (listen(masterSocketFD_, 5) < 0) {
        throw runtime_error("Listen failed: " + string(strerror(errno)));
    }

    addClient(masterSocketFD_);

    cout << "Server listening on port " << port_ << "\n";

    fd_set readFDs;

    while (true) {
        refreshFDSet(&readFDs);
        cout << "\nWaiting for client events...\n";

        if (select(getMaxFD() + 1, &readFDs, nullptr, nullptr, nullptr) < 0) {
            cerr << "Select failed\n";
            continue;
        }

        if (FD_ISSET(masterSocketFD_, &readFDs)) {
            sockaddr_in clientAddr;
            socklen_t len = sizeof(clientAddr);
            int clientFD = accept(masterSocketFD_, reinterpret_cast<sockaddr*>(&clientAddr), &len);
            if (clientFD < 0) {
                cerr << "Accept failed\n";
                continue;
            }
            addClient(clientFD);
            cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "\n";
        }

        for (int& fd : clientSockets_) {
            if (fd != -1 && fd != masterSocketFD_ && FD_ISSET(fd, &readFDs)) {
                TestStruct request;
                ssize_t recvBytes = recv(fd, &request, sizeof(request), 0);
                if (recvBytes <= 0) {
                    cout << "Client disconnected\n";
                    close(fd);
                    removeClient(fd);
                    continue;
                }

                if (request.a == 0 && request.b == 0) {
                    cout << "Client requested shutdown\n";
                    close(fd);
                    removeClient(fd);
                    continue;
                }

                ResultStruct result;
                result.c = request.a + request.b;

                ssize_t sentBytes = send(fd, &result, sizeof(result), 0);
                cout << "Processed " << recvBytes << " bytes; Sent " << sentBytes << " bytes\n";
            }
        }
    }
}
