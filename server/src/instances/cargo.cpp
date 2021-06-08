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
map <int, Consumer*> Cargo::consumers;


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

    // Tell all clients this cargo was created
    Connection::syncInstance(0, "SYNC_CARGO", "INITIAL", this->getJsonString() );

    // Log new cargo instance creation
    Logger::log_message("New cargo instance: " + info->name + " at: "
    + origin->getName() + " value: " + to_string(value) + " dest: " + destination->getName()
    , 3, Logger::GREEN);
}


Cargo::~Cargo() {
    // Remove from map of all cargos
    cargos.erase(id);

    // Tell all clients this cargo was destroyed
    json x;
    x["id"] = id;
    Connection::syncInstance(0, "SYNC_CARGO", "DESTROY", x.dump() );

    Logger::log_message("Destroyed a cargo", 3, "");
}


int Cargo::getId() {
    return id;
}


int Cargo::getValue() {
    return value;
}


Spaceport* Cargo::getOrigin() {
    return origin;
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

    // Tell all clients this product was created
    json z;

    z["id"] = x->id;
    z["name"] = x->name;
    z["desc"] = x->description;
    z["volatility"] = x->volatility;
    z["base_val"] = x->base_value;

    Connection::syncInstance(0, "SYNC_CARGO", "SYNC_PRODUCT", z.dump() );

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


int Cargo::syncAllProducts(int client_id) {

    map<int, cargo_type*>::iterator it;

    for (it = all_products.begin(); it != all_products.end(); it++) {
        cargo_type *this_product = it->second;

        json x;
        x["id"] = this_product->id;
        x["name"] = this_product->name;
        x["desc"] = this_product->name;
        x["volatility"] = this_product->volatility;
        x["base_val"] = this_product->base_value;

        Connection::syncInstance(client_id, "SYNC_CARGO", "PRODUCT", x.dump() );
    }

    return 1;   // return success
}


int Cargo::syncAllCargo(int client_id) {

    map<int, Cargo*>::iterator it;

    // Iterate through all cargos and send them to a client
    for (it = cargos.begin(); it != cargos.end(); it++) {
        Cargo *this_cargo = it->second;

        Connection::syncInstance(client_id, "SYNC_CARGO", "INITIAL", this_cargo->getJsonString() );
    }

    return 1;   // return success
}