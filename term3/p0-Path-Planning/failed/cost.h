#include <functional>
#include <iterator>
#include <map>
#include <math.h>

float goal_distance_cost(int goal_lane, 
                        int intended_lane, 
                        int final_lane, 
                        float distance_to_goal);

float inefficiency_cost(int target_speed, 
                        int intended_lane, 
                        int final_lane, 
                        vector<int> lane_speeds);