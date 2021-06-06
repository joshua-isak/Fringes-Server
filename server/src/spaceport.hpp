/*
Instance that represents a spaceport

Spaceports are attached to planets which are attached to stars
*/

#pragma once

#include <string>
#include <map>
#include <mutex>

#include "server/src/globals.hpp"

// Forward declare these classes to make the compiler happy...
class Star;
class Planet;
class Ship;

using namespace std;


// Struct to contain station address information
struct Address {
    int star_id;
    int planet_id;

    float star_x;           // starmap x coord of station
    float star_y;           // starmap y coord of station

    float orb_radius;       // orbit radius of planet around star
    float orb_degrees;      // orbit degree
};


class Spaceport {
public:
    static int id_counter;              // value of next spaceport id to be issued

private:
    int id;                             // unique spaceport id
    string name;                        // spaceport name

    int station_class;                  // station level of development

    Address address;                    // station location in space

    Star *star;                         // pointer to star this spaceport is around
    Planet *planet;                     // pointer to planet this spaceport is around

    map <int, Ship*> docked_ships;      // map of all ships currently docked at this station
    //map <> cargo                      // map containing the station's cargo bulletin

    mutex mtx;

public:
    // Constructor
    Spaceport(string _name, int _class, Address _address, Star *mystar, Planet *myplanet);

    // Get spaceport id
    int getId();

    // Get spaceport address
    Address getAddress();

    // Set spaceport address.orbit_degree
    void setOrbit(float new_orbit_deg);

    // Get spaceport name
    string getName();

    // Get all spaceport information as a json string
    string getJsonString();

    // Return the distance between this spaceport and the provided one (interstellar or intrastellar)
    float getDistance(Spaceport *b);
};