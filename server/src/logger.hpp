#pragma once


#include <string>


using namespace std;


// ANSI escape codes for colorful text output
#define NO_COL  ""
//#define RED     "\033[91m"
#define GREEN   "\033[92m"
//#define YELLOW  "\033[93m"
#define CYAN    "\033[96m"
#define COL_END "\033[0m"



class Logger {
public:
    static int debug_level;             // Level of messages to show, higher level, more debug messages (0-4)

    static string RED;                  // ANSI escape code for red
    static string YELLOW;               // ANSI escape code for yellow


    // Print a debug message to the console
    static void log_message(string message, int debug_lvl, string color);
};