
#include "server/src/globals.hpp"

#include "server/lib/nlohmann/json.hpp"
#include "server/src/logger.hpp"
#include "server/src/instances/planet.hpp"

using json = nlohmann::json;

int Planet::id_counter = 1;


Planet::Planet(string _name, int _star_id, float orb_rad, float orb_deg) {

    // Set unique id and iterate id counter
    id = id_counter;
    id_counter++;

    // Set initial variables
    name = _name;
    star_id = _star_id;
    star = stars[star_id];
    spaceport_id = 0;

    orbit_radius = orb_rad;
    orbit_degree = orb_deg;

    // Add this planet to map of all planets
    planets.insert({id, this});

    // Log planet creation
    Logger::log_message("New planet created: " + name, 0, Logger::GREEN);
}


int Planet::getId() {
    return id;
}


Spaceport* Planet::addSpaceport(string s_name, int _class) {

    // Create a new spaceport
    Address new_address = {star_id, id, star->getX(), star->getY(), orbit_radius, orbit_degree};
    Spaceport *new_port = new Spaceport(s_name, _class, new_address, star, this);

    // Set this spaceport as belonging to this planet
    spaceport_id = new_port->getId();

    return new_port;
}


string Planet::getJsonString() {

    json data;

    mtx.lock();

    data["id"] = id;
    data["star_id"] = star_id;
    data["sp_id"] = spaceport_id;
    data["name"] = name;

    data["orb_radius"] = orbit_radius;
    data["orb_degree"] = orbit_degree;

    mtx.unlock();

    return data.dump();
}
