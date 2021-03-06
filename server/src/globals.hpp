// Global Variables

#pragma once

#include <map>

#include "server/src/ship.hpp"
#include "server/src/spaceport.hpp"
#include "server/src/network/connection.hpp"
#include "server/src/company.hpp"
#include "server/src/instances/star.hpp"
#include "server/src/instances/planet.hpp"
#include "server/src/instances/cargo.hpp"

using namespace std;

// Forward declare these classes to make the compiler happy...
class Cargo;


extern string password;                     // password clients need in order to connect to the server
extern int planet_tick_speed;               // Magic Number for Planet speeds

extern map <int, Ship*> ships;              // map of all ships (id, pointer)
extern map <int, Spaceport*> spaceports;    // map of all spaceports (id, pointer)
extern map <int, Planet*> planets;          // map of all planets
extern map <int, Star*> stars;              // map of all stars
extern map <int, Cargo*> cargos;            // map of all cargo instances

extern map <int, Connection*> connections;  // map of all active TCP connections (socket_id, pointer)

extern map <int, Company*> companies;       // map of all companies (id, pointer)