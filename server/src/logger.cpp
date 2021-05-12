#include <time.h>
#include <iostream>

#include "logger.hpp"


int Logger::debug_level = 0;

string Logger::RED = "\033[91m";
string Logger::YELLOW = "\033[93m";


void Logger::log_message(int thread_id, string message, int debug_lvl, string color) {

    // Check if this message should be displayed
    if (debug_level < debug_lvl) { return; }

    // text to output
    string output;

    // Find and add timestamp to output
    time_t system_time = time(NULL);
    tm* now = localtime(&system_time);
    char timestamp[12] = {0};
    strftime(timestamp, 12, "[%H:%M:%S] ", now);
    output += timestamp;


    // Add color to output message and concatenate
    output = output + color + message;
    if (color != NO_COL) { output += COL_END; }         // add end escape code if neccessary

    // Output to stdout
    cout << output << endl;
}