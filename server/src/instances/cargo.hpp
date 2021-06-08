/*
Instance that represents a unit of cargo.

Flyweight model is used with cargo_info struct that contains information that can be shared among
multiple instances.

Spaceports use Producers to generate new cargo, Consumers tell producers where to route their
produced cargo.

Cargo instances contain no reference to the spaceport or ship that they currently belong to.
*/

#pragma once

#include <string>
#include <mutex>

#include "server/src/globals.hpp"

using namespace std;


// Collection of information that can be shared between cargo instances
struct cargo_type {
    int id;                 // unique product id

    string name;            // name of cargo
    string description;     // flavortext description of cargo

    float volatility;       // 0 to 1, higher is more volatile
    int base_value;         // base delivery payout
};



// POD attached to a spaceport that gives info on generation of specfic new cargo instances
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



// POD attached to a spaceport to let Producers know where new cargo should be routed
struct Consumer {
    int id;
    int spaceport_id;

    cargo_type *product;

    int demand;
};



class Cargo {
public:
    static int id_counter;                      // value of next unique cargo id to be issued
    static int product_id_counter;
    static int producer_id_counter;
    static int consumer_id_counter;
    static map <int, cargo_type*> all_products;  // map of all cargo_types
    static map <int , Producer*> producers;      // map of all spaceport's producers

private:
    int id;                     // unique cargo instance id

    cargo_type *info;           // flyweight info struct

    Spaceport *origin;          // spaceport of origin
    Spaceport *destination;     // destination spaceport

    //time_t deadline;          // time of delivery bonus expiration

    int value;                  // final calculated delivery value

    mutex mtx;

public:
    // Constructor
    Cargo(cargo_type *type, Spaceport *origin, Spaceport *dest);

    // Destructor
    ~Cargo();

    // Get cargo id
    int getId();

    // Get cargo value (final value)
    int getValue();

    // Get cargo origin spaceport
    Spaceport* getOrigin();

    // Create a new cargo_type struct and add it to the map of all products
    static cargo_type* addProduct(string name, string desc, float volatility, int base_val);

    // Return all of a cargo instance's information as a json string
    string getJsonString();

    // Initial sync all cargo types to the client belonging to client_id
    int syncAllProducts(int client_id);

    // Initial sync all cargo instances to the client belonging to client_id
    int syncAllCargo(int client_id);
};