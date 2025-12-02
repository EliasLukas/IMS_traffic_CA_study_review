#pragma once
#include "types.hpp"

int count_gap_ahead(const SimulationState &S, Car *car, bool current_lane,
                    bool look_left);
int count_gap_behind(const SimulationState &S, Car *car, bool look_left);

bool left_switch_t1(const SimulationState &S, Car *car);
bool left_switch_t2(const SimulationState &S, Car *car);
bool left_switch_t3(const SimulationState &S, Car *car);

bool right_switch_t1(const SimulationState &S, Car *car);
bool right_switch_t2(const SimulationState &S, Car *car);
bool right_switch_t3(const SimulationState &S, Car *car);

bool switch_lane_left(const SimulationState &S, Car *car);
bool switch_lane_right(const SimulationState &S, Car *car);

void resolve_lane_switch_write_to_copy(SimulationState &S);

void clear_copy(SimulationState &S);
void copy_into_roads(SimulationState &S);
