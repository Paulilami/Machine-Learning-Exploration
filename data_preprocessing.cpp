#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp> // Include JSON library for potential future output

// Function to validate input number within the range (1-100)
bool validateNumber(int num) {
  return (num >= 1 && num <= 100);
}

// Struct to represent a single data point
struct DataPoint {
  int raw_value;
  double normalized_value; // Add more fields for future processing needs
};

// Function to handle user input and perform basic preprocessing
std::vector<DataPoint> preprocessNumbers() {
  std::vector<DataPoint> dataPoints;
  int num;

  std::cout << "Enter numbers between 1 and 100 (type -1 to stop):" << std::endl;
  while (true) {
    std::cin >> num;

    // Check for end of input (-1)
    if (num == -1) {
      break;
    }

    // Validate input number
    if (!validateNumber(num)) {
      std::cerr << "Error: Invalid input. Please enter a number between 1 and 100." << std::endl;
      continue;
    }

    DataPoint dataPoint;
    dataPoint.raw_value = num;
    dataPoints.push_back(dataPoint);
  }

  return dataPoints;
}

// Function to perform further preprocessing on data points
std::vector<DataPoint> furtherPreprocess(const std::vector<DataPoint>& dataPoints) {
  std::vector<DataPoint> processedPoints;

  // Example: Normalize numbers to a range between 0 and 1
  for (const DataPoint& point : dataPoints) {
    DataPoint processedPoint;
    processedPoint.raw_value = point.raw_value;
    processedPoint.normalized_value = static_cast<double>(point.raw_value) / 100.0;
    processedPoints.push_back(processedPoint);
  }

  return processedPoints;
}

// Function to write preprocessed data to a file (optional for future integration)
void writeToFile(const std::vector<DataPoint>& dataPoints, const std::string& filename) {
  std::ofstream outfile(filename);
  if (!outfile.is_open()) {
    std::cerr << "Error: Could not open file for writing." << std::endl;
    return;
  }

  // Write data points in JSON format (example)
  nlohmann::json jsonData;
  std::vector<nlohmann::json> dataArray;
  for (const DataPoint& point : dataPoints) {
    dataArray.push_back({{"raw_value", point.raw_value}, {"normalized_value", point.normalized_value}});
  }
  jsonData["data"] = dataArray;

  outfile << jsonData;
  outfile.close();
  std::cout << "Preprocessed data written to file: " << filename << std::endl;
}

int main() {
  // Get preprocessed data points
  std::vector<DataPoint> rawDataPoints = preprocessNumbers();

  // Perform further preprocessing (optional)
  std::vector<DataPoint> processedDataPoints = furtherPreprocess(rawDataPoints);

  // Print or write to file (choose one or implement logic for both)
  // std::cout << "Preprocessed data points:" << std::endl;
  // for (const DataPoint& point : processedDataPoints) {
  //   std::cout << "Raw value: " << point.raw_value << ", Normalized value: " << point.normalized_value << std::endl;
  // }

  writeToFile(processedDataPoints, "preprocessed_data.json"); // Example output filename

  return 0;
}
