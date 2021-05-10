#pragma once

#include <string>

using namespace std;

struct Address {
    string star_name;
    int star_id;
    float distance_from_star;
    string location;
    float pos_x;
    float pos_y;
};

class Spaceport {
private:
    int id;                     // unique spaceport id
    string name;                // spaceport name

    int station_class;          // station level of development

    Address address;           // station location in space

    //map <> cargo              // map containing the station's cargo bulletin

public:
    // Constructor
    Spaceport(int _id, string _name, int _class, Address _address);

    // Get spaceport id
    int getId();

    // Get all spaceport information as a json string
    string getJsonString();
};