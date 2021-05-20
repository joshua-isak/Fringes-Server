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
int planet_tick_speed = 4;

map <int, Ship*> ships;                // map of all ships (id, pointer)
map <int, Spaceport*> spaceports;      // map of all spaceports (id, pointer)
map <int, Connection*> connections;    // map of all active TCP connections
map <int, Company*> companies;         // map of all companies (id, pointer)
map <int, Planet*> planets;          // map of all planets
map <int, Star*> stars;              // map of all stars


// Update the simulation every second
void update() {

    map<int, Ship*>::iterator it;
    map<int, Planet*>::iterator ip;
    time_t current_time;
    time_t next_update_time = time(NULL);

    while (true) {

        // Get the current UNIX timestamp
        current_time = time(NULL);

        // Update the orbit degree of all planets every minute
        if (current_time == next_update_time) {
            Logger::log_message("Updated all planet orbits", 1, Logger::CYAN);
            for(ip = planets.begin(); ip != planets.end(); ip++) {
                Planet *this_planet = ip->second;
                this_planet->updatePlanetOrbits();
            }
            next_update_time += 60;
        }

        // Loop through all warping ships and check if their arrival time has passed, call arrive() if so
        for (it = ships.begin(); it != ships.end(); it++) {

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

    // Initialize Stars, planets and spaceports
    Star *star1 = new Star("Sol", yellow, 0, 0);
    Star *star2 = new Star("Alpha Centauri", yellow, 2, 2);
    Star *star3 = new Star("Durandal", yellow, -0.8, -2.5);

    Planet *mercury = star1->addPlanet("Mercury", 1, 200);
    Planet *venus = star1->addPlanet("Venus", 1.6, 80);
    Planet *planet1 = star1->addPlanet("Earth", 2, 310);
    Planet *planet2 = star1->addPlanet("Mars", 2.4, 30);

    Planet *planet3 = star2->addPlanet("Alpha Centauri I", 1, 30);
    Planet *planet4 = star2->addPlanet("Alpha Centauri II", 1.5, 90);
    Planet *planet5 = star2->addPlanet("Alpha Centauri III", 3, 180);

    Planet *planet6 = star3->addPlanet("Durandal II", 3, 60);
    Planet *planet7 = star3->addPlanet("Reach", 1.5, 200);

    planet1->addSpaceport("Terra Station", 1);
    planet2->addSpaceport("Mars Station", 1);
    planet4->addSpaceport("Alpha Centauri II", 1);
    planet7->addSpaceport("Reach Station", 1);
    venus->addSpaceport("Venus Station", 1);


    // Launch thread to check ship progress
    thread t1(update);


    // Set up TCP listener
    Listener *master = new Listener("127.0.0.1", 4296);
    if (master->startListener() < 0) {
        Logger::log_message(master->last_error, 0, Logger::RED);
    }


    t1.join();
}