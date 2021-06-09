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
    max_cargo = 3;
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

    mtx.lock();
    string old_name = name;     // store old name to write into console
    name = new_name;
    mtx.unlock();

    // Log name change in console
    Logger::log_message(registration + " " + old_name + " changed name to " + new_name, 1, "");

    // Tell all clients about name change   //--TODO--// chance to only include name string and id in json
    Connection::syncInstance(0, "SYNC_SHIP", "RENAME", this->getJsonString());

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

    // Check if any of our cargo can be delivered to our arrival station
    map<int, Cargo*>::iterator ic;
    map<int, Cargo*> deliverable_cargo;

    for (ic = my_cargo.begin(); ic != my_cargo.end(); ic++) {

        Cargo *this_cargo = ic->second;

        if (this_cargo->getDestination() == current_spaceport) {
            deliverable_cargo.insert({this_cargo->getId(), this_cargo});
        }
    }

    // Remove and destroy all deliverable cargo
    for (ic = deliverable_cargo.begin(); ic != deliverable_cargo.end(); ic++) {
        this->removeCargoAndDestroy(ic->first);
    }

    // Tell all clients this ship has arrived
    Connection::syncInstance(0, "SYNC_SHIP", "ARRIVE", this->getJsonString());

}


int Ship::addCargoFromSpaceport(int cargo_id) {

    mtx.lock();

    Cargo *this_cargo = cargos[cargo_id];

    // Make sure this ship is docked at a spaceport
    if (travel_state != DOCKED) {
        last_error = registration + " cannot add cargo, not docked!";
        mtx.unlock();
        return -1;
    }

    // Make sure the cargo's origin is at the spaceport the ship is docked to
    if (this_cargo->getOrigin() != current_spaceport) {
        last_error = registration + " cannot add cargo, not at same spaceport as cargo!";
        mtx.unlock();
        return -1;
    }

    // Make sure adding this cargo does not exceed the ship's maximum capacity
    if (my_cargo.size() + 1 > max_cargo) {
        last_error = registration + " cannot add cargo, ship hold full";
        mtx.unlock();
        return -1;
    }

    // TODO // Make sure the company can afford to purchase this cargo

    // Remove cargo from spaceport's map of all cargo (and make sure it's still there)
    if (this_cargo->getOrigin()->removeCargo(cargo_id) == NULL) {
        last_error = registration + " cannot add cargo, not in spaceport's bulletin";
        mtx.unlock();
        return -1;
    }

    // Add cargo to this ship's map of all cargo
    my_cargo.insert({this_cargo->getId(), this_cargo});

    // Update this ship's cargo manifest to all clients
    json x;

    vector <int> my_cargo_ids;
    for (auto const& element : my_cargo) {
        my_cargo_ids.push_back(element.first);
    }

    x["cargo"] = my_cargo_ids;
    x["id"] = id;

    mtx.unlock();

    Connection::syncInstance(0, "SYNC_SHIP", "CARGO", x.dump() );

    return 0;   // return success
}





int Ship::removeCargoAndDestroy(int cargo_id) {

    // Remove cargo from ship's manifest
    my_cargo.erase(cargo_id);

    // Remove cargo from existence
    delete(cargos[cargo_id]);

 }


string Ship::getJsonString() {

    mtx.lock();

    json x;

    x["id"] = id;
    x["company_id"] = company_id;
    x["name"] = name;
    x["registration"] = registration;
    x["type"] = type;
    x["warps"] = total_warps;
    x["reliability"] = reliability;

    // cargo manifest
    vector <int> my_cargo_ids;
    for (auto const& element : my_cargo) {
        my_cargo_ids.push_back(element.first);
    }
    x["cargo"] = my_cargo_ids;
    x["max_cargo"] = max_cargo;

    x["last_spaceport"] = last_spaceport->getId();
    x["next_spaceport"] = next_spaceport->getId();
    x["current_spaceport"] = current_spaceport->getId();
    x["departure_time"] = departure_time;
    x["arrival_time"] = arrival_time;
    x["travel_state"] = travel_state;

    mtx.unlock();

    return x.dump();
}