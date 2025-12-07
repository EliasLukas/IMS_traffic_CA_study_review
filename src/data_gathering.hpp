/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#pragma once
#include "types.hpp"

// DataGatherer class for collecting simulation data
class DataGatherer
{
public:
  void init(const SimulationState &S, int total_ticks);
  void gather(const SimulationState &S);
  void close();
  void calculate_flow(const SimulationState &S);
  void register_event_lane_swap();

private:
  // PBM outputs
  void open_pbm_once(int width, int height);
  void gather_position_time_data(const SimulationState &S);
  void write_lane_pbm_row(FILE *f, Car **lane, int width);
  void sum_for_flow(const SimulationState &S);

  FILE *left_pbm_ = nullptr;
  FILE *right_pbm_ = nullptr;
  bool pbm_open_ = false;

  // Dimensions and counters
  int total_ticks_ = 0;
  int tick_index_ = 0;
  int written_rows_ = 0;
  int pbm_width_ = 0;
  int pbm_height_ = 0;
  int velocity_sum_left_ = 0;
  int velocity_sum_right_ = 0;
  int lane_swaps_ = 0;
  int flow_samples_ = 0;
};

// Singleton accessor
DataGatherer &get_data_gatherer();