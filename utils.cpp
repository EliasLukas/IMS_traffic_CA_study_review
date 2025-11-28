#include "utils.hpp"

const int ROAD_LENGTH = 300; // Use a constant for array size


// dummy data for testing
Hyperparameters hyperparameters = {
    .max_velocity = 5,
    .slowdown_probability = 0.3f,
    .lane_count = 2,
    .road_length = ROAD_LENGTH,
    .density = 0.2f,
    .switch_probability = 0.7f
};

Car *road1[ROAD_LENGTH];
Car *road2[ROAD_LENGTH];

Car **roads[] = {road1, road2};

//retuns the gap ahead in the specified lane (current_lane = true for current lane, false for other lane; look_left = true for left lane, false for right lane)
//gap is int up to max velocity
int count_gap_ahead(Car *car, bool current_lane, bool look_left){
    int lane;
    if(current_lane){
        lane = car->lane;
    } else if (look_left){
        lane = (car->lane - 1);
    } else {
        lane = (car->lane + 1);
    }

    if(lane < 0 || lane >= hyperparameters.lane_count){
        printf("Invalid lane access in count_gap_ahead: %d\n", lane);
        return -1; // invalid lane
    }

    int gap = 0;
    int position = car->position;

    Car **current_road = roads[lane];
    int road_length = hyperparameters.road_length;

    for(int i = 1; i <= hyperparameters.max_velocity; i++){
        int check_position = (position + i) % road_length;
        if(current_road[check_position] == nullptr){
            gap++;
        } else {
            break;
        }
    }

    return gap;
}


// Attention - return value -1 is valid, -1 means there is a car next to the current car
int count_gap_behind(Car *car, bool look_left){
    int lane;
    if(look_left){
        lane = (car->lane - 1);
    } else {
        lane = (car->lane + 1);
    }

    if(lane < 0 || lane >= hyperparameters.lane_count){
        printf("Invalid lane access in count_gap_behind: %d\n", lane);
        return -2; // invalid lane
    }

    int gap = -1;
    int position = car->position;

    Car **current_road = roads[lane];
    int road_length = hyperparameters.road_length;

    for(int i = 0; i <= hyperparameters.max_velocity; i++){
        int check_position = (position - i + road_length) % road_length;
        if(current_road[check_position] == nullptr){
            gap++;
        } else {
            break;
        }
    }

    return gap;
}

