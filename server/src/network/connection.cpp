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
    last_error = "";

    readable_ip = inet_ntoa(address.sin_addr);
    readable_ip += ":" + to_string(address.sin_port);

    Logger::log_message("New connection from " + readable_ip, 1, Logger::YELLOW);

    socket_id = _socket_id;
    addr_port = address.sin_port;

    // Conduct connection handshake
    if (this->handleHandshake() < 0) {
        Logger::log_message(last_error, 1, Logger::RED);  // close connection if there was an error
        this->close();
        return;
    }

    while (true) {

        // Print any data recieved on the socket
        int BUFSIZE = 128;
        char buffer[BUFSIZE] = {0};
        if (recv(socket_id, buffer, BUFSIZE - 1, 0) < 1) {
            //Logger::log_message("Connection closed to " + readable_ip, 1, Logger::YELLOW);
            this->close();      // close the connection if there was a buffer read error
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


void Connection::close() {

    // Close the file descriptor for this socket and log disconnection
    ::close(socket_id);
    Logger::log_message("Connection closed to " + readable_ip, 1, Logger::YELLOW);
    //--TODO--//
    // Code to close this thread!
 }


int Connection::send(char data[], int data_size) {

    //mtx.lock();
    int err = ::send(socket_id, data, data_size, 0);
    //mtx.unlock();

    return err;
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


int Connection::handleHandshake() {

    // Read in length of frame
    uint16_t length;
    if (recv(socket_id, &length, 2, 0) < 1) {
        last_error = "connection: handshake failed (1)";
        return -1;
    }

    // Translate frame length from network byte order
    length = be16toh(length);

    cout << length << endl;

    // Read in rest of frame
    char data[length];
    if (recv(socket_id, &data, length, 0) < 1) {
        last_error = "connection: handshake failed(2)";
        return -1;
    }

    // Digest client's *supposed* HELLO packet



    // Add connection to global map of all active connections
    connections.insert({socket_id, this});

    return 0;   // return success
}