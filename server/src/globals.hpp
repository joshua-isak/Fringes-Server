// Global Variables

#pragma once

#include <map>

#include "server/src/ship.hpp"
#include "server/src/spaceport.hpp"

using namespace std;

extern map <int, Ship*> ships;              // map of all ships (id, pointer)
extern map <int, Spaceport*> spaceports;    // map of all spaceports (id, pointer)