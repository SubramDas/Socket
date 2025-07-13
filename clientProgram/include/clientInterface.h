#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include <string>
#include <cstdint>

using namespace std;

struct TestStruct {
    unsigned int a;
    unsigned int b;
};

struct ResultStruct {
    unsigned int c;
};

class ClientInterface {
public:
    virtual bool connectToServer(const string& address, uint16_t port) = 0;
    virtual bool sendData(const TestStruct& data) = 0;
    virtual bool receiveData(ResultStruct& result) = 0;
    virtual ~ClientInterface() = default;
    // want this to be virtual and public, but I don’t need any custom destructor logic. Just use the default
};

#endif // INETWORKCLIENT_H
