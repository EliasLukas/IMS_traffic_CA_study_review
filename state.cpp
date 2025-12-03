#include "state.hpp"
#include <random>

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

void populate_cars(SimulationState &S)
{
  int total_cars = static_cast<int>(S.hyper.density * S.hyper.road_length *
                                    S.hyper.lane_count);
  int car_id = 0;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> lane_dis(0, S.hyper.lane_count - 1);
  std::uniform_int_distribution<> pos_dis(0, S.hyper.road_length - 1);
  while (car_id < total_cars)
  {
    int lane = lane_dis(gen);
    int position = pos_dis(gen);
    if (S.roads[lane][position] == nullptr)
    {
      Car *new_car = new Car{car_id, lane, position, 0};
      S.roads[lane][position] = new_car;
      car_id++;
    }
  }
  for (int lane = 0; lane < S.hyper.lane_count; ++lane)
  {
    for (int pos = 0; pos < S.hyper.road_length; ++pos)
    {
      S.roads_copy[lane][pos] = S.roads[lane][pos];
    }
  }
}

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
