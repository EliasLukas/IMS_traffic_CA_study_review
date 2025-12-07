/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#pragma once
#include "lane_change.hpp"
#include "types.hpp"

// Velocity update decision tests
bool velocity_t1(const SimulationState &S, Car *car);
bool velocity_t2(const SimulationState &S, Car *car);
bool velocity_t3(const SimulationState &S, Car *car);

// Velocity update functions
void update_car_velocity(SimulationState &S);
void update_position_in_copy(SimulationState &S);
