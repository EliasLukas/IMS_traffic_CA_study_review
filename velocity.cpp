#include "velocity.hpp"
#include <random>

// rng generator
static std::mt19937 &get_rng() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}

// Velocity update decision tests
bool velocity_t1(const SimulationState &S, Car *car) {
  return car->velocity < S.hyper.max_velocity; // can we accelerate?
}
bool velocity_t2(const SimulationState &S, Car *car) {
  int gap_ahead = count_gap_ahead(S, car, true, false);
  return car->velocity > gap_ahead; // is there a car ahead limiting us?
}
bool velocity_t3(const SimulationState &S, Car *car) {
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);
  float rand_val = dis(get_rng());
  return car->velocity > 0 &&
         rand_val <=
             S.hyper.slowdown_probability; // should we randomly slow down?
}

// Velocity update functions
void update_car_velocity(SimulationState &S) {
  for (int lane_index = 0; lane_index < S.hyper.lane_count; lane_index++) {
    for (int pos_index = 0; pos_index < S.hyper.road_length; pos_index++) {
      if (S.roads[lane_index][pos_index] == nullptr) {
        continue;
      }

      Car *car = S.roads[lane_index][pos_index];
      if (velocity_t1(S, car)) {
        car->velocity += 1;
      }
      if (velocity_t2(S, car)) {
        car->velocity = count_gap_ahead(S, car, true, false);
      }
      if (velocity_t3(S, car)) {
        car->velocity -= 1;
      }
    }
  }
}

// Update car positions in the copy roads
void update_position_in_copy(SimulationState &S) {
  for (int lane_index = 0; lane_index < S.hyper.lane_count; lane_index++) {
    for (int pos_index = 0; pos_index < S.hyper.road_length; pos_index++) {
      if (S.roads[lane_index][pos_index] == nullptr) {
        continue;
      }

      Car *car = S.roads[lane_index][pos_index];
      S.roads_copy[lane_index][pos_index] = nullptr;

      int new_pos = (pos_index + car->velocity) % S.hyper.road_length;
      S.roads_copy[lane_index][new_pos] = car;
      car->position = new_pos;
    }
  }
}
