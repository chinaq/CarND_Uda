#include <algorithm>
#include <iostream>
#include "vehicle.h"
#include <cmath>
#include <map>
#include <string>
#include <iterator>
#include "help.h"
#include "trajectory.h"
// #include "cost.h"

MapWaypoints::MapWaypoints(
        vector<double> &map_waypoints_x,
        vector<double> &map_waypoints_y,
        vector<double> &map_waypoints_s,
        vector<double> &map_waypoints_dx,
        vector<double> &map_waypoints_dy) {
    this->x = map_waypoints_x;
    this->y = map_waypoints_y;
    this->s = map_waypoints_s;
    this->dx = map_waypoints_dx;
    this->dy = map_waypoints_dy;
};





// Vehicle::Vehicle() : Vehicle(1, 2, 0.0, 49.5, LK) {};
Vehicle::Vehicle(MapWaypoints &waypoints, Settings &settings) {
    this->map_points = &waypoints;
    this->settings = &settings;

    this->the_car = new Car();
    this->prev_path = new PreviousPath();
    this->ref = new Ref();
    this->sensor_fusion = new json();
};


Vehicle::~Vehicle() {}



void Vehicle::update_data(Car &the_car, PreviousPath &prev_path, Ref &ref, json &sensor_fusion) {
    this->the_car = &the_car;
    this->prev_path = &prev_path;
    this->ref = &ref;
    this->sensor_fusion = &sensor_fusion;
}


State Vehicle::get_state() {
    return this->settings->state;
}


vector<State> Vehicle::successor_states() {
    vector<State> states;
    states.push_back(LK);
    State state = get_state();
    if(state == LK) {
        states.push_back(PLCL);
        states.push_back(PLCR);
    } else if (state == PLCR) {
        if (settings->lane < settings->lane_max) {
            states.push_back(PLCR);
            states.push_back(LCR);
        }
    } else if (state == PLCL) {
        if (settings->lane > 0) {
            states.push_back(PLCL);
            states.push_back(LCL);
        }
    }
    //If state is "LCL" or "LCR", then just return "KL"
    return states;
}


void Vehicle::gene_trajectories() {
    // cout << "vehicle::gene_trajectories" << endl;
    auto states = successor_states();
    get_lane_speeds();
    trajectories.clear();
    for (vector<State>::iterator it = states.begin(); it != states.end(); it++) {
        // if (state == LK) {
            trajectories.push_back(keep_lane_trajectory(settings->state));
        // } else if (state == LCL || state == LCR) {
        //     trajectories.push_back(lane_change_trajectory(state));
        // } else if (state == PLCL || state == PLCR) {
        //     trajectories.push(prep_lane_change_trajectory(state));
        // }
    }
}


void Vehicle::set_best_trajectory(int i) {
    // cout << "vehicle::set_best_trajectory" << endl;
    this->best_trajectory = this->trajectories[i];
}

vector<vector<double>> Vehicle::gene_points() {

        // if (d < (2+4*vehicle->lane+2) && d > (2+4*vehicle->lane-2)) {
        //     double vx = sensor_fusion[i][3];
        //     double vy = sensor_fusion[i][4];
        //     double check_speed = sqrt(vx*vx + vy*vy);
        //     double check_car_s = sensor_fusion[i][5];
        //     // when i reached final prev point where was the tracked vehicle
        //     check_car_s += (double)prev_size*.02*check_speed;
        //     // if the traced vehecle would be in front of me
        //     // but smaller than 30m
        //     if (check_car_s > car_s && (check_car_s-car_s) < 30) {
        //         // ref_vel = 29.5;
        //         too_close = true;
        //         if (vehicle->lane > 0) {
        //             vehicle->lane = 0;
        //         }
        //     }
        // }

    if (settings->too_close[best_trajectory.final_lane] 
        || settings->v > settings->speed_max) {
        settings->v -= .224;
        cout << "final lane: " << best_trajectory.final_lane << endl;
        cout << "too close" << endl;
    } else {
        settings->v += .224;
    }
    cout << "intended_speed: " << best_trajectory.intended_speed << endl;
    cout << "settings->v: " << settings->v << endl;

    settings->lane = best_trajectory.final_lane;
    settings->state = best_trajectory.target_state;
    cout << "settings lane: " << settings->lane << endl;
    cout << "settings state: " << settings->state << endl;
    // cout << "vehicle::gene_points" << endl;

    vector<double> ptsx = ref->ptsx;
    vector<double> ptsy = ref->ptsy;
    cout << "ptsx size before: " << ptsx.size() << endl;
    // copy (ptsx.begin(), ptsx.end(), ostream_iterator<double> (cout, ","));

    double lane_val = 2+4*settings->lane;
    vector<double> next_wp0 = getXY(the_car->s+30, lane_val, map_points->s, map_points->x, map_points->y);
    vector<double> next_wp1 = getXY(the_car->s+60, lane_val, map_points->s, map_points->x, map_points->y);
    vector<double> next_wp2 = getXY(the_car->s+90, lane_val, map_points->s, map_points->x, map_points->y);
    ptsx.push_back(next_wp0[0]);
    ptsx.push_back(next_wp1[0]);
    ptsx.push_back(next_wp2[0]);
    ptsy.push_back(next_wp0[1]);
    ptsy.push_back(next_wp1[1]);
    ptsy.push_back(next_wp2[1]);

    // to car coordinate

    // to car coordinate
    for (int i = 0; i < ptsx.size(); i++) {
        double shift_x = ptsx[i] - ref->x;
        double shift_y = ptsy[i] - ref->y;
        ptsx[i] = shift_x*cos(-ref->yaw) - shift_y*sin(-ref->yaw);
        ptsy[i] = shift_x*sin(-ref->yaw) + shift_y*cos(-ref->yaw);
    }

    // spline
    tk::spline s;
    s.set_points(ptsx, ptsy);

    vector<double> next_x_vals;
    vector<double> next_y_vals;
    // use remaining points of previousness
    cout << "prev paht xs size: " << prev_path->xs.size() << endl;
    for (int i = 0; i < prev_path->xs.size(); i++) {
        next_x_vals.push_back(prev_path->xs[i]);
        next_y_vals.push_back(prev_path->ys[i]);
    }

    // for new points which is 30m away from ref_x
    double target_x = 30.0;
    double target_y = s(target_x);
    double target_dist = sqrt(target_x*target_x + target_y*target_y);
    double x_add_on = 0;

    // set new points by ref_vel
    for (int i = 1; i <= 50-prev_path->xs.size(); i++) {
        double N = target_dist / (.02*settings->v/2.24);
        cout << settings->v << endl;
        double x_point = x_add_on + target_x/N;
        double y_point = s(x_point);
        x_add_on = x_point;

        // back to global coordiate
        double x_ref = x_point;
        double y_ref = y_point;
        x_point = x_ref*cos(ref->yaw) - y_ref*sin(ref->yaw);
        y_point = x_ref*sin(ref->yaw) + y_ref*cos(ref->yaw);
        x_point += ref->x;
        y_point += ref->y;
        next_x_vals.push_back(x_point);
        next_y_vals.push_back(y_point);
    }
    return {next_x_vals, next_y_vals};
}




Trajectory Vehicle::keep_lane_trajectory(State state) {
    // cout << "vehicle::keep_lane_trajectory" << endl;
    Trajectory trajectory;
    int lane = settings->lane;
    // cout << "lane: " << lane << endl;

    trajectory.goal_lane = lane;
    trajectory.intended_lane = lane;
    trajectory.final_lane = lane;
    trajectory.distance_to_goal = 30;

    trajectory.target_speed = settings->speed_max;
    trajectory.intended_speed = settings->lane_speeds[lane];
    // cout << "intended speed: " << trajectory.intended_speed << endl;
    trajectory.final_speed = settings->lane_speeds[lane];
    trajectory.target_state = state;
    return trajectory;
}


void Vehicle::get_lane_speeds() {
    // for (int i = 0; i < sensor_fusion->size(); i++) {
    double speed_max = settings->speed_max;
    settings->lane_speeds[0] = speed_max;
    settings->lane_speeds[1] = speed_max;
    settings->lane_speeds[2] = speed_max;

    settings->too_close[0] = false;
    settings->too_close[1] = false;
    settings->too_close[2] = false;

    for (int i = 0; i < (*sensor_fusion).size(); i++) {
        float d = (*sensor_fusion)[i][6];
        // if the tracked car kept in lane
        // cout << "get_lane_speeds: d: " << d << endl;
        // int ln = (int)d / 4;
        int ln = get_lane(d);

        if (ln == settings->lane) {
            settings->lane_speeds[ln] = min(settings->lane_speeds[ln], current_lane_speed(ln, i));
        } else if (ln+1 == settings->lane || ln-1 == settings->lane) {
            settings->lane_speeds[ln] = min(settings->lane_speeds[ln], next_lane_speed(ln, i));
        }
        // cout << "sensor " << i << ", lane " << ln << ", speed " << settings->lane_speeds[ln] << endl; 
    }
}



int Vehicle::get_lane(double d) {
    int ln;
    if (d < 0) 
        ln = -999;
    else if (d < 4)
        ln = 0;
    else if (d < 8)
        ln = 1;
    else if (d < 12)
        ln = 2;
    else
        ln = 999;
    return ln;
}



double Vehicle::current_lane_speed(int lane, int sensor_i) {

    // if the traced vehecle would be in front of me
    // but smaller than 30m
    int i = sensor_i;
    auto speed_distance = check_lane_speed(lane, i);
    auto check_speed = speed_distance[0];
    auto distance = speed_distance[1];
    // settings->too_close[i] = false;
    if (distance > 0 && distance < 30) {
        settings->too_close[i] = true;
        return check_speed;
    }
    return settings->speed_max;
}

double Vehicle::next_lane_speed(int lane, int sensor_i) {
    int i = sensor_i;
    auto speed_distance = check_lane_speed(lane, i);
    auto check_speed = speed_distance[0];
    auto distance = speed_distance[1];
    // settings->too_close[i] = false;
    if (distance < 30 && distance >-30) {
        settings->too_close[i] = true;
        return check_speed;
    }
    return settings->speed_max;
}


vector<double> Vehicle::check_lane_speed(int lane, int sensor_i) {
    // cout << "check lane speed" << endl;
    int i = sensor_i;
    json &sensor = *sensor_fusion;
    double vx = sensor[i][3];
    double vy = sensor[i][4];
    double check_speed = sqrt(vx*vx + vy*vy);
    double check_car_s = sensor[i][5];
    check_car_s += (double)prev_path->xs.size()*.02*check_speed;
    double distance = check_car_s-the_car->s;
    return {check_speed, distance};
}
