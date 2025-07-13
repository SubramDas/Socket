#include "serverApp.h"

ServerApp::ServerApp(ServerInterface& srv) : server(srv) {}

void ServerApp::run() {
    server.start();
}
