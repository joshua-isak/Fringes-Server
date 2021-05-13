// Global Variables

#pragma once

#include <map>

#include "server/src/ship.hpp"
#include "server/src/spaceport.hpp"
#include "server/src/network/connection.hpp"

using namespace std;

extern string password;                     // password clients need in order to connect

extern map <int, Ship*> ships;              // map of all ships (id, pointer)
extern map <int, Spaceport*> spaceports;    // map of all spaceports (id, pointer)
extern map <int, Connection*> connections;  // map of all active TCP connections