#include "clientSocket.h"
#include "clientApp.h"

#include <iostream>

using namespace std;

int main() {
    try{

        ClientSocket socketClient;
        ClientApp app(socketClient);
        app.run();
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }
    return 0;
}



// .h files are for declarations and interfaces
// Interface (.h): What the class or function does

// Implementation (.cpp): How it does it