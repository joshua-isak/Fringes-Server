#include <time.h>
#include <iostream>

#include "globals.hpp"

#include "server/lib/nlohmann/json.hpp"
#include "ship.hpp"
#include "server/src/logger.hpp"

using json = nlohmann::json;


int Ship::id_counter = 1;


Ship::Ship(string _name, string _reg, ship_type _type, int m_w, int m_v, Spaceport *startport) {

    // Set unique id and iterate id counter
    id = id_counter;
    id_counter += 1;

    // Set initial variables
    company_id = 0;
    name = _name;
    registration = _reg;
    type = _type;
    max_weight = m_w;
    max_volume = m_v;
    current_spaceport = startport;
    departure_time = 0;
    arrival_time = 0;

    total_warps = 0;
    reliability = 1;
    next_spaceport = startport;
    last_spaceport = startport;
    travel_state = DOCKED;

    // Add ship to map of all ships
    ships.insert({id, this});

    // Log ship creation
    Logger::log_message("New ship created: " + registration, 0, Logger::CYAN);

    // Tell all clients this ship has been created
    Connection::syncInstance(0, "SYNC_SHIP", "INITIAL", this->getJsonString());
}


int Ship::rename(string new_name) {
    name = new_name;
    return 0;
}


string Ship::getName() {
    return name;
}


time_t Ship::getArrivalTime() {
    return arrival_time;
}


ship_state Ship::getState() {
    return travel_state;
}

int Ship::getId() {
    return id;
}


int Ship::getCompanyId() {
    return company_id;
}


int Ship::changeCompany(int new_company_id) {

    mtx.lock();

    // Make sure this ship is docked
    if (travel_state != DOCKED) {
        last_error = registration + " must be docked to change ownership!";
        return -1;
    }

    //--TODO--// Make sure this ship has no cargo

    // Change owning company
    company_id = new_company_id;

    mtx.unlock();

    return 0;
}


int Ship::depart(Spaceport *destination) {

    mtx.lock();

    // Find the warp time in seconds, 60 seconds for every distance 1, and cast to int
    float distance = destination->getDistance(current_spaceport);
    int warp_time = (int) (60 * distance);

    // Check if this ship is already warping
    if (travel_state == WARP) {
        last_error = registration + " already in warp!";
        mtx.unlock();
        return -1;
    }

    // Check if we are already at the destination
    if (destination == current_spaceport) {
        last_error = registration + " already at destination!";
        mtx.unlock();
        return -1;
    }

    // Update all variables relevant to departure
    time_t current_time = time(NULL);
    last_spaceport = current_spaceport;
    next_spaceport = destination;
    arrival_time = current_time + warp_time;
    departure_time = current_time;
    total_warps += 1;
    travel_state = WARP;

    mtx.unlock();

    // Log our departure and ETA in the console
    string output;
    output = registration + " " + name + " has departed " + last_spaceport->getName()
    + " for " + destination->getName() + " ETA: " + to_string(warp_time) + " seconds";
    Logger::log_message(output, 0, "");

    // Tell all clients this ship has departed
    Connection::syncInstance(0, "SYNC_SHIP", "DEPART", this->getJsonString());

    return 0;   // return the success state
}


void Ship::arrive() {

    mtx.lock();

    // Update all variables relevant to arrival
    travel_state = DOCKED;
    current_spaceport = next_spaceport;

    mtx.unlock();

    // Log our arrival in the console
    string output;
    output = registration + " " + name + " has arrived at " + next_spaceport->getName();
    Logger::log_message(output, 0, "");

    // Tell all clients this ship has arrived
    Connection::syncInstance(0, "SYNC_SHIP", "ARRIVE", this->getJsonString());

}


int Ship::addCargo() { return 0; }


int Ship::removeCargo() { return 0; }


string Ship::getJsonString() {

    mtx.lock();

    json x;

    x["id"] = id;
    x["company_id"] = company_id;
    x["name"] = name;
    x["registration"] = registration;
    x["type"] = type;
    x["max_weight"] = max_weight;
    x["max_volume"] = max_volume;
    x["total_warps"] = total_warps;
    x["reliability"] = reliability;
    // cargo manifest
    x["last_spaceport"] = last_spaceport->getId();
    x["next_spaceport"] = next_spaceport->getId();
    x["current_spaceport"] = current_spaceport->getId();
    x["departure_time"] = departure_time;
    x["arrival_time"] = arrival_time;
    x["travel_state"] = travel_state;

    mtx.unlock();

    return x.dump();
}