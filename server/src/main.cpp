#include <map>
#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>

#include "server/src/globals.hpp"

//#include "ship.hpp"
//#include "spaceport.hpp"
#include "server/src/network/listener.hpp"
#include "server/src/logger.hpp"



int PROGRESS_INTERVAL = 1;           // seconds to wait between checking the warp progress of all ships
int MAX_CLIENTS = 10;                // maximum number of client connections //--TODO--// Enforce this!

// Global Variable Declarations
string password = "1234";              // Naive password management approach //--TODO--// NO PLAINTEXT PWDS

map <int, Ship*> ships;                // map of all ships (id, pointer)
map <int, Spaceport*> spaceports;      // map of all spaceports (id, pointer)
map <int, Connection*> connections;    // map of all active TCP connections
map <int, Company*> companies;         // map of all companies (id, pointer)


// Periodically check if ships have arrived at their destination
void checkShipProgress(std::map <int, Ship*> *ships) {

    map<int, Ship*>::iterator it;
    time_t current_time;

    while (true) {

        // Get the current UNIX timestamp
        current_time = time(NULL);

        // Loop through all warping ships and check if their arrival time has passed, call arrive() if so
        for (it = ships->begin(); it != ships->end(); it++) {

            Ship *this_ship = it->second;

            // Skip checking this ship if this one is not currently warping
            if (this_ship->getState() != WARP) { continue; }

            time_t arrival_time = this_ship->getArrivalTime();

            if (current_time >= arrival_time) {
                this_ship->arrive();
            }
        }

        // Wait PROGRESS_INTERVAL seconds until next check
        this_thread::sleep_for(chrono::seconds(PROGRESS_INTERVAL));
    }
}



int main(int argc, char *argv[]) {

    cout << "Starting server..." << endl;

    // Set debug level for logger
    Logger::debug_level = 3;

    // Initialize Spaceports
    Address new_address_1 = {"Sol", 1, 100, "some_string", 0, 0};
    Spaceport *port_1 = new Spaceport("Terra Station", 1, new_address_1);

    Address new_address_2 = {"Alpha Centauri", 2, 100, "some_string", 1, 1};
    Spaceport *port_2 = new Spaceport("Alpha Centauri II", 1, new_address_2);

    Address new_address_3 = {"Polaris", 2, 100, "some_string", 2.5, -0.4};
    Spaceport *port_3 = new Spaceport("Polaris V", 1, new_address_3);

    Address new_address_4 = {"Proxima Centauri", 2, 100, "some_string", 2, -2};
    Spaceport *port_4 = new Spaceport("Proxima Centauri I", 1, new_address_4);

    Address new_address_5 = {"Durandal", 2, 100, "some_string", -1.8, -2};
    Spaceport *port_5 = new Spaceport("Durandal III", 1, new_address_5);

    Address new_address_6 = {"Altair", 2, 100, "some_string", -2.8, -0.3};
    Spaceport *port_6 = new Spaceport("Altair I", 1, new_address_6);

    Address new_address_7 = {"Deneb", 2, 100, "some_string", -1.3, -3};
    Spaceport *port_7 = new Spaceport("Deneb IV", 1, new_address_7);

    Address new_address_8 = {"Vega", 2, 100, "some_string", -0.5, 2.5};
    Spaceport *port_8 = new Spaceport("Vega Prime", 1, new_address_8);

    Address new_address_9 = {"Betelgeuse", 2, 100, "some_string", -3, 2};
    Spaceport *port_9 = new Spaceport("Betelgeuse Station", 1, new_address_9);

    Address new_address_10 = {"Sirius", 2, 100, "some_string", 3, 2.3};
    Spaceport *port_10 = new Spaceport("Sirius XM", 1, new_address_10);

    Address new_address_11 = {"Yildun", 2, 100, "some_string", -4, 0.7};
    Spaceport *port_11 = new Spaceport("Yildun VII", 1, new_address_11);

    Address new_address_12 = {"Mizar", 2, 100, "some_string", 1.3, 2.5};
    Spaceport *port_12 = new Spaceport("Mizar Battle-Station", 1, new_address_12);


    // Launch thread to check ship progress
    thread t1(checkShipProgress, &ships);


    // Set up TCP listener
    Listener *master = new Listener("127.0.0.1", 4296);
    if (master->startListener() < 0) {
        Logger::log_message(master->last_error, 0, Logger::RED);
    }


    t1.join();
}