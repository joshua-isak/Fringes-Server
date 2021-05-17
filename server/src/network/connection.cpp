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
    socket_id = _socket_id;
    addr_port = address.sin_port;

    readable_ip = inet_ntoa(address.sin_addr);
    readable_ip += ":" + to_string(address.sin_port);

    Logger::log_message("New connection from " + readable_ip, 1, Logger::YELLOW);

    // Conduct connection handshake (read in client HELLO and send server WELCOME)
    if (this->doHandshake() < 0) {
        Logger::log_message(last_error, 1, Logger::RED);  // close connection if there was an error
        this->close();
        return;
    }

    // Sync data for all spaceports
    map<int, Spaceport*>::iterator ix;
    for (ix = spaceports.begin(); ix != spaceports.end(); ix++) {
        Spaceport *this_spaceport = ix->second;
        Connection::syncInstance(socket_id, "SYNC_STATION", "INITIAL", this_spaceport->getJsonString());
    }

    // Sync data for all ships
    map<int, Ship*>::iterator iy;
    for (iy = ships.begin(); iy != ships.end(); iy++) {
        Ship *this_ship = iy->second;
        Connection::syncInstance(socket_id, "SYNC_SHIP", "INITIAL", this_ship->getJsonString());
    }

    // Check if we need to generate a new company for this user
    if (Company::u_to_company.find(username) == Company::u_to_company.end()) {
        // Generate new company for this user
        Company *new_company = new Company("Unnamed_company", username);
        company_id = new_company->getId();
    }
    else {
        // Set this connection's company_id to an existing one
        company_id = Company::u_to_company[username]->getId();
    }

    // Sync data for all companies
    map<int, Company*>::iterator iz;
    for (iz = companies.begin(); iz != companies.end(); iz++) {
        Company *this_company = iz->second;
        Connection::syncInstance(socket_id, "SYNC_COMPANY", "INITIAL", this_company->getJsonString());
    }

    while (true) {

        // Read in length of frame
        uint16_t f_length;
        if (recv(socket_id, &f_length, 2, 0) < 1) {
            Logger::log_message("connection: failed to read from socket", 1, Logger::RED);
            this->close();
            return;
        }

        // Read in rest of frame
        char data[f_length];
        recv(socket_id, &data, f_length, 0);        //--TODO--// MAKE A READBYTES FUNCTION!!!!

        // Read in command string           //--TODO--// umm what if c_length > buffer size... these need mem protection!
        int seek = 0;                                   // seek value of data buffer
        int c_length = data[seek];                      // command string length
        string command (&data[seek+1], c_length-1);     // command string (-1 to trim off extra whitespace)
        seek += c_length + 1;

        // Handle client command
        // Switch statements only work with integral types :(
        if (command == "SEND_SHIP") { this->handleShipSend(data, seek); }

    }
}


void Connection::close() {

    // Close the file descriptor for this socket and remove from map of connections
    ::close(socket_id);
    connections.erase(socket_id);

    // Log disconnection
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

    Logger::log_message("sent packet to " + this->username, 4, Logger::CYAN);

    return err;
}


int Connection::sendFrame(char data[], int data_size) {

    char outbuf [data_size + 2] = {0};
    uint16_t frame_len = data_size;

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


int Connection::sendFrameAll(char data[], int data_size) {

    map <int, Connection*>::iterator it;
    int error;

    // Iterate through all connections and call sendFrame()
    for (it = connections.begin(); it != connections.end(); it++) {

        Connection *this_conn = it->second;

        error = this_conn->sendFrame(data, data_size);

        if (error == -1) { return -1; }
    }

    return error;
}


int Connection::doHandshake() {

    // Read in length of frame
    uint16_t f_length;
    if (recv(socket_id, &f_length, 2, 0) < 1) {
        last_error = "connection: handshake failed (1)";
        return -1;
    }

    // Read in rest of frame
    char data[f_length];
    if (recv(socket_id, &data, f_length, 0) < 1) {
        last_error = "connection: handshake failed (2)";
        return -1;
    }

    // Read in command string   //--TODO-- umm what if c_length > buffer size... these need mem protection!
    int seek = 0;                                   // seek value of data buffer
    int c_length = data[seek];                      // command string length
    string command (&data[seek+1], c_length-1);     // command string (-1 to trim off extra whitespace)

    // Read in version string
    seek += c_length + 1;
    int v_length = data[seek];                      // version string length
    string version (&data[seek+1], v_length-1);     // version string

    // Read in username string
    seek += v_length + 1;
    int u_length = data[seek];                      // _username string length
    string _username (&data[seek+1], u_length-1);   // _username string

    // Read in password string
    seek += u_length + 1;
    int p_length = data[seek];                      // _password string length
    string _password (&data[seek+1], p_length-1);   // _password string

    // Check if client sent the HELLO command
    string hello = "HELLO";
    if (command != "HELLO") {
        last_error = "connection: malformed client HELLO";
        this->sendError("Malformed HELLO message");
        return -1;
    }

    // Make sure client version is compatible
    if (version != version) {
        last_error = "connection: client version incompatible";
        this->sendError("Client version incompatible");
        return -1;
    }

    // Verify password
    if (_password != password) {
        last_error = "connection: client password incorrect";
        this->sendError("Server password incorrect");
        return -1;
    }

    // Make sure a client with this username is not already connected
     map<int, Connection*>::iterator it;
    for (it = connections.begin(); it != connections.end(); it++) {
        Connection *conn = it->second;
        if (username == conn->username ) {
            last_error = "connection: client with this username already connected!";
            this->sendError("User with your name already connected!");
            return -1;
        }
    }

    // Send WELCOME to client
    string w_command = "WELCOME";
    uint8_t w_c_len = 7 + 1;                // +1 to include string null terminator
    char w_outbuf[w_c_len + 1];

    memcpy(w_outbuf, &w_c_len, 1);                      // write command length to buffer
    strcpy(w_outbuf + 1, w_command.c_str());            // write command string to buffer

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


int Connection::syncInstance(int conn_id, string sync_command, string sync_type, string json_data) {

    string command = sync_command;

    uint8_t command_len = command.length() + 1;             // +1 to include string null terminator
    uint8_t sync_type_len = sync_type.length() + 1;
    uint16_t json_len = json_data.length() + 1;

    char outbuf [command_len + sync_type_len + json_len + 4];   // buffer of packet data
    int seek = 0;                                               // current buffer seek

    // Write command data to buffer
    Connection::writeCommand(outbuf, command, &seek);

    // Write sync_type data to buffer (sub command)
    Connection::writeCommand(outbuf, sync_type, &seek);

    // Write json_data to buffer
    memcpy(outbuf + seek, &json_len, 2);                        // 2 because uint16's are 2 bytes long
    seek += 2;
    strcpy(outbuf + seek, json_data.c_str());//, json_len);

    // Send this to all clients if conn_id is zero
    if (conn_id == 0) {
        return sendFrameAll(outbuf, sizeof(outbuf));
    }

    // Send it to connection conn_id if specified
    else {
        return connections[conn_id]->sendFrame(outbuf, sizeof(outbuf));
    }
}


int Connection::handleShipSend(char data[], int seek) {

    // Read in ship id
    int ship_id = data[seek];
    seek += 2;

    // Read in destination station id
    int dest_id = data[seek];

    // Check if the shipID is valid
    if (ships.find(ship_id) == ships.end()) {
        Logger::log_message("connection: send_ship: client send bad ship_id", 0, Logger::RED);
        this->sendError("ship id invalid!");
        return -1;
    }

    // Check if the spaceportID is valid
    if (spaceports.find(dest_id) == spaceports.end()) {
        Logger::log_message("connection: send_ship: client send bad spaceport_id", 0, Logger::RED);
        this->sendError("spaceport id invalid!");
        return -1;
    }

    // Check if this ship belongs to this user's company
    Ship *this_ship = ships[ship_id];
    if (this_ship->getCompanyId() != company_id) {
        Logger::log_message("connection: send_ship: client tried to send a ship that isn't theirs!", 0, Logger::RED);
        this->sendError("this ship does not belong to your company!");
        return -1;
    }

    // Attempt to send the ship to the destination spaceport, print any errors
    Spaceport *that_spaceport = spaceports[dest_id];
    if (this_ship->depart(that_spaceport) < 0) {
        Logger::log_message("connection: send_ship: " + this_ship->last_error, 0, Logger::RED);
        this->sendError(this_ship->last_error);
        return -1;
    }

    return 0;   // return success
}


int Connection::sendError(string error_message) {

    string command = "SEND_ERROR";

    uint8_t command_len = command.length() + 1;             // +1 to include string null terminator
    uint8_t error_len = error_message.length() + 1;         // +1 for null terminator, cannot exceed 255 bytes

    // Make sure the error message is not too long
    if (error_len > 250) { return -1; }

    char outbuf [command_len + error_len + 1 + 1];          // +1+1 for 2 uint8
    int seek = 0;

    // Write command data to buffer
    this->writeCommand(outbuf, command, &seek);

    // Write error_message to buffer
    memcpy(outbuf + seek, &error_len, 1);                   // write in error_len
    seek += 1;
    strcpy(outbuf + seek, error_message.c_str());           // write in error_message

    // Send the packet to the client
    return this->sendFrame(outbuf, sizeof(outbuf));
}


void Connection::writeCommand(char buffer[], string command, int *seek) {
    //--TODO--// ADD CHECKING FOR BUFFER OVERFLOW
    uint8_t command_len = command.length() + 1;             // +1 to include string null terminator

    // Write command data to buffer
    memcpy(buffer, &command_len, 1);                        // 1 because uint8's are 1 byte long
    *seek += 1;
    strcpy(buffer + *seek, command.c_str());                 // write command string
    *seek += command_len;
}