#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <random> 

#include "data_preprocessing.cpp" 

std::vector<double> generateRandomUnitVector(int dimension) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> distribution(-1.0, 1.0);

  std::vector<double> vector(dimension);
  for (int i = 0; i < dimension; ++i) {
    vector[i] = distribution(gen);
  }

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

double tfIdfWeighting(const DataPoint& dataPoint, const std::vector<int>& documentFrequencies) {
  int rawValue = dataPoint.raw_value;
  int documentCount = documentFrequencies.size(); 

  double termFrequency = static_cast<double>(/* count of rawValue in the data */) / dataPoint.raw_value;

  double inverseDocumentFrequency = std::log2(static_cast<double>(documentCount) / documentFrequencies[rawValue - 1]);

  return termFrequency * inverseDocumentFrequency;
}

std::vector<double> vectorizeDataPointRandomProjection(const DataPoint& dataPoint, int dimension, const std::vector<int>& documentFrequencies) {
  std::vector<double> vector = generateRandomUnitVector(dimension);

  double weight = tfIdfWeighting(dataPoint, documentFrequencies);

  vector[0] = weight * dataPoint.raw_value;

  return vector;
}

std::vector<std::vector<double>> vectorizeDataRandomProjection(const std::vector<DataPoint>& dataPoints, int dimension, const std::vector<int>& documentFrequencies) {
  std::vector<std::vector<double>> vectors;
  for (const DataPoint& point : dataPoints) {
    vectors.push_back(vectorizeDataPointRandomProjection(point, dimension, documentFrequencies));
  }
  return vectors;
}

int main() {
  std::vector<DataPoint> preprocessedPoints = {/* preprocessed data points */};

  int vectorDimension = 1000;

  std::vector<int> documentFrequencies = {/* document frequencies for each value */};

  std::vector<std::vector<double>> dataVectors = vectorizeDataRandomProjection(preprocessedPoints, vectorDimension, documentFrequencies);

  std::cout << "Data vectors (using random projection):" << std::endl;
  for (const std::vector<double>& vec : dataVectors) {
    std::cout << "[";
    for (double value : vec) {
			std::cout << "[";
	  for (double value : vec) {
	    std::cout << value << ", "; 
		 }
	  std::cout << "]" << std::endl;
	}

	  return 0;
	}
