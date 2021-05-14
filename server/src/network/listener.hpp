/*
Object that listens for new TCP connections on the specified ip/port number
and spawns a new connection object thread to deal with the connection
*/

#pragma once

#include <string>
#include <sys/socket.h>

using namespace std;


#define BACKLOG 10      // Max pending connections

class Listener {
public:
    int socket_id;         // socket id
    const char * ip;       // ip address
    int port;              // port number
    string last_error;       // string containing description of last thrown error


    /*
    Listener constructor
    Needs an ipv4 address (char array) and port number (integer)
    */
    Listener(const char * _ip, int _port);

    /*
    Start a listener for new TCP connections (blocking),
    incoming connections are handled via a new thread (connection operator() ).

    Returns -1 if there is an error and sets a string describing the error in Listener.last_error
    */
    int startListener();

};