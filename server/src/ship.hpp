#pragma once

#include <time.h>
#include <string>
#include <map>
#include <mutex>

#include "server/lib/nlohmann/json.hpp"
#include "spaceport.hpp"

using json = nlohmann::json;

using namespace std;


enum ship_state {DEPARTING, ARRIVING, WARP, DOCKED};
enum ship_type {SMALL, MEDIUM, LARGE};



class Ship {
public:
    static int id_counter;              // value of next ship id to be issued
    string last_error;

private:

    int id;                             // unique ship id
    string name;                        // ship name
    string registration;                // unique registration number
    ship_type type;                     // ship class

    int max_weight;                     // maximum total cargo weight
    int max_volume;                     // maximum total cargo volume

    int total_warps;                    // number of times ship has entered warp

    float reliability;                  // ship failure rate

    //map <int, Cargo*> cargo;            // map containing ship's current cargo manifest

    Spaceport *last_spaceport;          // spaceport ship has departed from
    Spaceport *next_spaceport;          // spaceport ship is travelling to

    time_t departure_time;              // departure time from last spaceport
    time_t arrival_time;                // arrival time to next spaceport

    ship_state travel_state;            // current ship travelling state

    Spaceport *current_spaceport;       // currently docked spaceport

    mutex mtx;                          // mutex to lock critical sections of class functions

public:
    // Constructor
    Ship(string _name, string _reg, ship_type _type, int m_w, int m_v, Spaceport *startport);

    // Send the ship off to the destination spaceport
    int depart(Spaceport *destination);

    // Mark the arrival of the ship to its destination
    void arrive();

    // Add a cargo item to the ship
    int addCargo();

    // Remove a cargo item from the ship
    int removeCargo();

    // Rename the ship
    int rename(string new_name);

    // Get ship arrival time as a UNIX timestamp
    time_t getArrivalTime();

    // Get ship name
    string getName();

    // Get ship id
    int getId();

    // Get ship travel state
    ship_state getState();

    // Return all ship information as a json string
    string getJsonString();
};