/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#include "state.hpp"
#include <random>

// Initialize the simulation state
void init_state(SimulationState &S)
{
  int L = S.hyper.lane_count;
  int N = S.hyper.road_length;

  for (int lane = 0; lane < L; ++lane)
  {
    S.road_storage[lane] = new Car *[N];
    S.road_copy_storage[lane] = new Car *[N];
    S.roads[lane] = S.road_storage[lane];
    S.roads_copy[lane] = S.road_copy_storage[lane];
    for (int pos = 0; pos < N; ++pos)
    {
      S.roads[lane][pos] = nullptr;
      S.roads_copy[lane][pos] = nullptr;
    }
  }
}

// Populate cars on the road based on density
void populate_cars(SimulationState &S)
{
  int total_cars = static_cast<int>(S.hyper.density * S.hyper.road_length *
                                    S.hyper.lane_count);
  int car_id = 0;
  // Randomly place cars on the road
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> lane_dis(0, S.hyper.lane_count - 1);
  std::uniform_int_distribution<> pos_dis(0, S.hyper.road_length - 1);
  while (car_id < total_cars)
  {
    int lane = lane_dis(gen);
    int position = pos_dis(gen);
    if (S.roads[lane][position] == nullptr)
    { // empty spot
      Car *new_car = new Car{car_id, lane, position, 0};
      S.roads[lane][position] = new_car;
      car_id++;
    }
  }
  // Copy initial state to copy roads
  for (int lane = 0; lane < S.hyper.lane_count; ++lane)
  {
    for (int pos = 0; pos < S.hyper.road_length; ++pos)
    {
      S.roads_copy[lane][pos] = S.roads[lane][pos];
    }
  }
}

// Print the current state of the simulation
void print_state(const SimulationState &S)
{
  for (int lane = 0; lane < S.hyper.lane_count; lane++)
  {
    for (int pos = 0; pos < S.hyper.road_length; pos++)
    {
      Car *car = S.roads[lane][pos];
      if (car == nullptr)
      {
        std::printf("-");
      }
      else
      {
        std::printf("%d", car->velocity);
      }
    }
    std::printf("\n");
  }
  std::printf("\n\n\n");
}

// Deallocate resources used by the simulation state
void dealocate_state(SimulationState &S)
{
  for (int lane = 0; lane < S.hyper.lane_count; ++lane)
  {
    for (int pos = 0; pos < S.hyper.road_length; ++pos)
    {
      if (S.roads[lane][pos] != nullptr)
      {
        delete S.roads[lane][pos];
      }
    }
    delete[] S.road_storage[lane];
    delete[] S.road_copy_storage[lane];
  }
}
