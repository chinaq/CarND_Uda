#include <fstream>
#include <math.h>
#include <uWS/uWS.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "json.hpp"
#include "spline.h"
#include "vehicle.h"
#include "path.h"
#include "help.h"

using namespace std;


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


  ///////////////////// the vehicle ///////////////
  // Vehicle vehicle = Vehicle(1, 2, 0.0, State::LK);
  MapWaypoints waypoints = MapWaypoints(
    map_waypoints_x,
    map_waypoints_y,
    map_waypoints_s,
    map_waypoints_dx,
    map_waypoints_dy);
  Settings settings;
  Vehicle vehicle = Vehicle(waypoints, settings);

  CarPath car_path = CarPath(vehicle);
  CarPath* car_path_ptr = &car_path;
  //////////////////////////////////////////////////



  h.onMessage([&map_waypoints_x,&map_waypoints_y,&map_waypoints_s,
      // &map_waypoints_dx, &map_waypoints_dy,&lane,&ref_vel,&state, &car_path_ptr]
      &map_waypoints_dx, &map_waypoints_dy, &car_path_ptr]
      (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
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
            json msgJson;



            // TODO: define a path made up of (x,y) points that the car will visit sequentially every .02 seconds
            cout << "----------------" << endl;
            // cout << "main on message" << endl;
            auto pts = car_path_ptr->plan(j);
            vector<double> ptsx = pts[0];
            vector<double> ptsy = pts[1];
            //////////////// end todo ///////////////
            


            msgJson["next_x"] = ptsx;
            msgJson["next_y"] = ptsy;
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
