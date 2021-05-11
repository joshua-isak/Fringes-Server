#pragma once

#include <string>
#include <sys/socket.h>


using namespace std;

#define BACKLOG 10      // Max pending connections

class Connection {
public:
    int socket_id;         // socket id
    const char * ip;       // ip address
    int port;              // port number
    string last_error;       // string containing description of last thrown error


    // Constructor
    Connection(int _socket, const char * _ip, int _port);

    // Start a listener for new TCP connections (blocking)
    int startListener();

    // Threadfunction to handle a new connection
    int handleConnection(Connection *master, int socket_id, struct sockaddr_in address);

    // void operator()(Connection *master, int socket_id, struct sockaddr_in address);
};