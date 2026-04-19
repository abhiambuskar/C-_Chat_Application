#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <algorithm>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

bool Initialize(){
    WSAData data;
    return WSAStartup(MAKEWORD(2,2), &data) == 0;
}

void interactwithClient(SOCKET clientSocket, vector<SOCKET>& clients){
    cout<<"Client connected"<<endl;
    char buffer[4096];
    while(1){
        int byterecv = recv(clientSocket, buffer, sizeof(buffer),0);
        if(byterecv <= 0){
            cout<<"Client disconnected" <<endl;
            break;
        }
        string message(buffer, byterecv);
        cout<<"Message from client "<<message<<endl;

        for(auto client: clients){
            if(client != clientSocket){
                send(client, message.c_str(), message.length(),0);
            }
        }
        
    }


    auto it = find(clients.begin(), clients.end(), clientSocket);
    if(it != clients.end()){
        clients.erase(it);
    }
    closesocket(clientSocket);
    
}

int main(){
    if(!Initialize()){
        cout<<"Initialize not properly"<<endl;
        return 1;
    }
    cout<<"Hello "<<endl;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(listenSocket == INVALID_SOCKET){
        cout<<"Socket creation failed" <<endl;
        return 1;
    }

    //create address structure
    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    //convert ipaddress put inside the sin_family to binary format
    if(InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1){
        cout<<"Setting address structure failed"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    //bind
    if(bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR){
        cout<<"Binding failed"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    //listen
    if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR){
        cout<<"Listening error"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout<<"Server has started to listening to port" << port<<endl;

    vector<SOCKET>clients;
    while(1){
        //accept
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if(clientSocket == INVALID_SOCKET){
            cout<<"Invalid client socket"<<endl;
        }
        clients.push_back(clientSocket);
        thread t1(interactwithClient, clientSocket, std::ref(clients));
        t1.detach();
    }



    // cout<<"Client connected"<<endl;
    // char buffer[4096];
    // int byterecv = recv(clientSocket, buffer, sizeof(buffer),0);

    // string message(buffer, byterecv);
    // cout<<"Message from client "<<message<<endl;
    // closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}