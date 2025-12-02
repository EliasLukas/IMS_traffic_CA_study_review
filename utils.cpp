#include "utils.hpp"
#include <random>

const int ROAD_LENGTH = 100; // Use a constant for array size
const int TICK_COUNT = 5;    // Number of simulation ticks

// dummy data for testing
Hyperparameters hyperparameters = {.max_velocity = 3,
                                   .slowdown_probability = 0.00f,
                                   .lane_count = 2,
                                   .road_length = ROAD_LENGTH,
                                   .density = 0.1f,
                                   .switch_probability = 0.99f,
                                   .symmetric = false};

Car *road1[ROAD_LENGTH];
Car *road2[ROAD_LENGTH];
Car *road1_cp[ROAD_LENGTH];
Car *road2_cp[ROAD_LENGTH];

Car **roads[] = {road1, road2};
Car **roads_copy[] = {road1_cp, road2_cp};

// retuns the gap ahead in the specified lane (current_lane = true for current
// lane, false for other lane; look_left = true for left lane, false for right
// lane) gap is int up to max velocity
int count_gap_ahead(Car *car, bool current_lane, bool look_left)
{
  int lane;
  if (current_lane)
  {
    lane = car->lane;
  }
  else if (look_left)
  {
    lane = (car->lane - 1);
  }
  else
  {
    lane = (car->lane + 1);
  }

  if (lane < 0 || lane >= hyperparameters.lane_count)
  {
    return -1; // invalid lane
  }

  int gap = 0;
  int position = car->position;

  Car **current_road = roads[lane];
  int road_length = hyperparameters.road_length;

  for (int i = 1; i <= hyperparameters.max_velocity + 1; i++)
  {
    int check_position = (position + i) % road_length;
    if (current_road[check_position] == nullptr)
    {
      gap++;
    }
    else
    {
      break;
    }
  }

  return gap;
}

// Attention - return value -1 is valid, -1 means there is a car next to the
// current car
int count_gap_behind(Car *car, bool look_left)
{
  int lane;
  if (look_left)
  {
    lane = (car->lane - 1);
  }
  else
  {
    lane = (car->lane + 1);
  }

  if (lane < 0 || lane >= hyperparameters.lane_count)
  {
    return -2; // invalid lane
  }

  int gap = -1;
  int position = car->position;

  Car **current_road = roads[lane];
  int road_length = hyperparameters.road_length;

  for (int i = 0; i <= hyperparameters.max_velocity + 1; i++)
  {
    int check_position = (position - i + road_length) % road_length;
    if (current_road[check_position] == nullptr)
    {
      gap++;
    }
    else
    {
      break;
    }
  }

  return gap;
}

bool left_switch_t1(Car *car)
{
  int car_velocity = car->velocity;
  int gap_ahead = count_gap_ahead(car, true, false);
  if (gap_ahead < 0)
  {
    return false;
  }
  if (gap_ahead < car_velocity + 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool left_switch_t2(Car *car)
{
  int car_velocity = car->velocity;
  int gap_ahead_other = count_gap_ahead(car, false, true);
  if (gap_ahead_other < 0)
  {
    return false;
  }
  if (gap_ahead_other > car_velocity + 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool left_switch_t3(Car *car)
{
  int min_space_behind = hyperparameters.max_velocity;
  int gap_behind = count_gap_behind(car, true);
  if (gap_behind < -1)
  {
    return false;
  }
  if (gap_behind > min_space_behind)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool switch_lane_left(Car *car)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  float rand_val = dis(gen);
  float switch_prob = hyperparameters.switch_probability;
  if (left_switch_t1(car) && left_switch_t2(car) && left_switch_t3(car) &&
      rand_val <= switch_prob)
  {
    return true;
  }
  else
  {
    return false;
  }
}

///////////////////////////////////////////

// todo switch_lane_right
bool right_switch_t1(Car *car)
{
  // if assymetric rules, we want to move right regardless of ahead gap
  if (hyperparameters.symmetric == false)
  {
    return true;
  }
  int car_velocity = car->velocity;
  int gap_ahead = count_gap_ahead(car, true, true);
  if (gap_ahead < 0)
  {
    return false;
  }
  if (gap_ahead < car_velocity + 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool right_switch_t2(Car *car)
{
  int car_velocity = car->velocity;
  int gap_ahead_other = count_gap_ahead(car, false, false);
  if (gap_ahead_other < 0)
  {
    return false;
  }
  if (gap_ahead_other > car_velocity + 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool right_switch_t3(Car *car)
{
  int min_space_behind = hyperparameters.max_velocity;
  int gap_behind = count_gap_behind(car, false);
  if (gap_behind < -1)
  {
    return false;
  }
  if (gap_behind > min_space_behind)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool switch_lane_right(Car *car)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  float rand_val = dis(gen);
  float switch_prob = hyperparameters.switch_probability;
  if (right_switch_t1(car) && right_switch_t2(car) && right_switch_t3(car) &&
      rand_val <= switch_prob)
  {
    return true;
  }
  else
  {
    return false;
  }
}

///////////////////////////////////////////

// todo pravidla pro zmeny rychlosti
bool velocity_t1(Car *car)
{
  if (car->velocity < hyperparameters.max_velocity)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool velocity_t2(Car *car)
{
  int gap_ahead = count_gap_ahead(car, true, false);
  if (car->velocity > gap_ahead)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool velocity_t3(Car *car)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  float rand_val = dis(gen);
  if (car->velocity > 0 && rand_val <= hyperparameters.slowdown_probability)
  {
    return true;
  }
  else
  {
    return false;
  }
}

///////////////////////////////////////////////////

void resolve_lane_switch_write_to_copy()
{
  for (int lane_index = 0; lane_index < hyperparameters.lane_count; lane_index++)
  {
    for (int pos_index = 0; pos_index < ROAD_LENGTH; pos_index++)
    {
      if (roads[lane_index][pos_index] == nullptr)
      {
        continue;
      }

      Car *car = roads[lane_index][pos_index];
      bool left_switch = switch_lane_left(car);
      bool right_switch = switch_lane_right(car);

      if (left_switch && right_switch)
      {
        // switch left
        roads_copy[lane_index - 1][pos_index] = car;
        car->lane -= 1;
        roads_copy[lane_index][pos_index] = nullptr;
      }
      else if (left_switch)
      {
        // switch left
        roads_copy[lane_index - 1][pos_index] = car;
        car->lane -= 1;
        roads_copy[lane_index][pos_index] = nullptr;
      }
      else if (right_switch)
      {
        // switch right
        roads_copy[lane_index + 1][pos_index] = car;
        car->lane += 1;
        roads_copy[lane_index][pos_index] = nullptr;
      }
      else
      {
        // stay
        roads_copy[lane_index][pos_index] = car;
      }
    }
  }
}

void update_roads_by_roads_copy()
{
  for (int lane = 0; lane < hyperparameters.lane_count; lane++)
  {
    for (int pos = 0; pos < hyperparameters.road_length; pos++)
    {
      roads[lane][pos] = roads_copy[lane][pos];
    }
  }
}

void update_car_velocity()
{
  for (int lane_index = 0; lane_index < hyperparameters.lane_count; lane_index++)
  {
    for (int pos_index = 0; pos_index < ROAD_LENGTH; pos_index++)
    {
      if (roads[lane_index][pos_index] == nullptr)
      {
        continue;
      }

      Car *car = roads[lane_index][pos_index];

      if (velocity_t1(car))
      {
        car->velocity += 1;
      }
      if (velocity_t2(car))
      {
        car->velocity = count_gap_ahead(car, true, false);
      }
      if (velocity_t3(car))
      {
        car->velocity -= 1;
      }
    }
  }
}

void update_position_in_copy()
{
  for (int lane_index = 0; lane_index < hyperparameters.lane_count; lane_index++)
  {
    for (int pos_index = 0; pos_index < ROAD_LENGTH; pos_index++)
    {
      if (roads[lane_index][pos_index] == nullptr)
      {
        continue;
      }

      Car *car = roads[lane_index][pos_index];
      roads_copy[lane_index][pos_index] = nullptr;

      roads_copy[lane_index][(pos_index + car->velocity) % ROAD_LENGTH] = car;
      car->position = (car->position + car->velocity) % ROAD_LENGTH;
    }
  }
}

// todo simulation loop

void simulation_tick(Car ***roads, Car ***roads_copy)
{
  // first, resolve lane switching and write
  resolve_lane_switch_write_to_copy();
  // update OG roads by roads_copy updated with lane switch
  update_roads_by_roads_copy();
  // change car velocity according to rules
  update_car_velocity();
  // update position in loop
  update_position_in_copy();
  // update OG roads by roads_copy updated with lane switch
  update_roads_by_roads_copy();
}

void after_update_print_beta()
{
  for (int lane = 0; lane < hyperparameters.lane_count; lane++)
  {
    for (int pos = 0; pos < hyperparameters.road_length; pos++)
    {
      Car *car = roads[lane][pos];

      if (car == nullptr)
      {
        printf("-");
      }
      else
      {
        printf("%d", car->velocity);
      }
    }
    printf("\n");
  }
  printf("\n\n\n");
}

int main()
{
  // initialize roads
  for (int lane = 0; lane < hyperparameters.lane_count; lane++)
  {
    for (int pos = 0; pos < hyperparameters.road_length; pos++)
    {
      roads[lane][pos] = nullptr;
      roads_copy[lane][pos] = nullptr;
    }
  }
  // populate roads with cars based on density
  int total_cars =
      static_cast<int>(hyperparameters.density * hyperparameters.road_length *
                       hyperparameters.lane_count);
  int car_id = 0;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> lane_dis(0, hyperparameters.lane_count - 1);
  std::uniform_int_distribution<> pos_dis(0, hyperparameters.road_length - 1);
  while (car_id < total_cars)
  {
    int lane = lane_dis(gen);
    int position = pos_dis(gen);
    if (roads[lane][position] == nullptr)
    {
      Car *new_car = new Car{car_id, lane, position, 0};
      roads[lane][position] = new_car;
      car_id++;
    }
  }
  // copy whole roads for simultaneous updates
  for (int lane = 0; lane < hyperparameters.lane_count; lane++)
  {
    for (int pos = 0; pos < hyperparameters.road_length; pos++)
    {
      roads_copy[lane][pos] = roads[lane][pos];
    }
  }
  // run simulation for TICK_COUNT ticks
  for (int tick = 0; tick < TICK_COUNT; tick++)
  {
    after_update_print_beta();
    simulation_tick(roads, roads_copy);
  }
}

// todo vymyslet jak se zachovat kdyz oba switche budou valid

// todo arg parsing
