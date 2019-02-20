/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: 
	// 1. Set the number of particles. 
	// 2. Initialize all particles to first position (based on estimates of x, y, theta and their uncertainties from GPS)
	// 3. and all weights to 1. 
	// 4. Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).
	num_particles = 100;

	//creates a normal (Gaussian) distribution
	normal_distribution<double> dist_x(x, std[0]);
	normal_distribution<double> dist_y(y, std[1]);
	normal_distribution<double> dist_theta(theta, std[2]);
	
	default_random_engine gen;
	for (int i = 0; i < num_particles; ++i) {
		Particle particle;
		particle.id = i;
		particle.x = dist_x(gen);
		particle.y = dist_y(gen);
		particle.theta = dist_theta(gen);
		particle.weight = 1;

		particles.push_back(particle);
		weights.push_back(1);
	}
	is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/
	default_random_engine gen;
	for (int i=0; i<num_particles; i++) {
		double new_x;
		double new_y;
		double new_theta;

		if (yaw_rate == 0) {
			new_x = particles[i].x + velocity*delta_t*cos(particles[i].theta);
			new_y = particles[i].y + velocity*delta_t*sin(particles[i].theta);
			new_theta = particles[i].theta;
		} else {
			new_x = particles[i].x + velocity/yaw_rate * (sin(particles[i].theta + yaw_rate*delta_t) - sin(particles[i].theta));
			new_y = particles[i].y + velocity/yaw_rate * (cos(particles[i].theta) - cos(particles[i].theta + yaw_rate*delta_t));
			new_theta = particles[i].theta + yaw_rate*delta_t;
		}
		normal_distribution<double> dist_x(new_x, std_pos[0]);
		normal_distribution<double> dist_y(new_y, std_pos[1]);
		normal_distribution<double> dist_theta(new_theta, std_pos[2]);

		particles[i].x = dist_x(gen);
		particles[i].y = dist_y(gen);
		particles[i].theta = dist_theta(gen);
	}
}

void ParticleFilter::dataAssociation(Particle& particle, std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.
	vector<int> associations;
	vector<double> sense_xs;
	vector<double> sense_ys;
	for (unsigned int p=0; p<predicted.size(); p++) {
		double min_dis = DBL_MAX;
		double sense_x;
		double sense_y;
		for(unsigned int b=0; b<observations.size(); b++) {
			double dis = sqrt(pow((observations[b].x-predicted[p].x), 2.0) + pow((observations[b].x-predicted[p].x), 2.0));
			if (dis < min_dis) {
				min_dis = dis;
				sense_x = observations[b].x;
				sense_y = observations[b].y;
			}
		}
		associations.push_back(predicted[p].id);
		sense_xs.push_back(sense_x);
		sense_ys.push_back(sense_y);
	}
	SetAssociations(particle, associations, sense_xs, sense_ys);

}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html



	// for each particles 
		// turn obs to map coordinate
		// from map to get landmarks
		// get association from obs
		// for each association
			// cal gaussian by landmarks
		// multi all gaussian 



	// for each particles 
	for (int p=0; p<num_particles; p++) {
		vector<double> sense_x;
		vector<double> sense_y;
		Particle& par = particles[p];

		// turn obs to map coordinate
		vector<LandmarkObs> trans_observations;
		for (unsigned int i=0; i<observations.size(); i++) {
			LandmarkObs trans_obs;
			LandmarkObs obs = observations[i];

			trans_obs.x = par.x + obs.x*cos(par.theta) - obs.y*sin(par.theta);
			trans_obs.y = par.y + obs.x*sin(par.theta) + obs.y*cos(par.theta);
			trans_observations.push_back(trans_obs);
		}

		// predicted landmarks	
		vector<LandmarkObs> predicteds;
		for(unsigned int j=0; j<map_landmarks.landmark_list.size(); j++) {
			Map::single_landmark_s landmark = map_landmarks.landmark_list[j];
			double cal_dist = sqrt(pow(par.x-landmark.x_f, 2.0) + pow(par.y-landmark.y_f, 2.0));
			if (cal_dist < sensor_range) {
				LandmarkObs pred;
				pred.id = landmark.id_i;
				pred.x = landmark.x_f;
				pred.y = landmark.y_f;
				predicteds.push_back(pred);
			}
		}

		// set association
		dataAssociation(par, predicteds, trans_observations);
		par.weight = 1.0;
		for (unsigned int a=0; a<par.associations.size(); a++) {
			// cal gaussian by landmark
			double x = par.sense_x[a];
			double y = par.sense_y[a];
			double mu_x = predicteds[a].x;
			double mu_y = predicteds[a].y;
			double std_x = std_landmark[0];
			double std_y = std_landmark[1];

			double p_xy = 1/(2*M_PI*std_x*std_y) 
					* exp(-(pow(x-mu_x,2)/(2*pow(std_x,2))+pow(y-mu_y,2)/(2*pow(std_y,2))));
			if (p_xy > 0) par.weight *= p_xy;
		}	
		weights[p] = par.weight;
	}

}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution

	// while w[index] < beta:
	// 	beta = beta - w[index]
	// 	index = index + 1
	// select p[index]

	vector<Particle> new_pars;
	vector<double> new_weight;

	int index = rand() % num_particles;
	double beta = 0.0;
	double mw = *max_element(begin(weights), end(weights));
	for (int i=0; i<num_particles; i++) {
		beta += ((double) rand() / (RAND_MAX)) * (2.0*mw);
		while (beta>weights[index]) {
			beta -= weights[index];
			index = (index+1)%num_particles;
		}
		new_pars.push_back(particles[index]);
		new_weight.push_back(weights[index]);
	}
	particles = new_pars;
	weights = new_weight;
}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations, 
                                     const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
    //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
    // associations: The landmark id that goes along with each listed association
    // sense_x: the associations x mapping already converted to world coordinates
    // sense_y: the associations y mapping already converted to world coordinates

    particle.associations= associations;
    particle.sense_x = sense_x;
    particle.sense_y = sense_y;

	return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
