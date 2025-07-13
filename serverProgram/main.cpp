#include "serverSocket.h"
#include "serverApp.h"
#include <iostream>

int main() {
    try{
        SocketServer server(2001);  // Match this with client port
        ServerApp app(server);
        app.run();

    } catch ( const exception &ex){
        cerr << "Error: " << ex.what() << endl;
        return EXIT_FAILURE;
        
    }
    return 0;
}
