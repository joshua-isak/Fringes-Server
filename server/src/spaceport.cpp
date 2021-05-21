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