#include <iostream>
#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
  x_ = F_*x_;
  MatrixXd Ft = F_.transpose();
	P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
	VectorXd z_pred = H_ * x_;
	VectorXd y = z - z_pred;
  Cal_X_P(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */
  float x = x_(0);
  float y = x_(1);
  float vx = x_(2);
  float vy = x_(3);

  float rho = sqrt(x*x+y*y);
  if(rho < 0.0001){
    cout << "Rho too small - Error - Division by Zero" << endl;
    rho = 0.0001;
  }
  float theta = atan2(y,x);
  float ro_dot = (x*vx+y*vy)/rho;
  
  VectorXd z_pred = VectorXd(3);
  z_pred << rho, theta, ro_dot;
  VectorXd y_diff = z - z_pred;	
  while (y_diff[1] < -M_PI)
    y_diff[1] += 2*M_PI;
  while (y_diff[1] > M_PI)
    y_diff[1] -= 2*M_PI;
  
  cout << "theta: " << theta << endl;
  cout << "y_diff[1]: " << y_diff[1] << endl << endl;

  Cal_X_P(y_diff);
}


void KalmanFilter::Cal_X_P(const VectorXd &y) {
  // VectorXd y = z - z_pred;	
	MatrixXd Ht = H_.transpose();
	MatrixXd S = H_ * P_ * Ht + R_;
	MatrixXd Si = S.inverse();
	MatrixXd PHt = P_ * Ht;
	MatrixXd K = PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;
}