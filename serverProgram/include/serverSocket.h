#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "serverInterface.h"
#include <netinet/in.h>
#include <vector>
#include "../../common/configurations.h"

using namespace std;

class SocketServer : public ServerInterface {
private:
    void createMasterSocket();
    void bindMasterSocket();
    void listenConnections();
    void clientEvents(fd_set& readFds);
    bool selectSystemCall(fd_set& readFDs);
    int acceptSystemCall(sockaddr_in& clientAddr, socklen_t &len);
    void closeConnection(int& fd);
    void sendReceiveBytes(int& fd, fd_set& readFDs);


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
    SocketServer(int port, int maxClients = MAX_CLIENTS);
    ~SocketServer() override;

    void start() override;

};

#endif // SOCKETSERVER_H
