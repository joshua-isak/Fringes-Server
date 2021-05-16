#pragma once

#include <string>

using namespace std;

struct Address {
    string star_name;
    int star_id;
    float distance_from_star;
    //float orbit_position;
    string location;
    float pos_x;
    float pos_y;
};

class Spaceport {
public:
    static int id_counter;      // value of next spaceport id to be issued

private:
    int id;                     // unique spaceport id
    string name;                // spaceport name

    int station_class;          // station level of development

    Address address;           // station location in space

    //map <> cargo              // map containing the station's cargo bulletin

public:
    // Constructor
    Spaceport(string _name, int _class, Address _address);

    // Get spaceport id
    int getId();

    // Get spaceport address
    Address getAddress();

    // Get spaceport name
    string getName();

    // Get all spaceport information as a json string
    string getJsonString();

    // Return the distance between this spaceport and the provided one
    float getDistance(Spaceport *b);
};