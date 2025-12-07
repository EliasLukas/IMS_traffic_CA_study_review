/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#pragma once
#include "types.hpp"

// Initialize the simulation state
void init_state(SimulationState &S);
// Populate cars on the road based on density
void populate_cars(SimulationState &S);
// Print the current state of the simulation
void print_state(const SimulationState &S);
// Deallocate resources used by the simulation state
void dealocate_state(SimulationState &S);