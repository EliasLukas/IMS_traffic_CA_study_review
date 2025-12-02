#pragma once
#include "lane_change.hpp"
#include "types.hpp"

bool velocity_t1(const SimulationState &S, Car *car);
bool velocity_t2(const SimulationState &S, Car *car);
bool velocity_t3(const SimulationState &S, Car *car);

void update_car_velocity(SimulationState &S);
void update_position_in_copy(SimulationState &S);
