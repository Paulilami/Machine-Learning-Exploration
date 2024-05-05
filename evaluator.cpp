#include <iostream>
#include <vector>
#include "neuralnetwork.cpp" // Include the neural network definition
#include <cmath> // for std::pow

// Function to calculate Mean Absolute Error (MAE)
double calculateMAE(const std::vector<std::vector<double>>& predictions,
                   const std::vector<std::vector<double>>& targets) {
  double totalError = 0.0;
  for (size_t i = 0; i < predictions.size(); ++i) {
    for (size_t j = 0; j < predictions[i].size(); ++j) {
      double error = std::abs(predictions[i][j] - targets[i][j]);
      totalError += error;
    }
  }
  return totalError / (predictions.size() * targets[0].size());
}

// Function to calculate R-squared (coefficient of determination)
double calculateRSquared(const std::vector<std::vector<double>>& predictions,
                         const std::vector<std::vector<double>>& targets) {
  double sum_of_squared_errors = 0.0, sum_of_squared_means = 0.0;
  double mean_target = 0.0;
  for (size_t i = 0; i < predictions.size(); ++i) {
    mean_target += targets[i][0]; // Assuming single target value per data point
    for (size_t j = 0; j < predictions[i].size(); ++j) {
      double error = predictions[i][j] - targets[i][j];
      sum_of_squared_errors += error * error;
      sum_of_squared_means += (targets[i][j] - mean_target) * (targets[i][j] - mean_target);
    }
  }
  mean_target /= predictions.size();
  return 1.0 - (sum_of_squared_errors / sum_of_squared_means);
}

// Function to evaluate multiple neural networks
void evaluateNetworks(const std::vector<NeuralNetwork>& networks,
                       const std::vector<std::vector<double>>& dataVectors,
                       const std::vector<std::vector<double>>& targets) {
  for (const auto& net : networks) {
    std::vector<std::vector<double>> predictions;
    for (const auto& dataVector : dataVectors) {
      Eigen::VectorXd input = Eigen::Map<const Eigen::VectorXd>(dataVector.data(), dataVector.size());
      Eigen::VectorXd prediction = net.forward(input);
      predictions.push_back(prediction.array().toVectorXd().toStdVector());
    }

    // Calculate evaluation metrics
    double mse = calculateMSE(predictions, targets);
    double mae = calculateMAE(predictions, targets);
    double rSquared = calculateRSquared(predictions, targets);

    std::cout << "Network Evaluation Results:" << std::endl;
    std::cout << "Mean Squared Error (MSE): " << mse << std::endl;
    std::cout << "Mean Absolute Error (MAE): " << mae << std::endl;
    std::cout << "R-squared: " << rSquared << std::endl;

    // Optionally perform classification specific metrics (modify as needed)
    if (targets[0].size() > 1) {
      // Assuming multi-class classification
      std::vector<double> precisions(targets[0].size(), 0.0);
      std::vector<double> recalls(targets[0].size(), 0.0);
      // ... (calculate precision and recall for each class) ...
      std::cout << "Precision (per class):" << std::endl;
      for (size_t i = 0; i < precisions.size(); ++i) {
        std::cout << "Class " << i << ": " << precisions[i] << std::endl;
      }
      std::cout << "Recall (per class):" << std::endl;
      for (size_t i = 0; i < recalls.size(); ++i) {
        std::cout << "Class " << i << ": " << recalls[i] << std::endl;
      }
    }

    // Visualization (c? libraries like Matplotlib or Plotly)
    // ... ( plot predictions vs targets, confusion matrix, etc.) ...

    std::cout << std::endl; // Separator between network evaluations
  }
}

int main() {
  // ... (load trained neural networks)
  // ... (load evaluation data)

  // Evaluate multiple networks
  evaluateNetworks(networks, testDataVectors, testTargets);

  return 0;
}
