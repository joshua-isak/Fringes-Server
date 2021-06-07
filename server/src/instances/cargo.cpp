#include "cargo.hpp"
#include "server/src/logger.hpp"

#include "server/lib/nlohmann/json.hpp"


using json = nlohmann::json;


// Static variables
int Cargo::id_counter = 1;
int Cargo::product_id_counter = 1;
int Cargo::producer_id_counter = 1;
int Cargo::consumer_id_counter = 1;
map <int, cargo_type*> Cargo::all_products;
map <int, Producer*> Cargo::producers;


Cargo::Cargo(cargo_type *type, Spaceport *_origin, Spaceport *_dest) {

    // Set unique id and iterate counter
    id = id_counter;
    id_counter++;

    // Set initial variables
    info = type;
    origin = _origin;
    destination = _dest;

    // Calculate delivery value
    float distance = origin->getDistance(destination);
    float x = info->base_value * distance;
    value = (int) pow(x, 1 + (info->volatility * 2) );

    // Add to map of cargos
    cargos.insert({id, this});

    // Log new cargo instance creation
    Logger::log_message("New cargo instance: " + info->name + " at: "
    + origin->getName() + " value: " + to_string(value), 3, Logger::GREEN);
}


Cargo::~Cargo() {
    // Remove from map of all cargos
    cargos.erase(id);
    Logger::log_message("Destroyed a cargo", 3, "");
}


int Cargo::getId() {
    return id;
}


int Cargo::getValue() {
    return value;
}


cargo_type* Cargo::addProduct(string name, string desc, float volatility, int base_value) {

    // Allocate a new cargo_type and add it to its map
    cargo_type *x = new cargo_type();

    x->id = product_id_counter;
    product_id_counter++;

    x->name = name;
    x->description  = desc;
    x->volatility = volatility;
    x->base_value = base_value;

    all_products.insert({x->id, x});

    Logger::log_message("New product created: " + name, 1, Logger::GREEN);

    return x;
}


string Cargo::getJsonString() {

    json x;

    mtx.lock();

    x["id"] = id;
    x["info_id"] = info->id;
    x["value"] = value;
    x["origin_id"] = origin->getId();
    x["dest_id"] = destination->getId();

    mtx.unlock();

    return x.dump();
}


int Cargo::syncAllCargoTypes(int client_id) {
    return 0;
}


int Cargo::syncAllCargo(int client_id) {
    return 0;
}