/*
Object that mannages an active TCP connection.

Conducts a handshake to get client information (username, version, password)
and verifies that it is correct. Then syncs game information to the client and
continually updates it to game state changes on the server. Also handles commands
sent from the client in a loop until the connection is closed.
*/

#pragma once

#include <string>
#include <sys/socket.h>
#include <mutex>

using namespace std;


class Connection {
private:
    string readable_ip;     // human readable representation of ipv4 address
    int addr_port;          // human readable representation of port number
    int socket_id;          // file descriptor for this socket
    string last_error;      // description of last error
    string username;        // username of connected client
    //mutex mtx;              // mutex to lock critical sections of code //--TODO-- make a pointer

public:

    // Threadable function to handle a TCP connection
    void operator()(int socket_id, struct sockaddr_in address);

    // Close this connection
    void close();

    // Send data to this connection, returns -1 for error or # of bytes sent
    int send(char data[], int data_size);

    // Send data to all active connections, returns -1 for error
    static int sendAll(char data[], int data_size);

    // Do client handshake, returns -1 for error
    int doHandshake();

    // Prepend frame_len (data_size) to data[] and send it to this connection, returns send()'s values
    int sendFrame(char data[], int data_size);

    // sendFrame to all active connections, returns -1 for error
    static int sendFrameAll(char data[], int data_size);

    // send json data and a sync value for a single ship to conn_id (conn_id 0 to send to all connections)
    static int syncShip(int conn_id, string sync_type, string json_data);

    // send json data and a sync value for a single station to conn_id (conn_id 0 to send to all connections)
    static int syncStation(int conn_id, string sync_type, string json_data);

    // depart a ship to a destination station, takes data to read from and current seek of that data
    int handleShipSend(char data[], int seek);
};