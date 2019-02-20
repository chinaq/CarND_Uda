#include "path.h"
#include <iostream>
#include "vehicle.h"
#include <math.h>
#include <map>
#include <string>
#include <iterator>
#include "spline.h"
#include "json.hpp"
#include "help.h"



CarPath::CarPath(Vehicle &vehicle) {
    this->vehicle = &vehicle;
}

CarPath::~CarPath() {};


void CarPath::update_the_car(json &j){
    Car* the_car = vehicle->the_car;
    the_car->x = j[1]["x"];
    the_car->y = j[1]["y"];
    the_car->s = j[1]["s"];
    the_car->d = j[1]["d"];
    the_car->yaw = j[1]["yaw"];
    the_car->speed = j[1]["speed"];
};


void CarPath::update_pre_path(json &j) {
    PreviousPath* prev_path = vehicle->prev_path;
    prev_path->xs = j[1]["previous_path_x"];
    prev_path->ys = j[1]["previous_path_y"];
    prev_path->end_s = j[1]["end_path_s"];
    prev_path->end_d = j[1]["end_path_d"];
};



void CarPath::update_ref(json &j) {

    Car &the_car = *vehicle->the_car;
    PreviousPath &prev_path = *vehicle->prev_path;

    double ref_x = the_car.x;
    double ref_y = the_car.y;
    double ref_yaw = deg2rad(the_car.yaw);

    vector<double> ptsx;
    vector<double> ptsy;
    int prev_size = prev_path.xs.size();
    cout << "prev_size: " << prev_size << endl;
    if(prev_size < 2) {
        double prev_car_x = the_car.x - cos(the_car.yaw);
        double prev_car_y = the_car.y - sin(the_car.yaw);
        ptsx.push_back(prev_car_x);
        ptsx.push_back(the_car.x);
        ptsy.push_back(prev_car_y);
        ptsy.push_back(the_car.y);
        cout << "prev_car_x: " << prev_car_x << endl;
    } else {
        ref_x = prev_path.xs[prev_size-1];
        ref_y = prev_path.ys[prev_size-1];
        double ref_x_prev = prev_path.xs[prev_size-2];
        double ref_y_prev = prev_path.ys[prev_size-2];
        ref_yaw = atan2(ref_y-ref_y_prev, ref_x-ref_x_prev);
        ptsx.push_back(ref_x_prev);
        ptsx.push_back(ref_x);
        ptsy.push_back(ref_y_prev);
        ptsy.push_back(ref_y);
    }

    vehicle->ref->x = ref_x;
    vehicle->ref->y = ref_y;
    vehicle->ref->yaw = ref_yaw;
    vehicle->ref->ptsx = ptsx;
    vehicle->ref->ptsy = ptsy;
};


void CarPath::update_sensor_fusion(json &j){
    vehicle->sensor_fusion = &j[1]["sensor_fusion"];
};


vector<vector<double>> CarPath::plan(json &j){
    // cout << "path plan" << endl;
    update_the_car(j);
    update_pre_path(j);
    update_ref(j);
    update_sensor_fusion(j);


    vehicle->gene_trajectories();
    // for (vector<Trajectory>::iterator it = trajectories.begin(); it != trajectories.end(); it++) {
    //     cost = calcluate_cost(*it);
    //     costs.push_back(cost);
    // }
    // vecotr<float>::iterator best_cost = min_element(begin(costs), end(costs));
    // int best_idx = distance(begin(costs), best_cost);
    // pts = trajectories[best_idx].gene_points();

    vehicle->set_best_trajectory(0);
    auto pts = vehicle->gene_points();
    // vector<double> ptsx = pts[0];
    // vector<double> ptsy = pts[1];
    // return {ptsx, ptsy};
    return pts;
};


