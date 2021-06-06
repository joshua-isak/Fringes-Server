#include "cargo.hpp"

#include "server/lib/nlohmann/json.hpp"

using json = nlohmann::json;


int Cargo::id_counter = 1;


Cargo::Cargo() {
}


int Cargo::getId() {
    return id;
}


int Cargo::getValue() {
    return value;
}


string Cargo::getJsonString() {

    json x;

    mtx.lock();

    x["id"] = id;
    x["info_id"] = info_id;
    x["value"] = value;
    x["origin_id"] = origin->getId();
    x["dest_id"] = destination->getId();

    mtx.unlock();

    return x.dump();
}


int Cargo::syncAllCargoTypes(int client_id) {

}


int Cargo::syncAllCargo(int client_id) {

}