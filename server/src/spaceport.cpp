#include <time.h>
#include <string>
#include <map>

#include "globals.hpp"

#include "server/src/logger.hpp"
#include "server/lib/nlohmann/json.hpp"
#include "spaceport.hpp"

using json = nlohmann::json;

int Spaceport::id_counter = 1;

Spaceport::Spaceport(string _name, int _class, Address _address, Star *mystar, Planet *myplanet) {

    // Set unique id and iterate id counter
    id = id_counter;
    id_counter += 1;

    // Set initial variables
    //id = _id;
    name = _name;
    station_class = _class;
    address = _address;
    star = mystar;
    planet = myplanet;

    // Add spaceport to map of all spaceports
    spaceports.insert({id, this});

    // Log spaceport creation
    Logger::log_message("New spaceport created: " + name, 0, Logger::GREEN);
}


int Spaceport::getId() {
    return id;
}


Address Spaceport::getAddress() {
    return address;
}

void Spaceport::setOrbit(float new_orbit_degree) {
    mtx.lock();
    address.orb_degrees = new_orbit_degree;
    mtx.unlock();
}


string Spaceport::getName() {
    return name;
}


int Spaceport::addProducer(cargo_type *product, int _max, int _min, int dest_pref, int dest_pref_weight, float _weight) {

    // Allocate a new producer and add it to its cargo map and this station's map
    Producer *x = new Producer();

    x->id = Cargo::producer_id_counter;
    Cargo::producer_id_counter++;
    x->spaceport_id = id;
    x->product = product;
    x->max_manifest = _max;
    x->min_manifest = _min;
    x->dest_pref = dest_pref;
    x->dest_pref_weight = dest_pref_weight;
    x->weight = _weight;

    Cargo::producers.insert({x->id, x});
    my_producers.insert({x->id, x});

    Logger::log_message("New producer for " + x->product->name + " created at " + name, 1, Logger::GREEN);

    return 1;
}


Cargo* Spaceport::removeCargo(int cargo_id) {

    mtx.lock();

    // Make sure cargo_id is in station's manifest
    if (my_cargo.find(cargo_id) == my_cargo.end()) {
        mtx.unlock();
        return NULL;
    }

    Cargo *this_cargo = my_cargo[cargo_id];

    // Remove cargo from station's manifest
    my_cargo.erase(cargo_id);

    // Update all clients on this station's new cargo bulletin
    json x;

    vector <int> my_cargo_ids;
    for (auto const& element : my_cargo) {
        my_cargo_ids.push_back(element.first);
    }

    x["cargo"] = my_cargo_ids;
    x["id"] = id;

    mtx.unlock();

    Connection::syncInstance(0, "SYNC_STATION", "CARGO", x.dump() );

    return this_cargo;
}


string Spaceport::getJsonString() {

    json x;

    mtx.lock();

    x["id"] = id;
    x["name"] = name;
    x["station_class"] = station_class;
    //x["address"]["star_name"] = address.star_name;
    x["address"]["star_id"] = address.star_id;
    x["address"]["planet_id"] = address.planet_id;
    //x["address"]["orb_radius"] = address.orb_radius;
    //x["address"]["orb_degrees"] = address.orb_degrees;
    x["address"]["star_x"] = address.star_x;
    x["address"]["star_y"] = address.star_y;

    // cargo bulletin
    vector <int> my_cargo_ids;
    for (auto const& element : my_cargo) {
        my_cargo_ids.push_back(element.first);
    }
    x["cargo"] = my_cargo_ids;

    mtx.unlock();

    return x.dump();
}



float Spaceport::getDistance(Spaceport *port_b) {
    Address a = address;
    Address b = port_b->getAddress();

    float a_pos_x, a_pos_y, b_pos_x, b_pos_y;

    float pi = 3.14159;         // an approximation of pi
    float to_rad = pi / 180;    // multiply a degree by to_rad to get radians

    // Calculate interstellar distance if a and b do not orbit the same star
    if (a.star_id != b.star_id) {
        a_pos_x = a.star_x;
        a_pos_y = a.star_y;
        b_pos_x = b.star_x;
        b_pos_y = b.star_y;
    }

    // Calculate intrasystem distance if the spaceports orbit the same star
    else {
        a_pos_x = a.orb_radius * cos(a.orb_degrees * to_rad);
        a_pos_y = a.orb_radius * sin(a.orb_degrees * to_rad);
        b_pos_x = b.orb_radius * cos(b.orb_degrees * to_rad);
        b_pos_y = b.orb_radius * sin(b.orb_degrees * to_rad);
    }

    // Apply and return the distance formula
    double z1 = pow(b_pos_x - a_pos_x, 2);
    double z2 = pow(b_pos_y - a_pos_y, 2);

    return (float) sqrt(z1 + z2 * 1.0);
}