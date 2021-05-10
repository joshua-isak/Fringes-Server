#include "server/lib/nlohmann/json.hpp"
#include "ship.hpp"

using json = nlohmann::json;

Ship::Ship(int _id, string _name, string _reg, ship_type _type, int m_w, int m_v, Spaceport *startport) {
    id = _id;
    name = _name;
    registration = _reg;
    type = _type;
    max_weight = m_w;
    max_volume = m_v;
    docked_spaceport = startport;
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


int Ship::depart(Spaceport *destination) {}


int Ship::addCargo() {}


int Ship::removeCargo() {}


string Ship::getJsonString() {
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
    x["docked_spaceport"] = docked_spaceport->getId();
    x["departure_time"] = departure_time;
    x["arrival_time"] = arrival_time;
    x["travel_state"] = travel_state;

    return x.dump();
}