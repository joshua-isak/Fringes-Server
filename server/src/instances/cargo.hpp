/*
Instance that represents a unit of cargo.

Flyweight model is used with cargo_info struct that contains information that can be shared among
multiple instances.
*/

#pragma once

#include <string>
#include <mutex>

#include "server/src/globals.hpp"

using namespace std;

// Collection of information that can be shared between cargo instances
struct cargo_type {
    string name;            // name of cargo
    string description;     // flavortext description of cargo

    float volatility;       // 0 to 1, higher is more volatile
    int base_value;         // base delivery payout
};


// Object attached to a spaceport that handles generation of specfic new cargo instances
struct Producer {
    int id;                     // unique producer id
    int spaceport_id;           // id of spaceport this producer is attached to

    cargo_type *product;        // reference to product's info

    int max_manifest;           // max spaceport manifest slots this product can occupy
    int min_manifest;           // min spaceport manifest slots this product should occupy

    int dest_pref;              // spaceport id of preferred product destination
    int dest_pref_weight;       // percent of total new products reserved for preferred destination

    float weight;               // percent of total manifest to allocate to this product (0 to 1)

};


// Object attached to a spaceport to let Producers know where new cargo should be routed
struct Consumer {
    int id;
    int spaceport_id;

    cargo_type *product;

    int demand;

};


class Cargo {
public:
    static int id_counter;      // value of next unique cargo id to be issued

private:
    int id;                     // unique cargo instance id

    cargo_type info;            // flyweight info struct
    int info_id;                // id of info struct

    Spaceport *origin;          // spaceport of origin
    Spaceport *destination;     // destination spaceport

    //time_t deadline;          // time of delivery bonus expiration

    int value;                  // final calculated delivery value

    mutex mtx;

public:
    // Constructor
    Cargo();

    // Get cargo id
    int getId();

    // Get cargo value (final value)
    int getValue();

    // Return all of cargo instance's information as a json string
    string getJsonString();

    // Initial sync all cargo types to the client belonging to client_id
    int syncAllCargoTypes(int client_id);

    // Initial sync all cargo instances to the client belonging to client_id
    int syncAllCargo(int client_id);
};