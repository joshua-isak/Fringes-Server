#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <string>

#include "server/src/globals.hpp"

#include "connection.hpp"


void Connection::operator()(int socket_id, struct sockaddr_in address) {
    cout << "New connection from IP: " << inet_ntoa(address.sin_addr) << " Port: " << address.sin_port << endl;

    while (true) {

        // Print any data recieved on the socket
        int BUFSIZE = 128;
        char buffer[BUFSIZE] = {0};
        if (recv(socket_id, buffer, BUFSIZE - 1, 0) < 1) {
            cout << "connection closed" << endl;
            return;
        }
        cout << buffer;

        // Send data to the client
        string data = ships[1]->getJsonString();
        char outbuf[data.length()];
        strcpy(outbuf, data.c_str());
        ::send(socket_id, outbuf, sizeof(outbuf), 0);
    }
}


void Connection::close() { return; }


int Connection::send(char data[]) { return; }


int Connection::sendAll(char data[]) { return; }