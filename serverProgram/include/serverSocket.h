#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "serverInterface.h"
#include <netinet/in.h>
#include <vector>

using namespace std;

struct TestStruct {
    unsigned int a;
    unsigned int b;
};

struct ResultStruct {
    unsigned int c;
};

class SocketServer : public ServerInterface {
private:
    void initializeMonitorFDs();
    void addClient(int socketFD);
    void removeClient(int socketFD);
    void refreshFDSet(fd_set* readFDs);
    int getMaxFD() const; // read only

    int port_;
    int masterSocketFD_;
    int maxClients_;
    vector<int> clientSockets_;
    sockaddr_in serverAddr_;

public:
    SocketServer(int port, int maxClients = 5);
    ~SocketServer() override;

    void start() override;

};

#endif // SOCKETSERVER_H
