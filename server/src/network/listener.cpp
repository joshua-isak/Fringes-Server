#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <string>

#include "server/src/globals.hpp"

#include "listener.hpp"
#include "connection.hpp"
#include "server/src/logger.hpp"


Listener::Listener(const char * _ip, int _port) {
    ip = _ip;
    port = _port;
}


int Listener::startListener() {

    Logger::log_message("Starting listener...", 0, "");

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        last_error = "listener: failed to create socket";
        return -1;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        last_error = "listener: failed to setsockopt";
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        last_error = "listener: socket bind failed";
        return -1;
    }

    // Listen for new TCP connections
    if (listen(server_fd, BACKLOG) < 0) {
        last_error = "listener: listen failed";
        return -1;
    }

    Logger::log_message("Listening for new connections on port " + to_string(port) + "...", 0, "");

    while (true) {

        // Accept one new TCP connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            last_error = "listener: accept failed";
            return -1;
        }

        // Start a new thread to handle the new connection
        thread t(Connection(), new_socket, address);
        t.detach();
    }

    Logger::log_message("Listener stopped.", 0, "");

    return 0;
}

