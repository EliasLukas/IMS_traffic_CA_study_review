#pragma once
#include "types.hpp"

void gather_data(const SimulationState &S);
void gather_init(const SimulationState &S, int total_ticks);
void gather_close();