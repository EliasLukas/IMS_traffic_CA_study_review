#include "data_gathering.hpp"
#include "types.hpp"
#include <algorithm>
#include <cstdio>
#include <vector>

// PBM outputs
static FILE *g_left_pbm = nullptr;
static FILE *g_right_pbm = nullptr;
static bool g_pbm_open = false;

// Dimensions and counters
static int g_total_ticks = 0; // ticks configured at init
static int g_tick_index =
    0; // how many times gather_data was called (router tick)
static int g_written_rows = 0; // rows written to PBMs so far
static int g_pbm_width = 0;    // min(road_length, 400)
static int g_pbm_height = 0;   // total_ticks - settle
static int velocity_sum_left = 0;
static int velocity_sum_right = 0;
static int lane_swaps = 0;
static int g_flow_samples = 0; // number of times sum_for_flow was recorded
// Settling period before we start writing PBM rows
static const int k_settle_ticks = 1000;

// Open PBM images once, writing headers with known width/height
static void open_pbm_once(int width, int height) {
  if (g_pbm_open)
    return;

  g_left_pbm = std::fopen("left_lane.pbm", "w");
  g_right_pbm = std::fopen("right_lane.pbm", "w");
  if (!g_left_pbm || !g_right_pbm) {
    if (g_left_pbm) {
      std::fclose(g_left_pbm);
      g_left_pbm = nullptr;
    }
    if (g_right_pbm) {
      std::fclose(g_right_pbm);
      g_right_pbm = nullptr;
    }
    g_pbm_open = false;
    return;
  }

  // PBM ASCII header (P1), width/height known up-front
  std::fprintf(g_left_pbm, "P1\n%d %d\n", width, height);
  std::fprintf(g_right_pbm, "P1\n%d %d\n", width, height);

  g_pbm_open = true;
}

// Write one PBM row: for each car, paint a segment from (pos - (vel - 1))..pos
static void write_lane_pbm_row(FILE *f, Car **lane, int width) {
  // Build a row of '0'/'1'
  std::vector<char> row(width, '0');

  for (int pos = 0; pos < width; ++pos) {
    Car *c = lane[pos];
    if (!c)
      continue;

    const int vel = c->velocity;
    // Segment length equals vel (vel==0 -> single pixel at pos)
    int start = pos - (vel > 0 ? (vel - 1) : 0);
    if (start < 0)
      start = 0;

    for (int i = start; i <= pos; ++i) {
      row[i] = '1';
    }
  }

  // Emit ASCII PBM row with spaces
  for (int x = 0; x < width; ++x) {
    std::fputc(row[x], f);
    if (x + 1 < width)
      std::fputc(' ', f);
  }
  std::fputc('\n', f);
}

// Specific gatherer: position-time PBM writer with settling
static void gather_position_time_data(const SimulationState &S) {
  // Lazily open PBM files when we’re ready to write
  if (!g_pbm_open) {
    g_pbm_width = std::min(S.hyper.road_length, 400);
    g_pbm_height = std::max(0, g_total_ticks - k_settle_ticks);
    if (g_pbm_height <= 0)
      return; // nothing to write for too-short runs
    open_pbm_once(g_pbm_width, g_pbm_height);
  }

  if (!g_pbm_open)
    return;
  if (g_written_rows >= g_pbm_height)
    return;

  // lane[0] -> left_lane.pbm, lane[1] -> right_lane.pbm
  if (S.hyper.lane_count >= 1 && g_left_pbm) {
    write_lane_pbm_row(g_left_pbm, S.roads[0], g_pbm_width);
    std::fflush(g_left_pbm);
  }
  if (S.hyper.lane_count >= 2 && g_right_pbm) {
    write_lane_pbm_row(g_right_pbm, S.roads[1], g_pbm_width);
    std::fflush(g_right_pbm);
  }

  ++g_written_rows;
}

void gather_init(const SimulationState &S, int total_ticks) {
  g_total_ticks = total_ticks;
  g_tick_index = 0;
  g_written_rows = 0;
  g_flow_samples = 0;
  velocity_sum_left = 0;
  velocity_sum_right = 0;
  lane_swaps = 0;

  // Don’t open PBMs here; we open lazily after settling with known height.
  g_pbm_open = false;
  g_left_pbm = nullptr;
  g_right_pbm = nullptr;

  g_pbm_width = std::min(S.hyper.road_length, 400);
  g_pbm_height = std::max(0, total_ticks - k_settle_ticks);
}

void sum_for_flow(const SimulationState &S) {
  const int width = S.hyper.road_length;

  // Sum velocities for left lane (index 0)
  if (S.hyper.lane_count >= 1) {
    Car **lane0 = S.roads[0];
    for (int pos = 0; pos < width; ++pos) {
      Car *c = lane0[pos];
      if (c) {
        velocity_sum_left += c->velocity;
      }
    }
  }

  // Sum velocities for right lane (index 1)
  if (S.hyper.lane_count >= 2) {
    Car **lane1 = S.roads[1];
    for (int pos = 0; pos < width; ++pos) {
      Car *c = lane1[pos];
      if (c) {
        velocity_sum_right += c->velocity;
      }
    }
  }

  // Count this sampling event
  ++g_flow_samples;
}

// Router: call specific data gathering based on run arguments (flags in
// S.hyper)
void gather_data(const SimulationState &S) {
  // One call per simulation tick
  ++g_tick_index;

  if (g_tick_index < k_settle_ticks)
    return;

  if (S.hyper.position_time_data) {
    gather_position_time_data(S);
  }
  if (S.hyper.csv_output) {
    // statistics for flow gathered every fifth step only
    if (g_tick_index % 5 == 0) {
      sum_for_flow(S);
    }
  }

  // Add more modes here later:
  // if (S.hyper.some_other_flag) { gather_other_mode(S); }
}

void register_event_lane_swap() {
  if (g_tick_index >= k_settle_ticks)
    lane_swaps++;
}

void calculate_flow(const SimulationState &S) {
  if (!S.hyper.csv_output)
    return;
  if (g_flow_samples == 0) {
    std::printf("0.000, 0.000, 0.000,\n");
    return;
  }

  // flow stats
  const double denom = static_cast<double>(g_flow_samples) *
                       static_cast<double>(S.hyper.road_length);
  const double left_flow = static_cast<double>(velocity_sum_left) / denom;
  const double right_flow = static_cast<double>(velocity_sum_right) / denom;
  const double avg_flow = (left_flow + right_flow) / (S.hyper.lane_count);

  // lane changes per site and time step stats

  long gather_cnt = g_total_ticks - k_settle_ticks;
  const double avg_lane_change_per_site_and_time =
      static_cast<double>(lane_swaps) /
      (S.hyper.lane_count * S.hyper.road_length * gather_cnt);

  const double avg_lane_changes_per_site_and_timestep_and_density =
      avg_lane_change_per_site_and_time / S.hyper.density;
  const double avg_changes_per_car =
      static_cast<double>(lane_swaps) /
      (S.hyper.density * S.hyper.lane_count * S.hyper.road_length * gather_cnt);

  std::printf("%f, %f, %f, %f, %f, %f,\n", left_flow, right_flow, avg_flow,
              avg_lane_change_per_site_and_time,
              avg_lane_changes_per_site_and_timestep_and_density,
              avg_changes_per_car);
}

void gather_close() {
  if (g_left_pbm) {
    std::fclose(g_left_pbm);
    g_left_pbm = nullptr;
  }
  if (g_right_pbm) {
    std::fclose(g_right_pbm);
    g_right_pbm = nullptr;
  }
  g_pbm_open = false;

  g_total_ticks = 0;
  g_tick_index = 0;
  g_written_rows = 0;
  g_pbm_width = 0;
  g_pbm_height = 0;
  g_flow_samples = 0;
  velocity_sum_left = 0;
  velocity_sum_right = 0;
}

// Also close on program exit if not closed explicitly
struct GatherCloseGuard {
  ~GatherCloseGuard() { gather_close(); }
};
static GatherCloseGuard g_guard;