#include "simulation.hpp"
#include "state.hpp"
#include "types.hpp"
#include "data_gathering.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void print_usage(const char *prog)
{
  std::printf("Usage: %s [options]\n", prog);
  std::printf("Options:\n");
  std::printf("  --max-velocity N        (int, default 3)\n");
  std::printf("  --ticks N               (int, default 5)\n");
  std::printf("  --density F             (float 0..1, default 0.1)\n");
  std::printf("  --slowdown-prob F       (float 0..1, default 0.0)\n");
  std::printf("  --switch-prob F         (float 0..1, default 1.0)\n");
  std::printf("  --symmetric 0|1         (bool, default 0)\n");
  std::printf("  --road-length N         (int, default 100)\n");
  std::printf("  --lane-count N          (int, default 2)\n");
  // todo update for lookback-zero and position-time-data and csv output
}

int main(int argc, char **argv)
{
  int road_length = 100;
  int ticks = 5;
  SimulationState S{.hyper = {
                        .max_velocity = 3,
                        .slowdown_probability = 0.00f,
                        .lane_count = 2,
                        .road_length = road_length,
                        .density = 0.1f,
                        .switch_probability = 1.0f,
                        .symmetric = false,
                        .position_time_data = false,
                        .lookback_zero = false,
                        .csv_output = false},
                    .roads = {nullptr, nullptr},
                    .roads_copy = {nullptr, nullptr},
                    .road_storage = {nullptr, nullptr},
                    .road_copy_storage = {nullptr, nullptr}};

  // Parse CLI args
  for (int i = 1; i < argc; ++i)
  {
    if (std::strcmp(argv[i], "--max-velocity") == 0 && i + 1 < argc)
    {
      S.hyper.max_velocity = std::atoi(argv[++i]);
    }
    else if (std::strcmp(argv[i], "--ticks") == 0 && i + 1 < argc)
    {
      ticks = std::atoi(argv[++i]);
    }
    else if (std::strcmp(argv[i], "--density") == 0 && i + 1 < argc)
    {
      S.hyper.density = std::atof(argv[++i]);
    }
    else if (std::strcmp(argv[i], "--slowdown-prob") == 0 && i + 1 < argc)
    {
      S.hyper.slowdown_probability = static_cast<float>(std::atof(argv[++i]));
    }
    else if (std::strcmp(argv[i], "--switch-prob") == 0 && i + 1 < argc)
    {
      S.hyper.switch_probability = static_cast<float>(std::atof(argv[++i]));
    }
    else if (std::strcmp(argv[i], "--symmetric") == 0 && i + 1 < argc)
    {
      S.hyper.symmetric = std::atoi(argv[++i]) != 0;
    }
    else if (std::strcmp(argv[i], "--position-time-data") == 0 && i + 1 < argc)
    {
      S.hyper.position_time_data = std::atoi(argv[++i]) != 0;
    }
    else if (std::strcmp(argv[i], "--lookback-zero") == 0 && i + 1 < argc)
    {
      S.hyper.lookback_zero = std::atoi(argv[++i]) != 0;
    }
    else if (std::strcmp(argv[i], "--csv-output") == 0 && i + 1 < argc)
    {
      S.hyper.csv_output = std::atoi(argv[++i]) != 0;
    }
    else if (std::strcmp(argv[i], "--road-length") == 0 && i + 1 < argc)
    {
      road_length = std::atoi(argv[++i]);
      S.hyper.road_length = road_length;
    }
    else if (std::strcmp(argv[i], "--lane-count") == 0 && i + 1 < argc)
    {
      S.hyper.lane_count = std::atoi(argv[++i]);
    }
    else if (std::strcmp(argv[i], "--help") == 0 ||
             std::strcmp(argv[i], "-h") == 0)
    {
      print_usage(argv[0]);
      return 0;
    }
    else
    {
      std::printf("Unknown or incomplete option: %s\n", argv[i]);
      print_usage(argv[0]);
      return 1;
    }
  }

  // Basic validation
  if (S.hyper.lane_count < 1)
  {
    S.hyper.lane_count = 1;
  }
  if (S.hyper.road_length < 1)
  {
    S.hyper.road_length = 1;
  }
  if (S.hyper.max_velocity < 0)
  {
    S.hyper.max_velocity = 0;
  }
  if (ticks < 0)
  {
    ticks = 0;
  }
  if (S.hyper.density < 0.0f)
  {
    S.hyper.density = 0.0f;
  }
  if (S.hyper.density > 1.0f)
  {
    S.hyper.density = 1.0f;
  }
  if (S.hyper.slowdown_probability < 0.0f)
  {
    S.hyper.slowdown_probability = 0.0f;
  }
  if (S.hyper.slowdown_probability > 1.0f)
  {
    S.hyper.slowdown_probability = 1.0f;
  }
  if (S.hyper.switch_probability < 0.0f)
  {
    S.hyper.switch_probability = 0.0f;
  }
  if (S.hyper.switch_probability > 1.0f)
  {
    S.hyper.switch_probability = 1.0f;
  }

  init_state(S);
  populate_cars(S);
  gather_init(S, ticks);

  for (int tick = 0; tick < ticks; ++tick)
  {
    // print_state(S);
    simulation_tick(S);
    gather_data(S);
  }
  calculate_flow(S);
  gather_close();

  return 0;
}
