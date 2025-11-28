typedef struct car {
    int id;
    int lane;
    int position;
    int velocity;
} Car;
    
typedef struct line_switch_params{
    int look_ahead_distance;
    int other_look_ahead_distance;
    int other_look_behind_distance;
    float switch_probability;
} LineSwitchParams;

typedef struct hyperparameters {
    int max_velocity;
    float slowdown_probability;
    int lane_count;
    int road_length;
    float density;
    LineSwitchParams line_switch_params;
} Hyperparameters;