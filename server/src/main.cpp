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
string password = "1234";              // Naive password management approach //--TODO--// NO PLAINTEXT PWDS

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
    Logger::debug_level = 4;

    // Initialize Spaceports
    Address new_address_1 = {"Sol", 1, 100, "some_string", 0, 0};
    Spaceport *port_1 = new Spaceport(1, "Terra Station", 1, new_address_1);

    Address new_address_2 = {"Alpha Centauri", 2, 100, "some_string", 1, 1};
    Spaceport *port_2 = new Spaceport(2, "Alpha Centauri II", 1, new_address_2);

    Address new_address_3 = {"Polaris", 2, 100, "some_string", 2.5, -0.4};
    Spaceport *port_3 = new Spaceport(3, "Polaris V", 1, new_address_3);

    Address new_address_4 = {"Proxima Centauri", 2, 100, "some_string", 2, -2};
    Spaceport *port_4 = new Spaceport(4, "Proxima Centauri I", 1, new_address_4);

    Address new_address_5 = {"Durandal", 2, 100, "some_string", -1.8, -2};
    Spaceport *port_5 = new Spaceport(5, "Durandal III", 1, new_address_5);

/////////////////////////
    Address new_address_6 = {"Altair", 2, 100, "some_string", -2.8, -0.3};
    Spaceport *port_6 = new Spaceport(6, "Altair I", 1, new_address_6);

    Address new_address_7 = {"Deneb", 2, 100, "some_string", -1.3, -3};
    Spaceport *port_7 = new Spaceport(7, "Deneb IV", 1, new_address_7);

    Address new_address_8 = {"Vega", 2, 100, "some_string", -0.5, 2.5};
    Spaceport *port_8 = new Spaceport(8, "Vega Prime", 1, new_address_8);

    Address new_address_9 = {"Betelgeuse", 2, 100, "some_string", -3, -3.3};
    Spaceport *port_9 = new Spaceport(9, "Betelgeuse Station", 1, new_address_9);


    // Initialize ships
    Ship *ship_1 = new Ship("Enterprise", "NCC-1701", SMALL, 16, 16, port_1);
    Ship *ship_2 = new Ship("Kelvin", "NCC-0514", MEDIUM, 16, 16, port_2);
    Ship *ship_3 = new Ship("Autumn", "NCC-4196", MEDIUM, 16, 16, port_3);
    Ship *ship_4 = new Ship("Orville", "NCC-789", MEDIUM, 16, 16, port_4);
    Ship *ship_5 = new Ship("Serenity", "FF-055", MEDIUM, 16, 16, port_5);
    Ship *ship_6 = new Ship("Aquaria", "NMS-1444", MEDIUM, 16, 16, port_5);
    Ship *ship_7 = new Ship("Mantra", "SCS 1273", MEDIUM, 16, 16, port_5);
    Ship *ship_8 = new Ship("Pariah", "NMS-1821", MEDIUM, 16, 16, port_5);
    Ship *ship_9 = new Ship("Bohr", "NCC-3435", MEDIUM, 16, 16, port_5);



    // Launch thread to check ship progress
    thread t1(checkShipProgress, &ships);

    ship_1->depart(port_3);
    ship_2->depart(port_1);
    ship_3->depart(port_5);
    ship_4->depart(port_5);


    // Set up TCP listener
    Listener *master = new Listener("127.0.0.1", 4296);
    if (master->startListener() < 0) {
        Logger::log_message(master->last_error, 0, Logger::RED);
    }

    t1.join();

}