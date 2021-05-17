/*
Object that manages a company, run by a user connection.

Generated when a new username connects, remains tied to that specific username.

*/

#pragma once

#include <string>
#include <map>

#include "server/src/globals.hpp"

using namespace std;


class Company {
public:
    static map <string, Company*> u_to_company; // map of all companies keyed by username to pointer

    static int id_counter;                      // value of the next company id to be issued
    string last_error;

private:

    int id;                                     // unique company id
    string name;                                // company name
    string user;                                // username of company owner
    map <int, Ship*> myships;                   // map containing directory of this company's ships
    mutex mtx;                                  // mutex to lock critical sections of class functions

public:

    // Constructor
    Company(string _name, string _user);

    // Add a ship to ownership of company, returns -1 for error
    int addShip(Ship *ship);

    // Rename the company
    int rename(string new_name);

    // Return all company information as a json string
    string getJsonString();

    // Get the company id
    int getId();

};