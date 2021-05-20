#include <string>
#include <vector>

#include "globals.hpp"

#include "server/lib/nlohmann/json.hpp"
#include "server/src/logger.hpp"

using json = nlohmann::json;


// Static class variables
int Company::id_counter = 1;
map <string, Company*> Company::u_to_company;


Company::Company(string _name, string _user) {

    // Set unique id and iterate id counter
    id = id_counter;
    id_counter++;

    // Set initial variables
    user = _user;
    name = _name;

    // Log company creation
    Logger::log_message("New company generated for " + user, 0, Logger::GREEN);

    // Generate 7 ships for the company     // TESTING PURPOSES ONLY REALLY
    for (int i = 0; i < 7; i++) {

        int x = Ship::id_counter;
        string new_reg = "NCC-" + to_string(1000 + x);

        Ship *new_ship = new Ship("Unnamed_ship", new_reg, MEDIUM, 16, 16, spaceports[1]);
        new_ship->changeCompany(id);
        myships.insert({new_ship->getId(), new_ship});
    }

    // Add company to map of all companies
    companies.insert({id, this});

    // Add company to internal map of all companies keyed by username
    u_to_company.insert({user, this});
}


int Company::getId() {
    return id;
}


int Company::addShip(Ship *ship) {

    // Try to change ship's company owner
    if (ship->changeCompany(id) < 0) {
        last_error = name + " " + to_string(ship->getId()) + " could not change company";
        return -1;
    }

    // Add ship to company map of all ships
    myships.insert({ship->getId(), ship});
    return 0;
}


string Company::getJsonString() {

    mtx.lock();

    json x;

    x["id"] = id;
    x["name"] = name;
    x["user"] = user;
    x["num_ships"] = myships.size();

    vector <int> myship_ids;
    for (auto const& element : myships) {
        myship_ids.push_back(element.first);
    }

    x["ships"] = myship_ids;

    mtx.unlock();

    return x.dump();
}