/*
Instance that represents a spaceport

Spaceports are attached to planets which are attached to stars

Only spaceports can produce new cargo instances
*/

#pragma once

#include <string>
#include <map>
#include <mutex>

#include "server/src/globals.hpp"

using namespace std;

// Forward declare these classes to make the compiler happy...
class Star;
class Planet;
class Ship;
class Cargo;
struct Producer;
struct Consumer;
struct cargo_type;


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

    int station_level;                  // station level of development

    Address address;                    // station location in space

    Star *star;                         // pointer to star this spaceport is around
    Planet *planet;                     // pointer to planet this spaceport is around

    map <int, Ship*> docked_ships;      // map of all ships currently docked at this station

    map <int, Producer*> my_producers;  // map of all of the station's cargo producers
    map <int, Consumer*> my_consumers;  // map of all of the station's cargo consumers
    map <int, Cargo*> my_cargo;         // map containing the station's cargo bulletin
    int target_cargo;                   // target number of cargo instances to keep in bulletin
    time_t next_cargo_update;           // timestamp of next update to cargo bulletin
    int cargo_update_frequency;         // how often to update cargo bulletin (in seconds)

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

    // Get spaceport id and cargo manifest json string
    string getCargoAndIdJson();

    // Return the distance between this spaceport and the provided one (interstellar or intrastellar)
    float getDistance(Spaceport *b);

    // Return the next cargo manifest update time for this spaceport
    time_t getCargoUpdateTime();

    // Create a new producer at this spaceport for an existing cargo_type
    int addProducer(cargo_type *product, int _max, int _min, int dest_pref, int dest_pref_weight, float _weight);

    // Create a new consumer at this spaceport for an existing cargo_type
    int addConsumer(cargo_type *product, int demand);

    // Produce new cargo, return number of new cargo created, force = true remakes entire bulletin and deletes old cargo
    int updateCargoManifest(bool force);

    // Remove cargo with id cargo_id from this spaceport's manifest, return NULL for error
    Cargo* removeCargo(int cargo_id);
};