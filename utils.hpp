#include <cstdio>

typedef struct car {
    int id;
    int lane;
    int position;
    int velocity;
} Car;

typedef struct hyperparameters {
    int max_velocity;
    float slowdown_probability;
    int lane_count;
    int road_length;
    float density;
    float switch_probability;
} Hyperparameters;