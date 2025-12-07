/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#pragma once
#include "types.hpp"

// Lane change decision functions and operations
int count_gap_ahead(const SimulationState &S, Car *car, bool current_lane,
                    bool look_left);
int count_gap_behind(const SimulationState &S, Car *car, bool look_left);

// Lane change decision tests
bool left_switch_t1(const SimulationState &S, Car *car);
bool left_switch_t2(const SimulationState &S, Car *car);
bool left_switch_t3(const SimulationState &S, Car *car);

bool right_switch_t1(const SimulationState &S, Car *car);
bool right_switch_t2(const SimulationState &S, Car *car);
bool right_switch_t3(const SimulationState &S, Car *car);

// Lane switching functions
bool switch_lane_left(const SimulationState &S, Car *car);
bool switch_lane_right(const SimulationState &S, Car *car);

// Lane switching resolution
void resolve_lane_switch_write_to_copy(SimulationState &S);

// Utility functions for copying road states
void clear_copy(SimulationState &S);
void copy_into_roads(SimulationState &S);
