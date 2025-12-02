#include "simulation.hpp"
#include "lane_change.hpp"
#include "velocity.hpp"

void simulation_tick(SimulationState &S)
{
  // Lane switching (simultaneous into copy)
  clear_copy(S);
  resolve_lane_switch_write_to_copy(S);
  copy_into_roads(S);

  // Velocity update
  update_car_velocity(S);

  // Move cars simultaneously via copy
  clear_copy(S);
  update_position_in_copy(S);
  copy_into_roads(S);
}
