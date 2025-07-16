#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include "clientInterface.h"

class ClientApp {
private:
    ClientInterface& client;
    void processClientRequests();

public:
    // Prevents unexpected conversions in case of complex objects
    explicit ClientApp(ClientInterface& networkClient); 
    
    // needs a return type
    void run(); 
};

#endif // CLIENTAPP_H
