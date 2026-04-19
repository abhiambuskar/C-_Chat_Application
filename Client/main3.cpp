#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
using namespace std;
#pragma comment(lib, "ws2_32.lib")


// intialize winsock
// create socket
// connect to server
// send and revc
// close the socket

bool Initialize(){
    WSAData data;
    return WSAStartup(MAKEWORD(2,2), &data) == 0;
}

void SendMessages(SOCKET s){
    cout<<"Enter the chat name"<<endl;
    string name;
    getline(cin, name);
    string message;
    while(1){
        getline(cin, message);
        string msg = name +" : "+message;
        int bytesend = send(s, msg.c_str(), msg.length(),0);
        if(bytesend == SOCKET_ERROR){
            cout<<"Error while sending the message "<<endl;
            break;
        }

        if(message == "quit"){
            cout<<"Stopping the application"<<endl;
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}

void ReceiveMessages(SOCKET s){
    char buffer[4096];
    int recvlength;
    string msg;
    while(1){
        recvlength = recv(s, buffer, sizeof(buffer),0);
        if(recvlength <= 0){
            cout<<"Error while receiving"<<endl;
            break;
        }else{
            msg = string(buffer,recvlength);
            cout<<msg<<endl;
        }
    }
    closesocket(s);
    WSACleanup();
}

int main(){
    if(!Initialize()){
        cout<<"Initialize not properly"<<endl;
        return 1;
    }

    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM,0);
    if(s == INVALID_SOCKET){
        cout<<"INVALID SOCKET Created"<<endl;
        return 1;
    }

    
    int port = 12345;
    string serveraddrss = "127.0.0.1";
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, serveraddrss.c_str(), &(serveraddr.sin_addr));

    if(connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR){
        cout<<"Not able to connect to server"<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }   

    cout<<"Sucessfully connected to server"<<endl;

    thread senderthread(SendMessages, s);
    thread receivethread(ReceiveMessages, s);

    senderthread.join();
    receivethread.join();
    return 1;
}