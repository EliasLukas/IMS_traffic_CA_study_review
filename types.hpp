#pragma once
#include <cstdio>

struct Car
{
  int id;
  int lane;
  int position;
  int velocity;
};

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
};

struct SimulationState
{
  Hyperparameters hyper;
  Car **roads[2];
  Car **roads_copy[2];
  Car **road_storage[2];
  Car **road_copy_storage[2];
};
