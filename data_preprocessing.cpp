#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp> 
bool validateNumber(int num) {
  return (num >= 1 && num <= 100);
}

struct DataPoint {
  int raw_value;
  double normalized_value; 
};

std::vector<DataPoint> preprocessNumbers() {
  std::vector<DataPoint> dataPoints;
  int num;

  std::cout << "Enter numbers between 1 and 100 (type -1 to stop):" << std::endl;
  while (true) {
    std::cin >> num;

    if (num == -1) {
      break;
    }

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

void writeToFile(const std::vector<DataPoint>& dataPoints, const std::string& filename) {
  std::ofstream outfile(filename);
  if (!outfile.is_open()) {
    std::cerr << "Error: Could not open file for writing." << std::endl;
    return;
  }

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
  std::vector<DataPoint> rawDataPoints = preprocessNumbers();

  std::vector<DataPoint> processedDataPoints = furtherPreprocess(rawDataPoints);

  // ...

  return 0;
}
