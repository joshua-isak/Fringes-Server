#include <map>
#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>

#include "ship.hpp"
#include "spaceport.hpp"

using namespace std;

int PROGRESS_INTERVAL = 1;           // seconds to wait between checking the warp progress of all ships

map <int, Ship*> ships;              // map of all ships (id, pointer)
map <int, Spaceport*> spaceports;    // map of all spaceports (id, pointer)



// Periodically check if ships have arrived at their destination
void checkShipProgress(map <int, Ship*> *ships) {

    map<int, Ship*>::iterator it;
    time_t current_time;

    while (true) {

        // Get the current UNIX timestamp
        current_time = time(NULL);

        // Loop through all (currently warping) ships and check if their arrival time has passed, called arrive() if so
        for(it = ships->begin(); it != ships->end(); it++) {

            // Look at the next ship if this one is not currently warping
            if (it->second->getState() != WARP) { continue; }

            time_t arrival_time = it->second->getArrivalTime();

            if (current_time >= arrival_time) {
                it->second->arrive();
            }
        }

        // Wait PROGRESS_INTERVAL seconds until next check
        this_thread::sleep_for(chrono::seconds(PROGRESS_INTERVAL));
    }
}



int main(int argc, char *argv[]) {

    cout << "Initalizing Server..." << endl;

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

    //string ship_info = ship_1->getJsonString();
    //string station_info = port_1->getJsonString();
    // cout << station_info << endl;
    // cout << ship_info << endl;


    // Launch thread to check ship progress
    thread t1(checkShipProgress, &ships);

    ship_1->depart(port_2);

    this_thread::sleep_for(chrono::seconds(14));
    ship_2->depart(port_1);

    t1.join();

}