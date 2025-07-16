#ifndef SERVERAPP_H
#define SERVERAPP_H

#include "serverInterface.h"

class ServerApp {
private:
    ServerInterface& server;

public:
    ServerApp(ServerInterface& srv);
    void run();
};

#endif // SERVERAPP_H
