#ifndef TRAJECTORY_H
#define TRAJECTORY_H



enum State {
  PLCL,     // prepare for lane change left
  PLCR,     // prepare for lane change right
  LCL,      // lane change left
  LCR,      // lane change right
  LK        // lane keep
};



class Trajectory {
public:
    int goal_lane;
    int intended_lane;
    int final_lane;
    double distance_to_goal;

    double target_speed;
    double intended_speed; 
    double final_speed;
    State target_state;
};
#endif