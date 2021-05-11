#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

#include "connection.hpp"

using namespace std;

Connection::Connection(int _socket, const char * _ip, int _port) {
    socket_id = _socket;
    ip = _ip;
    port = _port;
}


void connectionThread(Connection *master, int socket_id, struct sockaddr_in address) {

    cout << "New connection from IP: " << inet_ntoa(address.sin_addr) << " Port: " << address.sin_port << endl;

    while (true) {
        int BUFSIZE = 128;
        char buffer[BUFSIZE] = {0};
        recv(socket_id, buffer, BUFSIZE - 1, 0);
        cout << buffer;
    }
}


int Connection::startListener() {

    cout << "Starting listener..." << endl;

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        last_error = "listener: failed to create socket";
        return 1;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        last_error = "listener: failed to setsockopt";
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        last_error = "listener: socket bind failed";
        return 1;
    }

    // Listen for new TCP connections
    if (listen(server_fd, 3) < 0) {
        last_error = "listener: listen failed";
        return 1;
    }

    cout << "Listening for new connections on port " << port << endl;

    while (true) {

        // Accept one new TCP connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            last_error = "listener: accept failed";
            return 1;
        }

        // Start a new thread to handle the new connection
        thread t(connectionThread, this, new_socket, address);
        t.detach();
    }


    valread = read(new_socket, buffer, 1024);
    cout << buffer << endl;
    //printf("%s\n",buffer );
    //send(new_socket , hello , strlen(hello) , 0 );
    //printf("Hello message sent\n");

    cout << "Listener stopped..." << endl;

    return 0;
}

