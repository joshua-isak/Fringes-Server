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
    username = "";

    readable_ip = inet_ntoa(address.sin_addr);
    readable_ip += ":" + to_string(address.sin_port);

    Logger::log_message("New connection from " + readable_ip, 1, Logger::YELLOW);

    socket_id = _socket_id;
    addr_port = address.sin_port;

    // Conduct connection handshake (read in client HELLO and send server WELCOME)
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
    if (username != "") {
        Logger::log_message(username + " has disconnected", 0, Logger::YELLOW);
    }

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


int Connection::sendFrame(char data[], int data_size) {

    char outbuf [data_size + 2] = {0};
    uint16_t frame_len = data_size;

    frame_len = be16toh(frame_len);     // translate frame length into network byte order

    memcpy(outbuf, &frame_len, 2);
    memcpy(outbuf + 2, data, data_size);

    return this->send(outbuf, sizeof(outbuf));

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
    uint16_t f_length;
    if (recv(socket_id, &f_length, 2, 0) < 1) {
        last_error = "connection: handshake failed (1)";
        return -1;
    }

    // Translate frame length from network byte order
    f_length = be16toh(f_length);

    // Read in rest of frame
    char data[f_length];
    if (recv(socket_id, &data, f_length, 0) < 1) {
        last_error = "connection: handshake failed (2)";
        return -1;
    }

    // Read in command string
    int seek = 0;                                   // seek value of data buffer
    int c_length = data[seek];                      // command string length
    string command (&data[seek+1], c_length);       // command string

    // Read in version string
    seek += c_length + 1;
    int v_length = data[seek];                      // version string length
    string version (&data[seek+1], v_length);       // version string

    // Read in username string
    seek += v_length + 1;
    int u_length = data[seek];                      // _username string length
    string _username (&data[seek+1], u_length);     // _username string

    // Read in password string
    seek += u_length + 1;
    int p_length = data[seek];                      // _password string length
    string _password (&data[seek+1], p_length);     // _password string


    // Check if client sent the HELLO command
    if (command != "HELLO") { last_error = "connection: malformed client HELLO"; return -1; }

    // Make sure client version is compatible
    if (version != version) { last_error = "connection: client version incompatible"; return -1; }

    // Verify password
    if (_password != password) { last_error = "connection: client password incorrect"; return -1; }


    // Send server WELCOME
    string w_command = "WELCOME";
    uint8_t w_c_len = 7;
    char w_outbuf[w_c_len + 1];

    memcpy(w_outbuf, &w_c_len, 1);                      // write command length to buffer
    strncpy(w_outbuf + 1, w_command.c_str(), 7);        // write command string to buffer

    if ( this->sendFrame(w_outbuf, sizeof(w_outbuf)) < 0) {
        last_error = "connection: handshake failed (3)";
        return -1;
    }


    // Complete connection
    connections.insert({socket_id, this});      // add to map of connections
    username = _username;                       // set connection's username

    // Log client connection
    Logger::log_message(username + " has connected", 0, Logger::YELLOW);

    return 0;   // return success
}