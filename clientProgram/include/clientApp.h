#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include "clientInterface.h"

class ClientApp {
private:
    ClientInterface& client;

public:
    explicit ClientApp(ClientInterface& networkClient);
    void run(); // needs a return type
};

#endif // CLIENTAPP_H
