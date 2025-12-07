/////////////////////////////////////////
// Project: IMS 2025 T8
// Authors: Lukas Elias, xeliasl00
//          Jacek Folwarczny, xfolwaj00
/////////////////////////////////////////

#include "data_gathering.hpp"
#include "types.hpp"
#include <algorithm>
#include <cstdio>
#include <vector>

// Number of initial ticks to skip for settling
static const int k_settle_ticks = 1000;

// PBM output functions
void DataGatherer::open_pbm_once(int width, int height)
{
  if (pbm_open_)
  {
    return;
  }

  left_pbm_ = std::fopen("left_lane.pbm", "w");
  right_pbm_ = std::fopen("right_lane.pbm", "w");
  if (!left_pbm_ || !right_pbm_)
  {
    if (left_pbm_)
    {
      std::fclose(left_pbm_);
      left_pbm_ = nullptr;
    }
    if (right_pbm_)
    {
      std::fclose(right_pbm_);
      right_pbm_ = nullptr;
    }
    pbm_open_ = false;
    return;
  }

  std::fprintf(left_pbm_, "P1\n%d %d\n", width, height);
  std::fprintf(right_pbm_, "P1\n%d %d\n", width, height);

  pbm_open_ = true;
}

// Write a single row of PBM data for a lane
void DataGatherer::write_lane_pbm_row(FILE *f, Car **lane, int width)
{
  std::vector<char> row(width, '0');
  for (int pos = 0; pos < width; ++pos)
  {
    Car *c = lane[pos];
    if (!c)
    {
      continue;
    }
    const int vel = c->velocity;
    int start = pos - (vel > 0 ? (vel - 1) : 0);
    if (start < 0)
    {
      start = 0;
    }
    for (int i = start; i <= pos; ++i)
    {
      row[i] = '1';
    }
  }
  for (int x = 0; x < width; ++x)
  {
    std::fputc(row[x], f);
    if (x + 1 < width)
    {
      std::fputc(' ', f);
    }
  }
  std::fputc('\n', f);
}

// Gather position-time data into PBM files
void DataGatherer::gather_position_time_data(const SimulationState &S)
{
  if (!pbm_open_)
  {
    pbm_width_ = std::min(S.hyper.road_length, 400);
    pbm_height_ = std::max(0, total_ticks_ - k_settle_ticks);
    if (pbm_height_ <= 0)
    {
      return;
    }
    open_pbm_once(pbm_width_, pbm_height_);
  }
  if (!pbm_open_)
  {
    return;
  }
  if (written_rows_ >= pbm_height_)
  {
    return;
  }
  if (S.hyper.lane_count >= 1 && left_pbm_)
  {
    write_lane_pbm_row(left_pbm_, S.roads[0], pbm_width_);
    std::fflush(left_pbm_);
  }
  if (S.hyper.lane_count >= 2 && right_pbm_)
  {
    write_lane_pbm_row(right_pbm_, S.roads[1], pbm_width_);
    std::fflush(right_pbm_);
  }
  ++written_rows_;
}

// Initialize data gatherer
void DataGatherer::init(const SimulationState &S, int total_ticks)
{
  total_ticks_ = total_ticks;
  tick_index_ = 0;
  written_rows_ = 0;
  flow_samples_ = 0;
  velocity_sum_left_ = 0;
  velocity_sum_right_ = 0;
  lane_swaps_ = 0;

  pbm_open_ = false;
  left_pbm_ = nullptr;
  right_pbm_ = nullptr;

  pbm_width_ = std::min(S.hyper.road_length, 400);
  pbm_height_ = std::max(0, total_ticks - k_settle_ticks);
}

// Sum velocities for flow calculation
void DataGatherer::sum_for_flow(const SimulationState &S)
{
  const int width = S.hyper.road_length;
  if (S.hyper.lane_count >= 1)
  {
    Car **lane0 = S.roads[0];
    for (int pos = 0; pos < width; ++pos)
    {
      Car *c = lane0[pos];
      if (c)
      {
        velocity_sum_left_ += c->velocity;
      }
    }
  }
  if (S.hyper.lane_count >= 2)
  {
    Car **lane1 = S.roads[1];
    for (int pos = 0; pos < width; ++pos)
    {
      Car *c = lane1[pos];
      if (c)
      {
        velocity_sum_right_ += c->velocity;
      }
    }
  }
  ++flow_samples_;
}

// Gather data at each tick
void DataGatherer::gather(const SimulationState &S)
{
  ++tick_index_;
  if (tick_index_ < k_settle_ticks)
  {
    return;
  }
  if (S.hyper.position_time_data)
  {
    gather_position_time_data(S);
  }
  if (S.hyper.csv_output)
  {
    if (tick_index_ % 5 == 0)
    {
      sum_for_flow(S);
    }
  }
}

// Register a lane swap event
void DataGatherer::register_event_lane_swap()
{
  if (tick_index_ >= k_settle_ticks)
  {
    lane_swaps_++;
  }
}

// Calculate and print flow statistics
void DataGatherer::calculate_flow(const SimulationState &S)
{
  if (!S.hyper.csv_output)
  {
    return;
  }
  if (flow_samples_ == 0)
  {
    std::printf("0.000, 0.000, 0.000,\n");
    return;
  }
  const double denom = static_cast<double>(flow_samples_) *
                       static_cast<double>(S.hyper.road_length);
  const double left_flow = static_cast<double>(velocity_sum_left_) / denom;
  const double right_flow = static_cast<double>(velocity_sum_right_) / denom;
  const double avg_flow = (left_flow + right_flow) / (S.hyper.lane_count);

  long gather_cnt = total_ticks_ - k_settle_ticks;
  //   const double avg_lane_change_per_site_and_time =
  //       static_cast<double>(lane_swaps_) /
  //       (S.hyper.lane_count * S.hyper.road_length * gather_cnt);
  const double avg_lane_change_per_site_and_time =
      static_cast<double>(lane_swaps_) / (S.hyper.road_length * gather_cnt);
  const double avg_lane_changes_per_site_and_timestep_and_density =
      avg_lane_change_per_site_and_time / S.hyper.density;
  std::printf("%f, %f, %f, %f, %f\n", left_flow, right_flow, avg_flow,
              avg_lane_change_per_site_and_time,
              avg_lane_changes_per_site_and_timestep_and_density);
}

// Close data gatherer and clean up
void DataGatherer::close()
{
  if (left_pbm_)
  {
    std::fclose(left_pbm_);
    left_pbm_ = nullptr;
  }
  if (right_pbm_)
  {
    std::fclose(right_pbm_);
    right_pbm_ = nullptr;
  }
  pbm_open_ = false;
  total_ticks_ = 0;
  tick_index_ = 0;
  written_rows_ = 0;
  pbm_width_ = 0;
  pbm_height_ = 0;
  flow_samples_ = 0;
  velocity_sum_left_ = 0;
  velocity_sum_right_ = 0;
}

// Singleton instance
DataGatherer &get_data_gatherer()
{
  static DataGatherer instance;
  return instance;
}
