#include "lane_change.hpp"
#include "data_gathering.hpp"
#include <random>

int count_gap_ahead(const SimulationState &S, Car *car, bool current_lane,
                    bool look_left)
{
  int lane =
      current_lane ? car->lane : (look_left ? car->lane - 1 : car->lane + 1);
  if (lane < 0 || lane >= S.hyper.lane_count)
  {
    return -1;
  }
  int gap = 0;
  int position = car->position;
  Car **current_road = S.roads[lane];
  for (int i = 1; i <= S.hyper.max_velocity + 1; i++)
  {
    int check_position = (position + i) % S.hyper.road_length;
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

int count_gap_behind(const SimulationState &S, Car *car, bool look_left)
{
  int lane = look_left ? car->lane - 1 : car->lane + 1;
  if (lane < 0 || lane >= S.hyper.lane_count)
  {
    return -2;
  }
  int gap = -1;
  int position = car->position;
  Car **current_road = S.roads[lane];
  for (int i = 0; i <= S.hyper.max_velocity + 1; i++)
  {
    int check_position =
        (position - i + S.hyper.road_length) % S.hyper.road_length;
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

bool left_switch_t1(const SimulationState &S, Car *car)
{
  int gap_ahead = count_gap_ahead(S, car, true, false);
  if (gap_ahead < 0)
  {
    return false;
  }
  return gap_ahead < car->velocity + 1;
}
bool left_switch_t2(const SimulationState &S, Car *car)
{
  int gap_ahead_other = count_gap_ahead(S, car, false, true);
  if (gap_ahead_other < 0)
  {
    return false;
  }
  return gap_ahead_other > car->velocity + 1;
}
bool left_switch_t3(const SimulationState &S, Car *car)
{
  int gap_behind = count_gap_behind(S, car, true);
  if (gap_behind < -1)
  {
    return false;
  }
  if (S.hyper.lookback_zero)
  {
    return gap_behind >= 0;
  }
  else
  {
    return gap_behind > S.hyper.max_velocity;
  }
}

bool right_switch_t1(const SimulationState &S, Car *car)
{
  if (!S.hyper.symmetric)
  {
    return true;
  }
  int gap_ahead = count_gap_ahead(S, car, true, true);
  if (gap_ahead < 0)
  {
    return false;
  }
  return gap_ahead < car->velocity + 1;
}
bool right_switch_t2(const SimulationState &S, Car *car)
{
  int gap_ahead_other = count_gap_ahead(S, car, false, false);
  if (gap_ahead_other < 0)
  {
    return false;
  }
  return gap_ahead_other > car->velocity + 1;
}
bool right_switch_t3(const SimulationState &S, Car *car)
{
  int gap_behind = count_gap_behind(S, car, false);
  if (gap_behind < -1)
  {
    return false;
  }
  if (S.hyper.lookback_zero)
  {
    return gap_behind >= 0;
  }
  else
  {
    return gap_behind > S.hyper.max_velocity;
  }
}

static std::mt19937 &get_rng()
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}

bool switch_lane_left(const SimulationState &S, Car *car)
{
  if (car->lane <= 0)
  {
    return false;
  }
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);
  float rand_val = dis(get_rng());
  return left_switch_t1(S, car) && left_switch_t2(S, car) &&
         left_switch_t3(S, car) && rand_val <= S.hyper.switch_probability;
}

bool switch_lane_right(const SimulationState &S, Car *car)
{
  if (car->lane >= S.hyper.lane_count - 1)
  {
    return false;
  }
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);
  float rand_val = dis(get_rng());
  return right_switch_t1(S, car) && right_switch_t2(S, car) &&
         right_switch_t3(S, car) && rand_val <= S.hyper.switch_probability;
}

void resolve_lane_switch_write_to_copy(SimulationState &S)
{
  for (int lane_index = 0; lane_index < S.hyper.lane_count; lane_index++)
  {
    for (int pos_index = 0; pos_index < S.hyper.road_length; pos_index++)
    {
      if (S.roads[lane_index][pos_index] == nullptr)
      {
        continue;
      }

      Car *car = S.roads[lane_index][pos_index];
      bool left_switch = switch_lane_left(S, car);
      bool right_switch = switch_lane_right(S, car);

      if (left_switch && right_switch)
      {
        // can not happen since max 2 lanes are permitted
      }
      else if (left_switch)
      {
        S.roads_copy[lane_index - 1][pos_index] = car;
        car->lane -= 1;

        // statistics gathering
        register_event_lane_swap();
      }
      else if (right_switch)
      {
        S.roads_copy[lane_index + 1][pos_index] = car;
        car->lane += 1;

        // statistics gathering
        register_event_lane_swap();
      }
      else
      {
        S.roads_copy[lane_index][pos_index] = car;
      }
    }
  }
}

void clear_copy(SimulationState &S)
{
  for (int l = 0; l < S.hyper.lane_count; ++l)
  {
    for (int p = 0; p < S.hyper.road_length; ++p)
    {
      S.roads_copy[l][p] = nullptr;
    }
  }
}

void copy_into_roads(SimulationState &S)
{
  for (int l = 0; l < S.hyper.lane_count; ++l)
  {
    for (int p = 0; p < S.hyper.road_length; ++p)
    {
      S.roads[l][p] = S.roads_copy[l][p];
    }
  }
}
