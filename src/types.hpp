/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#pragma once
#include <cstdio>

// Car structure representing each vehicle
struct Car
{
  int id;
  int lane;
  int position;
  int velocity;
};

// Hyperparameters for the simulation
struct Hyperparameters
{
  int max_velocity;
  float slowdown_probability;
  int lane_count;
  int road_length;
  float density;
  float switch_probability;
  bool symmetric;
  bool position_time_data;
  bool lookback_zero;
  bool csv_output;
};

// Simulation state containing roads and hyperparameters
struct SimulationState
{
  Hyperparameters hyper;
  Car **roads[2];
  Car **roads_copy[2];
  Car **road_storage[2];
  Car **road_copy_storage[2];
};
