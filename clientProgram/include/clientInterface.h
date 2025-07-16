#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include "../../common/common.h"
#include <string>
#include <cstdint>
// int8_t, uint8_t

using namespace std;

class ClientInterface {
public:
    virtual void connectToServer(const string& address, uint16_t port) = 0;
    virtual void sendData(const TestStruct& data) = 0;
    virtual void receiveData(ResultStruct& result) = 0;
    virtual ~ClientInterface() = default;
    // want this to be virtual and public, but I don’t need any custom destructor logic. Just use the default
};

#endif // INETWORKCLIENT_H
