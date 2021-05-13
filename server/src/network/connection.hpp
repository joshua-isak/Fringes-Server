#pragma once

#include <string>
#include <sys/socket.h>
#include <mutex>

using namespace std;


class Connection {
public:
    string readable_ip;     // human readable representation of ipv4 address
    int addr_port;          // human readable representation of port number
    int socket_id;          // file descriptor for this socket
    string last_error;      // string containing description of last error
    //mutex mtx;              // mutex to lock critical sections of code


    // Threadable function to handle a TCP connection
    void operator()(int socket_id, struct sockaddr_in address);

    // Close this connection
    void close();

    // Send data to this connection, returns -1 for error or # of bytes sent
    int send(char data[], int data_size);

    // Send data to all active connections, returns -1 for error
    static int sendAll(char data[], int data_size);

    // Handle client handshake, returns -1 for error
    int handleHandshake();
};