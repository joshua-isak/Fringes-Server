#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <string>

#include "server/src/globals.hpp"

#include "connection.hpp"
#include "server/src/logger.hpp"


void Connection::operator()(int _socket_id, struct sockaddr_in address) {
    string readable_ip = inet_ntoa(address.sin_addr);
    readable_ip += ":" + to_string(address.sin_port);

    Logger::log_message("New connection from " + readable_ip, 0, Logger::YELLOW);

    //cout << "New connection from IP: " << inet_ntoa(address.sin_addr) << " Port: " << address.sin_port << endl;

    socket_id = _socket_id;
    addr_port = address.sin_port;

    // Add connection to global map of all active connections
    connections.insert({socket_id, this});

    while (true) {

        // Print any data recieved on the socket
        int BUFSIZE = 128;
        char buffer[BUFSIZE] = {0};
        if (recv(socket_id, buffer, BUFSIZE - 1, 0) < 1) {
            Logger::log_message("Connection closed to " + readable_ip, 0, Logger::YELLOW);
            return;
        }
        cout << buffer;

        // Send data to the client
        string data = ships[1]->getJsonString();
        char outbuf[data.length()];
        strcpy(outbuf, data.c_str());
        this->send(outbuf, sizeof(outbuf));
    }
}


void Connection::close() { return; }


int Connection::send(char data[], int data_size) {
    return ::send(socket_id, data, data_size, 0);
}


int Connection::sendAll(char data[], int data_size) {

    map <int, Connection*>::iterator it;
    int error;

    // Iterate through all connections and call send()
    for (it = connections.begin(); it != connections.end(); it++) {

        Connection *this_conn = it->second;

        error = this_conn->send(data, data_size);

        if (error == -1) { return -1; }
    }

    return error;
}