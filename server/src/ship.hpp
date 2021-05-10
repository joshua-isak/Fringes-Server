#pragma once

#include <time.h>
#include <string>
#include <map>

#include "spaceport.hpp"


using namespace std;


enum ship_state {DEPARTING, ARRIVING, WARP, DOCKED};
enum ship_type {SMALL, MEDIUM, LARGE};



class Ship {
private:
    int id;                                 // unique ship id
    string name;                            // ship name
    string registration;                    // unique registration number (static)
    ship_type type;                         // ship class

    int max_weight;                         // maximum total cargo weight
    int max_volume;                         // maximum total cargo volume

    int total_warps;                        // number of times ship has entered warp

    float reliability;                      // ship failure rate

    //map <int, Cargo*> cargo;                // map containing ship's current cargo manifest

    Spaceport *last_spaceport;              // spaceport ship has departed from
    Spaceport *next_spaceport;              // spaceport ship is travelling to

    time_t departure_time;                  // departure time from last spaceport
    time_t arrival_time;                    // arrival time to next spaceport

    ship_state travel_state;                // current ship travelling state

    Spaceport *docked_spaceport;            // currently docked spaceport

public:
    // Constructor
    Ship(int _id, string _name, string _reg, ship_type _type, int m_w, int m_v, Spaceport *startport);

    // Send the ship off to the destination spaceport
    int depart(Spaceport *destination);

    // Add a cargo item to the ship
    int addCargo();

    // Remove a cargo item from the ship
    int removeCargo();

    // Rename the ship
    int rename(string new_name);

    // Get ship id
    int getId();

    // Get all ship information as a json string
    string getJsonString();
};