#include <map>
#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>

#include "server/src/globals.hpp"

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
map <int, Planet*> planets;            // map of all planets
map <int, Star*> stars;                // map of all stars
map <int, Cargo*> cargos;              // map of all cargo instances


// Update the simulation every second
void update() {

    map<int, Ship*>::iterator it;           // ship iterator
    map<int, Planet*>::iterator ip;         // planet iterator
    map<int, Spaceport*>::iterator is;      // spaceport iterator
    time_t current_time;
    time_t next_update_time = time(NULL);   // next time to update planet orbits

    while (true) {

        // Get the current UNIX timestamp
        current_time = time(NULL);

        // Update the orbit degree of all planets every minute
        if (current_time >= next_update_time) {
            Logger::log_message("Updating all planet orbits", 4, Logger::CYAN);
            for(ip = planets.begin(); ip != planets.end(); ip++) {
                Planet *this_planet = ip->second;
                this_planet->updatePlanetOrbits();
            }
            next_update_time = current_time + 60;
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

        // Loop through all spaceports and check if it is time to update their cargo manifest, do if so
        for (is = spaceports.begin(); is != spaceports.end(); is++) {

            Spaceport *this_spaceport = is->second;

            // Skip this spaceport if it is not time to update its cargo manifest
            if (current_time <= this_spaceport->getCargoUpdateTime()) { continue; }

            // Update the cargo manifest
            int num_created = this_spaceport->updateCargoManifest(false);
        }

        // Wait PROGRESS_INTERVAL seconds until next check
        this_thread::sleep_for(chrono::seconds(PROGRESS_INTERVAL));
    }
}



int main(int argc, char *argv[]) {

    cout << "Starting server..." << endl;

    // Set debug level for logger
    Logger::debug_level = 3;

    // Initialize cargo products
    cargo_type *antimatter = Cargo::addProduct("Antimatter", "", 0.01, 25);
    cargo_type *passenger = Cargo::addProduct("Passenger", "", 0, 8);

    // Initialize Stars, planets and spaceports
    Star *star1 = new Star("Sol", yellow, 0, 0);
    Star *star2 = new Star("Alpha Centauri", yellow, 0.6, -2.1);
    Star *star3 = new Star("Polaris", yellow, -0.4, 2.4);
    Star *star4 = new Star("Proxima Centauri", yellow, 0.8, -1.3);
    Star *star5 = new Star("Durandal", yellow, 2.2, 2.9);
    Star *star6 = new Star("Altair", yellow, -3.1, 1.4);
    Star *star7 = new Star("Deneb", yellow, -2.8, -3);
    Star *star8 = new Star("Vega", yellow, -4.3, -1.1);
    Star *star9 = new Star("Betelgeuse", yellow, 3.5, 1.5);
    Star *star10 = new Star("Sirius", yellow, 1.9, 1.9);
    Star *star11 = new Star("Yildun", yellow, 3.8, -2.8);
    Star *star12 = new Star("Mizar", yellow, 2.8, -0.3);

    Planet *p1 = star1->addPlanet("Mercury", 1, 200);
    Planet *p2 = star1->addPlanet("Venus", 1.6, 80);
    Planet *p3 = star1->addPlanet("Earth", 2, 310);
    Planet *p4 = star1->addPlanet("Mars", 2.4, 30);
    Planet *p5 = star1->addPlanet("Jupiter", 3.7, 65);
    Planet *p6 = star1->addPlanet("Saturn", 4.9, 120);
    Planet *p7 = star1->addPlanet("Uranus", 5.4, 0);
    Planet *p8 = star1->addPlanet("Neptune", 6, 260);

    Spaceport *sp1 = p3->addSpaceport("Terra Station", 2);
    Spaceport *sp2 = p2->addSpaceport("Gaia Station", 1);
    Spaceport *sp3 = p4->addSpaceport("Ares Stellar Gate", 1);
    Spaceport *sp4 = p5->addSpaceport("Colony of Europa", 1);
    Spaceport *sp5 = p6->addSpaceport("Titan Forges", 1);
    Spaceport *sp6 = p8->addSpaceport("The Bastion", 1);

    sp1->addProducer(antimatter, 3, 0, 0, 0, 0.1);
    sp2->addConsumer(antimatter, 0);
    sp3->addConsumer(antimatter, 0);
    sp4->addConsumer(antimatter, 0);
    sp5->addConsumer(antimatter, 0);
    sp6->addConsumer(antimatter, 0);

    Planet *p9 = star2->addPlanet("Alpha Centauri I", 0.6, 20);
    Planet *p10 = star2->addPlanet("Haven", 1.9, 90);
    Planet *p11 = star2->addPlanet("Alpha Centauri III", 3.4, 350);

    p10->addSpaceport("Shelter Station", 1);

    Planet *p12 = star3->addPlanet("Polaris I", 0.7, 270);
    Planet *p13 = star3->addPlanet("Polaris II", 1.2, 255);
    Planet *p14 = star3->addPlanet("Ursa", 2.2, 135);

    p14->addSpaceport("Boreal Station", 1);

    Planet *p15 = star4->addPlanet("Proxima Centauri I", 1.1, 355);
    Planet *p16 = star4->addPlanet("Proxima Centauri II", 2.7, 40);

    p16->addSpaceport("Centauri Exchange", 1);

    Planet *p17 = star5->addPlanet("Reach", 2, 190);
    Planet *p18 = star5->addPlanet("Durandal II", 3.1, 15);

    p17->addSpaceport("Reach Colony", 1);
    p18->addSpaceport("Reach Forward Outpost", 1);

    Planet *p19 = star6->addPlanet("Altair I", 2.2, 55);
    Planet *p20 = star6->addPlanet("Altair II", 3.2, 145);

    p19->addSpaceport("Aquila Citadel", 1);

    Planet *p21 = star7->addPlanet("Deneb Prime", 2, 190);

    p21->addSpaceport("Crux Shipyards", 1);

    Planet *p22 = star8->addPlanet("Vega I", 0.5, 65);
    Planet *p23 = star8->addPlanet("Vega II", 1.2, 175);
    Planet *p24 = star8->addPlanet("Meridian", 2, 210);

    p23->addSpaceport("Meridian Depot", 1);
    p24->addSpaceport("Meridian Colony", 1);

    Planet *p25 = star9->addPlanet("Betelgeuse I", 0.5, 100);
    Planet *p26 = star9->addPlanet("Betelgeuse II", 3.5, 140);

    p26->addSpaceport("Blacksite-7", 1);

    Planet *p27 = star10->addPlanet("Sirius I", 1.3, 100);
    Planet *p28 = star10->addPlanet("Sirius II", 2.6, 305);

    p28->addSpaceport("Sahavanor Corp.", 1);

    Planet *p29 = star11->addPlanet("Yildun I", 1.5, 120);
    Planet *p30 = star11->addPlanet("Sierra", 2.1, 150);
    Planet *p31 = star11->addPlanet("Sirius III", 2.8, 260);

    p30->addSpaceport("Riviera Outpost", 1);

    Planet *p32 = star12->addPlanet("Mizar Prime", 1.9, 250);

    p32->addSpaceport("Antonia Station", 1);




    // Launch thread to check ship progress
    thread t1(update);


    // Set up TCP listener
    Listener *master = new Listener("127.0.0.1", 4296);
    if (master->startListener() < 0) {
        Logger::log_message(master->last_error, 0, Logger::RED);
    }


    t1.join();
}