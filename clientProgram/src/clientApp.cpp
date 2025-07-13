#include "clientApp.h"
#include <iostream>

using namespace std;

ClientApp::ClientApp(ClientInterface& networkClient) : client(networkClient) {}

void ClientApp::run() {
    client.connectToServer("127.0.0.1", 2001);
    
    // if (!client.connectToServer("127.0.0.1", 2001)) {
    //     cerr << "Unable to connect to server.\n";
    //     return;
    // }

    while (true) {
        TestStruct inputData{};
        cout << "Enter a: ";
        cin >> inputData.a;
        cout << "Enter b: ";
        cin >> inputData.b;

        if (!client.sendData(inputData)) break;

        ResultStruct result{};
        if (!client.receiveData(result)) break;

        cout << "Result: " << result.c << "\n";
    }
}
