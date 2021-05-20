
#include <vector>

#include "server/src/globals.hpp"

#include "server/lib/nlohmann/json.hpp"
#include "server/src/instances/star.hpp"
#include "server/src/logger.hpp"

using json = nlohmann::json;

int Star::id_counter = 1;


Star::Star(string _name, star_type _type, float _x, float _y) {

    // Set unique id and iterate id counter
    id = id_counter;
    id_counter++;

    // Set initial variables
    name = _name;
    type = _type;
    x = _x;
    y = _y;

    // Add star to map of all stars
    stars.insert({id, this});

    // Log star creation
    Logger::log_message("New star created: " + name, 0, Logger::GREEN);

    // Tell all clients this star has been created
    //Connection::syncInstance(0, "SYNC_STAR", "INITIAL", this->getJsonString());

}


int Star::getId() {
    return id;
}


float Star::getX() {
    return x;
}


float Star::getY() {
    return y;
}


Planet* Star::addPlanet(string _name, float _orb_rad, float _orb_deg) {

    // Create a new planet
    Planet *new_planet = new Planet(_name, id, _orb_rad, _orb_deg);

    // Add it to this star's map of planets
    myplanets.insert({new_planet->getId(), new_planet});

    return new_planet;
}


string Star::getJsonString() {

    json data;

    mtx.lock();

    data["id"] = id;
    data["name"] = name;
    data["type"] = type;
    data["x"] = x;
    data["y"] = y;
    data["num_planets"] = myplanets.size();

    vector <int> myplanet_ids;
    for (auto const& element : myplanets) {
        myplanet_ids.push_back(element.first);
    }

    data["planets"] = myplanet_ids;

    mtx.unlock();

    return data.dump();
}