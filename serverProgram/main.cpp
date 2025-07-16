#include "serverSocket.h"
#include "serverApp.h"
#include <iostream>
#include "../common/configurations.h"

int main() {
    try{
        SocketServer server(SERVER_PORT);  
        ServerApp app(server);
        app.run();

    } catch ( const exception &ex){
        cerr << "Error: " << ex.what() << endl;
        return EXIT_FAILURE;
        
    }
    return 0;
}
