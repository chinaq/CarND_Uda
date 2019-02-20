#ifndef VEHICLE_H
#define VEHICLE_H
#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <string>
#include "json.hpp"
#include "help.h"
#include "trajectory.h"
#include "spline.h"

using namespace std;



class Car {
public:
  double x;
  double y;
  double s;
  double d;
  double yaw;
  double speed;
};

class MapWaypoints {
  public:
  vector<double> x;
  vector<double> y;
  vector<double> s;
  vector<double> dx;
  vector<double> dy;
  MapWaypoints(vector<double> &map_waypoints_x,
        vector<double> &map_waypoints_y,
        vector<double> &map_waypoints_s,
        vector<double> &map_waypoints_dx,
        vector<double> &map_waypoints_dy);
};

class PreviousPath {
public:
  json xs;
  json ys;
  json end_s;
  json end_d;
};

class Ref {
  public:
    double x;
    double y;
    double yaw;
    vector<double> ptsx;
    vector<double> ptsy;
};

class Settings {
public:
  State state=LK;
  int lane=1;
  int lane_max=2;
  double speed_max=49.5;
  double v=0.0;
  // double v=49.5;
  double lane_speeds[3] = {speed_max, speed_max, speed_max};
  bool too_close[3] = {false, false, false};
};


class Vehicle {
public:
  MapWaypoints* map_points;
  Settings* settings;

  Car* the_car;
  PreviousPath* prev_path;
  Ref* ref;
  json* sensor_fusion;

  Trajectory best_trajectory;
  vector<Trajectory> trajectories;

  Vehicle(MapWaypoints &map_points, Settings &settings);
  virtual ~Vehicle();

  void update_data(Car &the_car, PreviousPath &prev_path, Ref &ref, json &sensor_fusion);
  State get_state();
  void update(json &j);
  vector<State> successor_states();
  // std::tuple<vector<double>, vector<double>> keep_lane_trajectory();
  void get_lane_speeds();
  void gene_trajectories();
  Trajectory keep_lane_trajectory(State state);
  void set_best_trajectory(int i);
  vector<vector<double>> gene_points();

private:
  double current_lane_speed(int lane, int sensor_i);
  double next_lane_speed(int lane, int sensor_i);
  vector<double> check_lane_speed(int lane, int sensor);
  int get_lane(double d);
};

#endif
