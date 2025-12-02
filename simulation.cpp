#include "simulation.hpp"
#include "lane_change.hpp"
#include "velocity.hpp"

void simulation_tick(SimulationState &S) {
  // Lane switching (simultaneous into copy)
  resolve_lane_switch_write_to_copy(S);
  update_roads_by_roads_copy(S);

  // Velocity update
  update_car_velocity(S);

  // Move cars simultaneously via copy
  update_position_in_copy(S);
  update_roads_by_roads_copy(S);
}
