#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <random> // for random number generation

// Assuming we'll use preprocessed data points from data_preprocessing.cpp
#include "data_preprocessing.cpp" // Include the header file

// Function to generate a random unit vector in high-dimensional space
std::vector<double> generateRandomUnitVector(int dimension) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> distribution(-1.0, 1.0);

  std::vector<double> vector(dimension);
  for (int i = 0; i < dimension; ++i) {
    vector[i] = distribution(gen);
  }

  // Normalize the vector to unit length
  double norm = 0.0;
  for (double value : vector) {
    norm += value * value;
  }
  norm = std::sqrt(norm);
  for (int i = 0; i < dimension; ++i) {
    vector[i] /= norm;
  }

  return vector;
}

// Function to perform TF-IDF weighting on a data point (example)
double tfIdfWeighting(const DataPoint& dataPoint, const std::vector<int>& documentFrequencies) {
  int rawValue = dataPoint.raw_value;
  int documentCount = documentFrequencies.size(); // Assuming documentFrequencies size represents document count

  // Calculate term frequency (TF)
  double termFrequency = static_cast<double>(/* count of rawValue in the data */) / dataPoint.raw_value;

  // Calculate inverse document frequency (IDF)
  double inverseDocumentFrequency = std::log2(static_cast<double>(documentCount) / documentFrequencies[rawValue - 1]);

  return termFrequency * inverseDocumentFrequency;
}

// Function to vectorize a data point using a high-dimensional random projection
std::vector<double> vectorizeDataPointRandomProjection(const DataPoint& dataPoint, int dimension, const std::vector<int>& documentFrequencies) {
  std::vector<double> vector = generateRandomUnitVector(dimension);

  // Apply TF-IDF weighting to raw value (example weighting scheme)
  double weight = tfIdfWeighting(dataPoint, documentFrequencies);

  // Project the weighted raw value onto the random vector
  vector[0] = weight * dataPoint.raw_value;

  // Additional features can be included for projection (e.g., normalized_value)
  // vector[1] = weight * dataPoint.normalized_value; // You can add more elements based on your needs

  return vector;
}

// Function to vectorize all data points using random projection
std::vector<std::vector<double>> vectorizeDataRandomProjection(const std::vector<DataPoint>& dataPoints, int dimension, const std::vector<int>& documentFrequencies) {
  std::vector<std::vector<double>> vectors;
  for (const DataPoint& point : dataPoints) {
    vectors.push_back(vectorizeDataPointRandomProjection(point, dimension, documentFrequencies));
  }
  return vectors;
}

int main() {
  // Assuming you have preprocessed data points from data_preprocessing.cpp
  // You might need to adjust this based on how you call/access the data points
  std::vector<DataPoint> preprocessedPoints = {/* preprocessed data points */};

  // Define dimensionality for the high-dimensional space (choose a suitable value)
  int vectorDimension = 1000;

  // Assuming you have document frequency information for each possible value (1-100)
  std::vector<int> documentFrequencies = {/* document frequencies for each value */};

  // Vectorize the data points using random projection
  std::vector<std::vector<double>> dataVectors = vectorizeDataRandomProjection(preprocessedPoints, vectorDimension, documentFrequencies);

  // Print or use the data vectors (modify as needed)
  std::cout << "Data vectors (using random projection):" << std::endl;
  for (const std::vector<double>& vec : dataVectors) {
    std::cout << "[";
    for (double value : vec) {
			std::cout << "[";
	  for (double value : vec) {
	    std::cout << value << ", "; // Print each value in the vector
		 }
	  std::cout << "]" << std::endl;
	}

	  return 0;
	}
