#pragma once

#include <Eigen/Dense>


using PositionMatrix = Eigen::Matrix<double, Eigen::Dynamic, 3>;

enum class ExitType { BelowStaticTolerance, BelowDynamicTolerance, AboveMaxIterations };

struct LMsolution
{
	Eigen::VectorXd solution;
	int iterations;
	ExitType exit_type;
};

class RootFinder
{
public:
	static LMsolution LevenbergMarquardt(const PositionMatrix& centers, const Eigen::VectorXd& distances, const Eigen::Vector3d& initial_guess, 
		double lambda_scale = 10, int64_t maxIterations = 1000, double tolerance = 1e-6);
};