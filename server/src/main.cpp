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
int MAX_CLIENTS = 10;                // maximum number of client connections

// Global Variable Declarations
map <int, Ship*> ships;                // map of all ships (id, pointer)
map <int, Spaceport*> spaceports;      // map of all spaceports (id, pointer)
map <int, Connection*> connections;         // map of all active TCP connections


// Periodically check if ships have arrived at their destination
void checkShipProgress(std::map <int, Ship*> *ships) {

    map<int, Ship*>::iterator it;
    time_t current_time;

    while (true) {

        // Get the current UNIX timestamp
        current_time = time(NULL);

        // Loop through all (currently warping) ships and check if their arrival time has passed, call arrive() if so
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
    Logger::debug_level = 4;

    // Initialize Spaceports
    Address new_address_1 = {"Sol", 1, 100, "some_string", 0, 0};
    Spaceport *port_1 = new Spaceport(1, "Terra Station", 1, new_address_1);

    Address new_address_2 = {"Alpha Centauri", 2, 100, "some_string", 0.2, 0.2};
    Spaceport *port_2 = new Spaceport(2, "Alpha Centauri II Station", 1, new_address_2);


    // Initialize ships
    Ship *ship_1 = new Ship("Enterprise", "NCC-1701", SMALL, 16, 16, port_1);
    //ships.insert({ship_1->getId(), ship_1});

    Ship *ship_2 = new Ship("Kelvin", "NCC-0514", MEDIUM, 16, 16, port_2);
    //ships.insert({ship_2->getId(), ship_2});


    // Launch thread to check ship progress
    thread t1(checkShipProgress, &ships);

    ship_1->depart(port_2);

    this_thread::sleep_for(chrono::seconds(1));
    ship_2->depart(port_1);


    // Set up TCP listener
    Listener *master = new Listener(0, "127.0.0.1", 4296);
    if (master->startListener() > 0) {
        Logger::log_message(0, master->last_error, 0, Logger::RED);
    }

    t1.join();

}