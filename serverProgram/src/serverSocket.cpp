#include "serverSocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "../../common/configurations.h"

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
    // clears all bits in the set
    FD_ZERO(readFDs);
    for (int fd : clientSockets_) {
        if (fd != -1){
            FD_SET(fd, readFDs);
        }

        // FD_SET sets the bit corresponding to the file descriptor fd in the readFDs set, indicating that the socket is ready to be read from.
    }
}

int SocketServer::getMaxFD() const {
    int maxFD = -1;
    for (int fd : clientSockets_) {
        if (fd > maxFD) maxFD = fd;
    }
    return maxFD;
}

void SocketServer:: createMasterSocket(){
    masterSocketFD_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (masterSocketFD_ == -1) {
        throw runtime_error("Socket creation failed: " + string(strerror(errno)));
    }
}

void SocketServer:: bindMasterSocket(){

    int bindSocket = bind(masterSocketFD_, reinterpret_cast<sockaddr*>(&serverAddr_), sizeof(serverAddr_));
    
    if (bindSocket < 0) {
        throw runtime_error("Bind failed: " + string(strerror(errno)));
    }

}

void SocketServer::listenConnections(){
    if (listen(masterSocketFD_, LISTEN_BUFFER) < 0) {
        throw runtime_error("Listen failed: " + string(strerror(errno)));
    }
}

bool SocketServer::selectSystemCall(fd_set& readFDs){
    int selectCall = select(getMaxFD() + 1, &readFDs, nullptr, nullptr, nullptr);
    if ( selectCall < 0) {
        cerr << "Select failed\n";
        return false;
    }

    return true;
}

int SocketServer::acceptSystemCall(sockaddr_in& clientAddr, socklen_t &len){
    int clientFD = accept(masterSocketFD_, reinterpret_cast<sockaddr*>(&clientAddr), &len);

    if (clientFD == -1) {
        cerr << "Accept failed\n";
        return -1;
    }

    return clientFD;
}

void SocketServer::closeConnection(int& fd){
    
    close(fd);
    removeClient(fd);
}

void SocketServer::sendReceiveBytes(int& fd, fd_set& readFDs){

    if (fd != -1 && fd != masterSocketFD_ && FD_ISSET(fd, &readFDs)) {
    
        TestStruct request;
        ssize_t recvBytes = recv(fd, &request, sizeof(request), 0);
    
        if (recvBytes <= 0) {
            cout << "Client disconnected\n";
            closeConnection(fd);
            return;
        }

        if (request.a == 0 && request.b == 0) {
            cout << "Client requested shutdown\n";
            closeConnection(fd);
            return;
        }

        ResultStruct result;
        result.c = request.a + request.b;

        ssize_t sentBytes = send(fd, &result, sizeof(result), 0);
        cout << "Processed " << recvBytes << " bytes; Sent " << sentBytes << " bytes\n";
    }
} 


void SocketServer::clientEvents(fd_set& readFDs){
        
        refreshFDSet(&readFDs);
        cout << "\nWaiting for client events...\n";

        // wait for I/O operations on multiple file descriptors
        bool selectCall = selectSystemCall(readFDs);
        if(selectCall == false) return;

        // checks if the master socket is in the set
        if (FD_ISSET(masterSocketFD_, &readFDs)) {

            sockaddr_in clientAddr;
            socklen_t len = sizeof(clientAddr);

            int clientFD = acceptSystemCall(clientAddr, len);
            if(clientFD == -1) return;

            addClient(clientFD);

            cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "\n";

            // inet_ntoa function converts the binary IP address stored in clientAddr.sin_addr to a string in dotted decimal notation

            // ntohs: from network byte order to host byte order
        }

        for (int& fd : clientSockets_) {
            sendReceiveBytes(fd, readFDs);
        }
}


void SocketServer::start() {
    initializeMonitorFDs();

    createMasterSocket();

    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(port_);
    serverAddr_.sin_addr.s_addr = inet_addr(SERVER_IP);
    // INADDR_ANY: listen for incoming connections on all available network interfaces.

    bindMasterSocket();

    listenConnections();

    addClient(masterSocketFD_);

    cout << "Server listening on port " << port_ << "\n";

    fd_set readFDs;

    while (true) {
        clientEvents(readFDs);
    }
}
