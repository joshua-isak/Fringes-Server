#pragma once

#include <string>
#include <sys/socket.h>

using namespace std;


class Connection {
    public:

    // Threadable function to handle a TCP connection
    void operator()(int socket_id, struct sockaddr_in address);

    // Close the connection
    void close();

    // Send data to this connection, returns -1 for error,
    int send(char data[]);

    // Send data to all connections
    int sendAll(char data[]);
};