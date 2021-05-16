/*
Static object that prints out well formatted debug messages to the console and/or
writes them to a logfile.

Prepends a timestamp to the message as well as provides message coloring.
Uses a debug_lvl value to control the amount of messages displayed
(higher debug level displays more specific messages)
*/

#pragma once


#include <string>


using namespace std;


// ANSI escape codes for colorful text output
#define NO_COL  ""
#define COL_END "\033[0m"



class Logger {
public:
    static int debug_level;             // Level of messages to show, higher level, more debug messages (0-4)

    static string RED;                  // ANSI escape code for red
    static string YELLOW;               // ANSI escape code for yellow
    static string CYAN;
    static string GREEN;


    /*
    Print a debug message to the console.

    debug_lvl specifies the debug level needed to display this message
    color specifies a color for the debug message, timestamp excluded
    */
    static void log_message(string message, int debug_lvl, string color);
};