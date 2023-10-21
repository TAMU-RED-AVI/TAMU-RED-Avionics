#include "RootFinder.h"
#include <iostream>

// Function to compute the residuals
Eigen::VectorXd computeResiduals(const PositionMatrix& filtered_centers, const Eigen::VectorXd& filtered_distances, const Eigen::Vector3d& estimate)
{
    Eigen::VectorXd residuals(filtered_centers.rows());

    for (size_t i = 0; i < filtered_centers.rows(); ++i)
    {
        residuals(i) = (estimate - filtered_centers.row(i).transpose()).norm() - filtered_distances[i];
    }

    return residuals;
}

// Levenberg-Marquardt algorithm for trilateration
LMsolution RootFinder::LevenbergMarquardt(const PositionMatrix& centers, const Eigen::VectorXd& distances, const Eigen::Vector3d& initial_guess, 
    double lambda_scale, int64_t maxIterations, double tolerance)
{
    LMsolution result;

    Eigen::Vector3d estimate = initial_guess;

    double lambda = 0.001;
    double updateNorm = 1.0; {};
    double previous_residual_norm = std::numeric_limits<double>::max();

    // Count non-zero distances
    size_t validCount = 0;
    for (size_t i = 0; i < distances.size(); ++i) {
        if (distances[i] >= 0.01) {
            validCount++;
        }
    }

    // Create filtered points and distances
    PositionMatrix filtered_centers(validCount, 3);
    Eigen::VectorXd filtered_distances(validCount);
    size_t j = 0;
    for (size_t i = 0; i < distances.size(); ++i) {
        if (distances[i] >= 0.1 && distances[i] <= 20) {
            filtered_centers.row(j) = centers.row(i);
            filtered_distances(j) = distances[i];
            j++;
        }
    }

    for (int64_t i = 0; i < maxIterations; ++i)
    {
        Eigen::VectorXd residuals = computeResiduals(filtered_centers, filtered_distances, estimate);

        if (i > 0)
        {
            // Check if the residual is below a specified tolerance
            if (previous_residual_norm - residuals.norm() < tolerance)
            {
                result.solution = estimate;
                result.iterations = i;
                result.exit_type = ExitType::BelowStaticTolerance;
                return result;
            }

            // Check if the change in residuals is below a dynamic tolerance
            if (abs(previous_residual_norm - residuals.norm()) < tolerance * residuals.norm() && i > maxIterations / 2)
            {
                result.solution = estimate;
                result.iterations = i;
                result.exit_type = ExitType::BelowDynamicTolerance;
                return result;
            }
        }

        previous_residual_norm = residuals.norm();

        // New Residual Computation
        Eigen::MatrixXd Jacobian(residuals.size(), 3);
        for (size_t i = 0; i < filtered_centers.rows(); ++i)
        {
            Eigen::Vector3d diff = estimate - filtered_centers.row(i).transpose();
            double dist = diff.norm();
            Jacobian.row(i) = (diff / dist).transpose();
        }

        Eigen::MatrixXd A = Jacobian.transpose() * Jacobian + lambda * Eigen::MatrixXd::Identity(Jacobian.cols(), Jacobian.cols());
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
        Eigen::VectorXd delta = svd.solve(-Jacobian.transpose() * residuals);
        Eigen::VectorXd estimate_new = estimate + delta;
        Eigen::VectorXd residuals_new = computeResiduals(filtered_centers, filtered_distances, estimate_new);

        // Lambda Scaling
        if (residuals_new.norm() < residuals.norm())
        {
            estimate = estimate_new;
            lambda /= lambda_scale;
        }
        else
        {
            lambda *= lambda_scale;
        }
    }

    result.solution = estimate;
    result.iterations = maxIterations;
    result.exit_type = ExitType::AboveMaxIterations;
    return result;
}