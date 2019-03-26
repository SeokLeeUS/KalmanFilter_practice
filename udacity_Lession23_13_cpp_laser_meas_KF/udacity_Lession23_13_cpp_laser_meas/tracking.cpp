#include "tracking.h"
#include <iostream>
#include "..\eigen\Dense"
#include <math.h>
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;
using std::vector;

Tracking::Tracking()
{
	is_initialized_ = false;
	previous_timestamp_ = 0;
	kf_.x_ = VectorXd(4);
	kf_.P_ = MatrixXd(4, 4);
	kf_.P_ << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1000, 0,
		0, 0, 0, 1000;

	//measurement covariance
	kf_.R_ = MatrixXd(2, 2);
	kf_.R_ << 0.0225, 0,
			  0, 0.0225;

	//measurement matrix
	kf_.H_ = MatrixXd(2, 4);
	kf_.H_ << 1, 0, 0, 0,
			  0, 1, 0, 0;

	kf_.F_ = MatrixXd(4, 4);
	kf_.F_ << 1, 0, 1, 0,
			  0, 1, 0, 1,
		      0, 0, 1, 0,
		      0, 0, 0, 1;

	// set the acceleration noise components
	noise_ax = 5;
	noise_ay = 5;
}

Tracking::~Tracking()
{

}


// Process a single measurement 

void Tracking::ProcessMeasurement(const MeasurementPackage &measurement_pack)
{
	if (!is_initialized_)
	{
		kf_.x_ << measurement_pack.raw_measurements_[0],
			      measurement_pack.raw_measurements_[1],
			      0,
			      0;

		previous_timestamp_ = measurement_pack.timestamp_;
		is_initialized_ = true;
		return;
	}

	// compute time elapsed between the current and previous measurements
	//dt - expressed in seconds
	float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
	previous_timestamp_ = measurement_pack.timestamp_;


	// TODO: YOUR CODE HERE
	// 1. Modify the F matrix so that the time is integrated
	// 2. Set the process covariance matrix Q
	// 3. Call the Kalman Filter predict() function
	// 4. Call the Kalman Filter update() function
	// with the most recent raw measurements_


	kf_.F_ << 1, 0, dt, 0,
			  0, 1, 0, dt,
			  0, 0, 1, 0,
		      0, 0, 0, 1;
	kf_.Q_ = MatrixXd(4, 4);
	kf_.Q_ << pow(dt, 4)*noise_ax/4, 0, pow(dt, 3)*noise_ax/2, 0,
		0, pow(dt, 4)*noise_ay/4, 0, pow(dt, 3)*noise_ay/2,
		pow(dt, 3)*noise_ax/2, 0, pow(dt, 2)*noise_ax, 0,
		0, pow(dt, 3)*noise_ay/2, 0, pow(dt, 2)*noise_ay;


	kf_.Predict();
	kf_.Update(measurement_pack.raw_measurements_);

	cout << "x_=" << kf_.x_ << endl;
	cout << "p_=" << kf_.P_ << endl;

}

