#ifndef ClientSocket_H
#define ClientSocket_H

#include "clientInterface.h"
#include <netinet/in.h>

using namespace std;

class ClientSocket : public ClientInterface {
private:
    int socketFD;
    sockaddr_in serverAddr;

public:
    ClientSocket();
    ~ClientSocket() override;

    bool connectToServer(const string& address, uint16_t port) override;
    bool sendData(const TestStruct& data) override;
    bool receiveData(ResultStruct& result) override;
};

#endif // ClientSocket_H
