/*
Instance that represents a star system.

Star systems contain planets and stations.
*/

#pragma once

#include <string>
#include <map>
#include <mutex>

#include "server/src/globals.hpp"

using namespace std;


enum star_type {red, yellow, blue, white_dwarf, neutron};

class Star {
public:
    static int id_counter;          // value of next star id to be issued

private:
    int id;                         // unique star id
    string name;                    // star name
    float radius;                   // radius of star

    float x;                        // x starmap coordinate
    float y;                        // y starmap coordinate

    star_type type;                 // star classification (color/properties)

    map <int, Planet*> myplanets;   // map of all planets orbiting this star

    mutex mtx;



public:
    // Constructor
    Star(string _name, star_type _type, float _x, float _y);

    // Get star id
    int getId();

    // Get star x coord
    float getX();

    // Get star y coord
    float getY();

    // Add planet to star, return pointer to the newly created planet
    Planet* addPlanet(string _name, float _orb_rad, float _orb_deg);

    // Add spaceport to star
    //int addSpaceport();

    // Get all star information as a json string
    string getJsonString();

};