/*
Instance that represents a planet.

Planets are attached to stars and may contain a spaceport
*/

#pragma once

#include <string>
#include <mutex>

#include "star.hpp"

using namespace std;


enum planet_type{};

class Planet {
public:
    static int id_counter;              // value of next planet id to be issued

private:
    int id;                             // unique planet id
    int star_id;                        // id of star this planet is orbiting
    Star *star;
    int spaceport_id;                   // id of spaceport at this planet, 0 for no station
    string name;                        // planet name
    float radius;                       // radius of planet

    float orbit_radius;                 // radius of orbit from star
    float orbit_degree;                 // degree along orbit (0-360)
    float orbit_speed;                  // Speed of Orbit

    mutex mtx;

public:
    // Constructor
    Planet(string _name, int _star_id, float orb_rad, float orb_deg);

    // Get planet id
    int getId();

    // Add spaceport to planet, return pointer to new spaceport
    Spaceport* addSpaceport(string s_name, int _class);

    // Get address of planet (to be set as spaceport address)
    //int getAddress();

    //
    float getPlanetOrbitSpeed();

    // Update the Planet orbits
    void updatePlanetOrbits();

    // Get all planet information at a json string
    string getJsonString();



};