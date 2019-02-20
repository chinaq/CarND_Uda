#include <fstream>
#include <math.h>
#include <uWS/uWS.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm> 
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "json.hpp"
#include "spline.h"
#include "help.h"

using namespace std;

// for convenience
using json = nlohmann::json;



// int main() {
int main(int argc, char** argv) {
  uWS::Hub h;

  // Load up map values for waypoint's x,y,s and d normalized normal vectors
  vector<double> map_waypoints_x;
  vector<double> map_waypoints_y;
  vector<double> map_waypoints_s;
  vector<double> map_waypoints_dx;
  vector<double> map_waypoints_dy;

  // Waypoint map to read from
    string path = "";
    // Get the path from a full path and filename
    // https://www.oreilly.com/library/view/c-cookbook/0596007612/ch10s16.html
    char sep = '/';
    string argv0 = argv[0];
    size_t i = argv0.rfind(sep, argv0.length());
    if (i != string::npos) {
        path = argv0.substr(0, i);
    }
  string map_file_ = path + "/../data/highway_map.csv";
    std::cout << map_file_ << std::endl;

  // The max s value before wrapping around the track back to 0
  double max_s = 6945.554;
  ifstream in_map_(map_file_.c_str(), ifstream::in);
  string line;
  while (getline(in_map_, line)) {
      istringstream iss(line);
      double x;
      double y;
      float s;
      float d_x;
      float d_y;
      iss >> x;
      iss >> y;
      iss >> s;
      iss >> d_x;
      iss >> d_y;
      map_waypoints_x.push_back(x);
      map_waypoints_y.push_back(y);
      map_waypoints_s.push_back(s);
      map_waypoints_dx.push_back(d_x);
      map_waypoints_dy.push_back(d_y);
  }


    ///////////////////// move smoothly ///////////////
    // start in lane 1
    int lane = 1;
    // move a reference velocity to target
    // double ref_vel = 49.5;      // mph
    double ref_vel = 0.0;      // mph
    //////////////////////////////////////////////////



  h.onMessage([&map_waypoints_x,&map_waypoints_y,&map_waypoints_s,&map_waypoints_dx,&map_waypoints_dy,&lane,&ref_vel](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    //auto sdata = string(data).substr(0, length);
    //cout << sdata << endl;
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(data);
      if (s != "") {
        auto j = json::parse(s);
        string event = j[0].get<string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
            // Main car's localization Data
                    double car_x = j[1]["x"];
                    double car_y = j[1]["y"];
                    double car_s = j[1]["s"];
                    double car_d = j[1]["d"];
                    double car_yaw = j[1]["yaw"];
                    double car_speed = j[1]["speed"];

                    // Previous path data given to the Planner
                    auto previous_path_x = j[1]["previous_path_x"];
                    auto previous_path_y = j[1]["previous_path_y"];
                    // Previous path's end s and d values 
                    double end_path_s = j[1]["end_path_s"];
                    double end_path_d = j[1]["end_path_d"];
                    // Sensor Fusion Data, a list of all other cars on the same side of the road.
                    auto sensor_fusion = j[1]["sensor_fusion"];
                    json msgJson;



                    // TODO: define a path made up of (x,y) points that the car will visit sequentially every .02 seconds

                    ///////////////////////// circle /////////////////
                    // double pos_x;
                    // double pos_y;
                    // double angle;
                    // int path_size = previous_path_x.size();
                    // for(int i = 0; i < path_size; i++) {
                    // 		next_x_vals.push_back(previous_path_x[i]);
                    // 		next_y_vals.push_back(previous_path_y[i]);
                    // }
                    // if(path_size == 0) {
                    // 		pos_x = car_x;
                    // 		pos_y = car_y;
                    // 		angle = deg2rad(car_yaw);
                    // } else {
                    // 		pos_x = previous_path_x[path_size-1];
                    // 		pos_y = previous_path_y[path_size-1];
                    // 		double pos_x2 = previous_path_x[path_size-2];
                    // 		double pos_y2 = previous_path_y[path_size-2];
                    // 		angle = atan2(pos_y-pos_y2,pos_x-pos_x2);
                    // }
                    // double dist_inc = 0.5;
                    // for(int i = 0; i < 50-path_size; i++) {    
                    // 		next_x_vals.push_back(pos_x+(dist_inc)*cos(angle+(i+1)*(pi()/100)));
                    // 		next_y_vals.push_back(pos_y+(dist_inc)*sin(angle+(i+1)*(pi()/100)));
                    // 		pos_x += (dist_inc)*cos(angle+(i+1)*(pi()/100));
                    // 		pos_y += (dist_inc)*sin(angle+(i+1)*(pi()/100));
                    // }
                    /////////////////////// end circle /////////////


                    //////////////  keep track /////////////////
                    // double dist_inc = 0.5;
                    // for(int i = 0; i < 50; i++){
                    // 	double next_s = car_s + (i+1)*dist_inc;
                    // 	double next_d = 6;
                    // 	vector<double> xy = getXY(next_s, next_d, map_waypoints_s, map_waypoints_x, map_waypoints_y);

                    // 	// std::cout << "(" << xy[0] << "," << xy[1] << ")" << std::endl;
                    // 	next_x_vals.push_back(xy[0]);
                    // 	next_y_vals.push_back(xy[1]);
                    // }
                    /////////////// end keep track ////////////



                    ///////////////// move smoothly /////////////////
                    int prev_size = previous_path_x.size();

                    // to prevent a collision
                    if (prev_size > 0) {
                        car_s = end_path_s;
                    }
                    // bool too_close = false;
                    // // check for each tracked vehicle
                    // for (int i = 0; i < sensor_fusion.size(); i++) {
                    // 	float d = sensor_fusion[i][6];
                    // 	// if the tracked car kept in lane
                    // 	if (d < (2+4*lane+2) && d > (2+4*lane-2)) {
                    // 		double vx = sensor_fusion[i][3];
                    // 		double vy = sensor_fusion[i][4];
                    // 		double check_speed = sqrt(vx*vx + vy*vy);
                    // 		double check_car_s = sensor_fusion[i][5];
                    // 		// when i reached final prev point where was the tracked vehicle
                    // 		check_car_s += (double)prev_size*.02*check_speed;
                    // 		// if the traced vehecle would be in front of me
                    // 		// but smaller than 30m
                    // 		if (check_car_s > car_s && (check_car_s-car_s) < 30) {
                    // 			// ref_vel = 29.5;
                    // 			too_close = true;
                    // 			if (lane > 0) {
                    // 				lane = 0;
                    // 			}
                    // 		}
                    // 	}
                    // }

                    // // revise velocity
                    // if (too_close) {
                    // 	ref_vel -= .224;
                    // } else if (ref_vel < 49.5) {
                    // 	ref_vel += .224;
                    // }



                    double HEAD_DIS = 20;
                    double SIDE_DIS = 15;
                    double SPEED_DIS = 35;

                    bool car_ahead = false;
                    bool car_left = false;
                    bool car_right = false;
                    double lane_speeds[3] = {999, 999, 999};
                    
                    for (int i = 0; i < sensor_fusion.size(); i++) {
                        float d = sensor_fusion[i][6];
                        int car_lane = -1;
                        if (d<0) {
                            continue;
                        } else if (d < 4) {
                            car_lane = 0;
                        } else if (d < 8) {
                            car_lane = 1;
                        } else if (d <= 12) {
                            car_lane = 2;
                        } else {
                            continue;
                        }
                        double vx = sensor_fusion[i][3];
                        double vy = sensor_fusion[i][4];
                        double check_speed = sqrt(vx*vx + vy*vy);
                        double check_car_s = sensor_fusion[i][5];
                        check_car_s += ((double)prev_size*0.02*check_speed);

                        // check distance
                        double distance = check_car_s - car_s;
                        if (car_lane == lane) {
                            car_ahead |= distance > 0 && distance < HEAD_DIS;
                        } else if ( car_lane - lane == -1 ) {
                            car_left |= abs(distance) < SIDE_DIS;
                        } else if ( car_lane - lane == 1 ) {
                            car_right |= abs(distance) < SIDE_DIS;
                        }

                        // check lane speeds
                        if((car_lane == 0 || car_lane == 1 || car_lane == 2) 
                                && (distance > 0 && distance < SPEED_DIS)) {
                            lane_speeds[car_lane] = min(lane_speeds[car_lane], check_speed);
                        }
                    }

                    // set lane and speed
                    double acc = 0;
                    const double MAX_SPEED = 49.5;
                    const double MAX_ACC = .224;
                    if (car_ahead) {
                        if (lane==1 && !car_left && !car_right 
                                && (lane_speeds[0] > lane_speeds[1] || lane_speeds[2] > lane_speeds[1])) {
                            if (lane_speeds[0] >= lane_speeds[2])
                                lane--;
                            else
                                lane++;
                        } else if ( !car_left && lane > 0 && lane_speeds[lane-1] > lane_speeds[lane]) {
                            lane--;
                        } else if ( !car_right && lane != 2 && lane_speeds[lane+1] > lane_speeds[lane]){
                            lane++;
                        } else if (lane_speeds[lane] < car_speed) {
                            acc -= MAX_ACC;
                        }
                    } else if ( ref_vel < MAX_SPEED ) {
                        acc += MAX_ACC;
                    }







                    vector<double> ptsx;
                    vector<double> ptsy;
                    double ref_x = car_x;
                    double ref_y = car_y;
                    double ref_yaw = deg2rad(car_yaw);

                    // get previous 2 points
                    if(prev_size < 2) {
                        double prev_car_x = car_x - cos(car_yaw);
                        double prev_car_y = car_y - sin(car_yaw);
                        ptsx.push_back(prev_car_x);
                        ptsx.push_back(car_x);
                        ptsy.push_back(prev_car_y);
                        ptsy.push_back(car_y);
                    } else {
                        ref_x = previous_path_x[prev_size-1];
                        ref_y = previous_path_y[prev_size-1];
                        double ref_x_prev = previous_path_x[prev_size-2];
                        double ref_y_prev = previous_path_y[prev_size-2];
                        ref_yaw = atan2(ref_y-ref_y_prev, ref_x-ref_x_prev);
                        ptsx.push_back(ref_x_prev);
                        ptsx.push_back(ref_x);
                        ptsy.push_back(ref_y_prev);
                        ptsy.push_back(ref_y);
                    }

                    // new points: 30m, 60m, 90m
                    double lane_val = 2+4*lane;
                    vector<double> next_wp0 = getXY(car_s+30, lane_val, map_waypoints_s, map_waypoints_x, map_waypoints_y);
                    vector<double> next_wp1 = getXY(car_s+60, lane_val, map_waypoints_s, map_waypoints_x, map_waypoints_y);
                    vector<double> next_wp2 = getXY(car_s+90, lane_val, map_waypoints_s, map_waypoints_x, map_waypoints_y);
                    ptsx.push_back(next_wp0[0]);
                    ptsx.push_back(next_wp1[0]);
                    ptsx.push_back(next_wp2[0]);
                    ptsy.push_back(next_wp0[1]);
                    ptsy.push_back(next_wp1[1]);
                    ptsy.push_back(next_wp2[1]);

                    // to car coordinate
                    for (int i = 0; i < ptsx.size(); i++) {
                        double shift_x = ptsx[i] - ref_x;
                        double shift_y = ptsy[i] - ref_y;
                        ptsx[i] = shift_x*cos(-ref_yaw) - shift_y*sin(-ref_yaw);
                        ptsy[i] = shift_x*sin(-ref_yaw) + shift_y*cos(-ref_yaw);
                    }


                    // final points
                    vector<double> next_x_vals;
                    vector<double> next_y_vals;
                    // spline
                    tk::spline s;
                    s.set_points(ptsx, ptsy);
                    // use remaining points of previousness
                    for (int i = 0; i < previous_path_x.size(); i++) {
                        next_x_vals.push_back(previous_path_x[i]);
                        next_y_vals.push_back(previous_path_y[i]);
                    }

                    // for new points which is 30m away from ref_x
                    double target_x = 30.0;
                    double target_y = s(target_x);
                    double target_dist = sqrt(target_x*target_x + target_y*target_y);
                    double x_add_on = 0;

                    // set new points by ref_vel
                    for (int i = 1; i <= 50-previous_path_x.size(); i++) {
                        ref_vel += acc;
                        if ( ref_vel > MAX_SPEED ) {
                            ref_vel = MAX_SPEED;
                        } else if ( ref_vel < MAX_ACC ) {
                            ref_vel = MAX_ACC;
                        }
                        double N = target_dist / (.02*ref_vel/2.24);
                        double x_point = x_add_on + target_x/N;
                        double y_point = s(x_point);
                        x_add_on = x_point;

                        // back to global coordiate
                        double x_ref = x_point;
                        double y_ref = y_point;
                        x_point = x_ref*cos(ref_yaw) - y_ref*sin(ref_yaw);
                        y_point = x_ref*sin(ref_yaw) + y_ref*cos(ref_yaw);
                        x_point += ref_x;
                        y_point += ref_y;
                        next_x_vals.push_back(x_point);
                        next_y_vals.push_back(y_point);
                    }
                    ///////////////// end move smoothly /////////////

                    // Did
                    


                    msgJson["next_x"] = next_x_vals;
                    msgJson["next_y"] = next_y_vals;
                    auto msg = "42[\"control\","+ msgJson.dump()+"]";
                    //this_thread::sleep_for(chrono::milliseconds(1000));
                    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the
  // program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data,
                     size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1) {
      res->end(s.data(), s.length());
    } else {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
