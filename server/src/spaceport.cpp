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
    name = _name;
    station_level = _class;
    address = _address;
    star = mystar;
    planet = myplanet;
    target_cargo = station_level * 10;
    cargo_update_frequency = 180;           // update cargo manifest every 3 minutes
    next_cargo_update = time(NULL) + 3;     // wait 3 seconds before first cargo update

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

time_t Spaceport::getCargoUpdateTime() {
    return next_cargo_update;
}


int Spaceport::addProducer(cargo_type *product, int _max, int _min, int dest_pref, int dest_pref_weight, float _weight) {

    // Allocate a new producer and add it to the relevant maps
    Producer *x = new Producer();

    x->id = Cargo::producer_id_counter;     // assign a new id and iterate the counter
    Cargo::producer_id_counter++;

    x->spaceport_id = id;
    x->product = product;
    x->max_manifest = _max;
    x->min_manifest = _min;
    x->dest_pref = dest_pref;
    x->dest_pref_weight = dest_pref_weight;
    x->weight = _weight;

    Cargo::producers.insert({x->id, x});        // add this producer to map of all producers
    my_producers.insert({x->id, x});            // add this producer to this station's map of its producers
    product->producers.insert({x->id, x});   // add this producer to product's map of its producers

    Logger::log_message("New producer for " + x->product->name + " created at " + name, 1, Logger::GREEN);

    return 1;
}


int Spaceport::addConsumer(cargo_type *product, int demand) {

    // Allocate a new consumer and add it to the relevant maps
    Consumer *x = new Consumer();

    x->id = Cargo::consumer_id_counter;     // assign a new id and iterate the counter
    Cargo::consumer_id_counter++;

    x->spaceport_id = id;
    x->product = product;
    x->demand = demand;

    // Add to all relevant map of consumers
    Cargo::consumers.insert({x->id, x});
    my_consumers.insert({x->id, x});
    product->consumers.insert({x->id, x});

    Logger::log_message("New consumer for " + x->product->name + " created at " + name, 1, Logger::GREEN);

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

    mtx.unlock();

    // Update all clients on this station's new cargo bulletin
    Connection::syncInstance(0, "SYNC_STATION", "CARGO", this->getCargoAndIdJson() );

    return this_cargo;
}


int Spaceport::updateCargoManifest(bool force) {
    int num_cargo_created = 0;

    // Remove and delete all cargo from manifest
    if (force) {

        // Clear the station's manifest
        mtx.lock();
        map<int, Cargo*> old_manifest = my_cargo;
        my_cargo.clear();
        mtx.unlock();

        // Tell all clients the station's manifest is now empty
        Connection::syncInstance(0, "SYNC_STATION", "CARGO", this->getCargoAndIdJson() );

        // Iterate through all of this spaceport's old cargo and delete the cargo instances
        map<int, Cargo*>::iterator it;
        for (it = old_manifest.begin(); it != old_manifest.end(); it++) { delete(it->second); }
    }

    mtx.lock();

    // Add cargo to the manifest until the target amount is reached
    map<int, Producer*>::iterator ip;
    while (my_cargo.size() < target_cargo) {

        // Skip all this if there are no producers
        if (my_producers.size() == 0) { break; }

        // Iterate through all producers
        for (ip = my_producers.begin(); ip != my_producers.end(); ip++) {

            map<int, Consumer*> ip_consumers = ip->second->product->consumers;  // sanity variable

            // Choose a random consumer for this producer's product   //--TODO--// This is SUPER SLOW
            map<int, Consumer*>::iterator random_consumer = ip_consumers.begin();
            int random_index = rand() % ip_consumers.size();
            advance(random_consumer, random_index);

            // Don't use a consumer belonging to this own station
            if (random_consumer->second->spaceport_id == id) { continue; }

            // Add a cargo bound for the above consumer to this spaceport's manifest
            Cargo *new_cargo = new Cargo(ip->second->product, this, spaceports[random_consumer->second->spaceport_id]);
            my_cargo.insert({new_cargo->getId(), new_cargo});
        }
    }

    mtx.unlock();

    // Reset the time until next manifest update
    next_cargo_update = time(NULL) + cargo_update_frequency;

    // Update clients on this spaceport's new cargo manifest
    Connection::syncInstance(0, "SYNC_STATION", "CARGO", this->getCargoAndIdJson() );

    return num_cargo_created;
}


string Spaceport::getCargoAndIdJson() {

    json x;

    mtx.lock();

    vector <int> my_cargo_ids;
    for (auto const& element : my_cargo) { my_cargo_ids.push_back(element.first); }

    x["id"] = id;
    x["cargo"] = my_cargo_ids;
    x["cu_time"] = next_cargo_update;

    mtx.unlock();

    return x.dump();
}


string Spaceport::getJsonString() {

    json x;

    mtx.lock();

    x["id"] = id;
    x["name"] = name;
    x["station_level"] = station_level;
    x["address"]["star_id"] = address.star_id;
    x["address"]["planet_id"] = address.planet_id;
    //x["address"]["orb_radius"] = address.orb_radius;
    //x["address"]["orb_degrees"] = address.orb_degrees;
    x["address"]["star_x"] = address.star_x;
    x["address"]["star_y"] = address.star_y;
    x["cu_time"] = next_cargo_update;

    // cargo bulletin, id of all contained cargo instances
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