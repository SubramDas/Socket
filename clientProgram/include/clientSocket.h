#ifndef ClientSocket_H
#define ClientSocket_H

#include "clientInterface.h"
#include <netinet/in.h>
// htons(), ntohs()

using namespace std;

class ClientSocket : public ClientInterface {
private:
    int socketFD;
    sockaddr_in serverAddr; // structure defined for socket address 

    void createSocket();
    void hostResolution(const string& IPaddress);
    void connectSocketFD();

public:
    ClientSocket();
    ~ClientSocket() override;

    void connectToServer(const string& address, uint16_t port) override;
    void sendData(const TestStruct& data) override;
    void receiveData(ResultStruct& result) override;
};

#endif // ClientSocket_H
