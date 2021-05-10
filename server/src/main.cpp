#include <map>
#include <iostream>

#include "ship.hpp"
#include "spaceport.hpp"

using namespace std;



map <int, Ship*> ships;              // map of all ships (id, pointer)
map <int, Spaceport*> spaceports;    // map of all spaceports (id, pointer)


int main(int argc, char *argv[]) {
    Address new_address = {"Sol", 1, 100, "some_string", 0, 0};
    Spaceport *port_1 = new Spaceport(1, "Terra Station", 1, new_address);

    int new_id = 1;
    Ship *ship_1 = new Ship(new_id, "Enterprise", "NCC-1701", SMALL, 16, 16, port_1);
    ships.insert({new_id, ship_1});

    string ship_info = ship_1->getJsonString();
    string station_info = port_1->getJsonString();

    cout << station_info << endl;
    cout << ship_info << endl;
}