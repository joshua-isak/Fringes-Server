#include <map>
#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>

#include "server/src/globals.hpp"

//#include "ship.hpp"
//#include "spaceport.hpp"
#include "server/src/network/listener.hpp"



int PROGRESS_INTERVAL = 1;           // seconds to wait between checking the warp progress of all ships
int MAX_CLIENTS = 10;                // maximum number of client connections

// Global Variable Declarations
std::map <int, Ship*> ships;              // map of all ships (id, pointer)
std::map <int, Spaceport*> spaceports;    // map of all spaceports (id, pointer)



// Periodically check if ships have arrived at their destination
void checkShipProgress(std::map <int, Ship*> *ships) {

    std::map<int, Ship*>::iterator it;
    time_t current_time;

    while (true) {

        // Get the current UNIX timestamp
        current_time = time(NULL);

        // Loop through all (currently warping) ships and check if their arrival time has passed, call arrive() if so
        for(it = ships->begin(); it != ships->end(); it++) {

            Ship *this_ship = it->second;

            // Skip checking this ship if this one is not currently warping
            if (this_ship->getState() != WARP) { continue; }

            time_t arrival_time = this_ship->getArrivalTime();

            if (current_time >= arrival_time) {
                this_ship->arrive();
            }
        }

        // Wait PROGRESS_INTERVAL seconds until next check
        std::this_thread::sleep_for(std::chrono::seconds(PROGRESS_INTERVAL));
    }
}



int main(int argc, char *argv[]) {

    std::cout << "Initalizing Server..." << std::endl;

    // Initialize Spaceports
    Address new_address_1 = {"Sol", 1, 100, "some_string", 0, 0};
    Spaceport *port_1 = new Spaceport(1, "Terra Station", 1, new_address_1);

    Address new_address_2 = {"Alpha Centauri", 2, 100, "some_string", 1, 1};
    Spaceport *port_2 = new Spaceport(2, "Alpha Centauri II Station", 1, new_address_2);


    // Initialize ships
    int new_id = 1;
    Ship *ship_1 = new Ship(new_id, "Enterprise", "NCC-1701", SMALL, 16, 16, port_1);
    ships.insert({new_id, ship_1});

    new_id = 2;
    Ship *ship_2 = new Ship(new_id, "Kelvin", "NCC-0514", MEDIUM, 16, 16, port_2);
    ships.insert({new_id, ship_2});


    // Launch thread to check ship progress
    std::thread t1(checkShipProgress, &ships);

    ship_1->depart(port_2);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    ship_2->depart(port_1);


    // Set up TCP listener
    Listener *master = new Listener(0, "127.0.0.1", 4296);
    if (master->startListener() > 0) {
        std::cout << master->last_error << std::endl;
    }

    t1.join();

}