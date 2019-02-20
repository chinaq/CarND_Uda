#ifndef CARPATH_H
#define CARPATH_H
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iterator>
#include "vehicle.h"
#include "json.hpp"
#include "help.h"
// #include "trajectory.h"

using namespace std;

class CarPath {
public:
    Vehicle *vehicle;

    CarPath(Vehicle &vehicle);
  	virtual ~CarPath();
    vector<vector<double>> plan(json &j);
private:
    void update_the_car(json &j);
    void update_pre_path(json &j);
    void update_ref(json &j);
    void update_sensor_fusion(json &j);
};
#endif