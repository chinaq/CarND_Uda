#ifndef HELP_H
#define HELP_H
#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <string>
#include "json.hpp"

using namespace std;

// namespace{

// for convenience
using json = nlohmann::json;
// For converting back and forth between radians and degrees.
constexpr double pi();
double deg2rad(double x);
double rad2deg(double x);
// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s);
double distance(double x1, double y1, double x2, double y2);
int ClosestWaypoint(double x, double y, const vector<double> &maps_x, const vector<double> &maps_y);
int NextWaypoint(double x, double y, double theta, const vector<double> &maps_x, const vector<double> &maps_y);

vector<double> getFrenet(double x, double y, double theta, const vector<double> &maps_x, const vector<double> &maps_y);
vector<double> getXY(double s, double d, const vector<double> &maps_s, const vector<double> &maps_x, const vector<double> &maps_y);

vector<double> local_to_global(double x_local, double y_local, double ref_x, double ref_y, double ref_yaw);
vector<double> global_to_local(double x_global, double y_global, double ref_x, double ref_y, double ref_yaw);

// }

#endif