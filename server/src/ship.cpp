#include <time.h>
#include <iostream>

#include "globals.hpp"

#include "server/lib/nlohmann/json.hpp"
#include "ship.hpp"

using json = nlohmann::json;


int Ship::last_issued_id = 1;


Ship::Ship(string _name, string _reg, ship_type _type, int m_w, int m_v, Spaceport *startport) {
    id = last_issued_id;
    last_issued_id += 1;

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


int Ship::depart(Spaceport *destination) {

    mtx.lock();

    // Find the warp time in seconds, 60 seconds for every distance 1, and cast to int
    float distance = destination->getDistance(current_spaceport);
    int warp_time = (int) (60 * distance);

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
    time_t system_time = time(NULL);
    tm* now = localtime(&system_time);
    cout
    << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] "
    << registration << " " << name << " has departed " << last_spaceport->getName() << " for "
    << destination->getName() << " ETA: " << warp_time << " seconds" << endl;

    // Tell all clients this ship has departed
    //--TODO--//

    return 0;   // return the success state
}


void Ship::arrive() {

    mtx.lock();

    // Update all variables relevant to arrival
    travel_state = DOCKED;
    current_spaceport = next_spaceport;

    mtx.unlock();

    // Log our arrival in the console
    time_t system_time = time(NULL);
    tm* now = localtime(&system_time);
    cout
    << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] "
    << registration << " " << name << " has arrived at " << next_spaceport->getName() << endl;

    // Tell all clients this ship has arrived
    string data = this->getJsonString();
    char outbuf[data.length()];
    strcpy(outbuf, data.c_str());
    Connection::sendAll(outbuf, sizeof(outbuf));

}


int Ship::addCargo() { return 0; }


int Ship::removeCargo() { return 0; }


string Ship::getJsonString() {

    mtx.lock();

    json x;

    x["id"] = id;
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