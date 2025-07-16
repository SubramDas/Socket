#include "clientApp.h"
#include "../../common/configurations.h"
#include <iostream>

using namespace std;

ClientApp::ClientApp(ClientInterface& networkClient) : client(networkClient) {}

void ClientApp::run() {
    client.connectToServer(SERVER_IP, SERVER_PORT);

    while(true){
       processClientRequests();
    }
}

void ClientApp::processClientRequests(){
        
        // Initialize with 0 values
        TestStruct inputData{}; 
        ResultStruct result{};
        
        cout << "Enter a: ";
        cin >> inputData.a;
        cout << "Enter b: ";
        cin >> inputData.b ;

        client.sendData(inputData);
        client.receiveData(result);
        
        cout << "Result: " << result.c << "\n";
}
