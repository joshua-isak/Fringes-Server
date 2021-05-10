#include <bits/stdc++.h>

#include "server/lib/nlohmann/json.hpp"
#include "spaceport.hpp"

using json = nlohmann::json;

Spaceport::Spaceport(int _id, string _name, int _class, Address _address) {
    id = _id;
    name = _name;
    station_class = _class;
    address = _address;
}


int Spaceport::getId() {
    return id;
}


Address Spaceport::getAddress() {
    return address;
}


string Spaceport::getName() {
    return name;
}


string Spaceport::getJsonString() {
    json x;

    x["id"] = id;
    x["name"] = name;
    x["station_class"] = station_class;
    x["address"]["star_name"] = address.star_name;
    x["address"]["star_id"] = address.star_id;
    x["address"]["distance_from_star"] = address.distance_from_star;
    x["address"]["location"] = address.location;
    x["address"]["pos_x"] = address.pos_x;
    x["address"]["pos_y"] = address.pos_y;

    return x.dump();
}



float Spaceport::getDistance(Spaceport *port_b) {
    Address a = address;
    Address b = port_b->getAddress();

    // Apply and return the distance formula
    double z1 = pow(b.pos_x - a.pos_x, 2);
    double z2 = pow(b.pos_y - a.pos_y, 2);

    return (float) sqrt(z1 + z2 * 1.0);
}