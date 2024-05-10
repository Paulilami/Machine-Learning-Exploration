#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <random> 
#include <Eigen/Dense> 
#include <algorithm> 
#include <functional> 

enum class DataType { DOUBLE, INTEGER, CATEGORICAL };

struct DataPoint {
  std::vector<std::variant<double, int, std::string>> features;
  std::vector<double> target; 
  DataType type; 
};

std::vector<DataPoint> loadData(const std::string& filename, const std::string& format) {
  std::vector<DataPoint> dataPoints;

  if (format == "CSV") {
    dataPoints = loadDataFromCSV(filename);
  } else if (format == "IMAGE") {
  } else {
    std::cerr << "Error: Unsupported data format: " << format << std::endl;
  }

  return dataPoints;
}

std::vector<DataPoint> loadDataFromCSV(const std::string& filename) {
  std::ifstream file(filename);
  std::vector<DataPoint> dataPoints;

  if (file.is_open()) {
    std::string line;
    std::getline(file, line); // Skip header row 
    std::vector<std::string> header;
    std::stringstream headerStream(line);
    std::string value;

    // Read header row 
    while (std::getline(headerStream, value, ',')) {
      header.push_back(value);
    }

    while (std::getline(file, line)) {
      DataPoint dataPoint;
      std::stringstream lineStream(line);

      int featureIdx = 0;
      while (std::getline(lineStream, value, ',')) {
        if (featureIdx < header.size()) {
          if (std::isdigit(value[0])) {
            try {
              dataPoint.features.push_back(std::stod(value));
              dataPoint.type = DataType::DOUBLE;
            } catch (const std::invalid_argument&) {
              dataPoint.features.push_back(std::stoi(value));
              dataPoint.type = DataType::INTEGER;
            }
          } else {
            dataPoint.features.push_back(value);
            dataPoint.type = DataType::CATEGORICAL;
          }
        }
        ++featureIdx;
      }

      std::getline(file, line); 
      lineStream = std::stringstream(line);
      while (std::getline(lineStream, value, ',')) {
        dataPoint.target.push_back(std::stod(value));
      }

      dataPoints.push_back(dataPoint);
    }
    file.close();
  } else {
    std::cerr << "Error: Could not open file " << filename << std::endl;
  }

  return dataPoints;
}

void preProcessData(std::vector<DataPoint>& dataPoints) {
  if (dataPoints[0].type == DataType::DOUBLE) {
    Eigen::VectorXd minValues = dataPoints[0].features[0];
    Eigen::VectorXd maxValues = dataPoints[0].features[0];
    for (const auto& dataPoint : dataPoints) {
      for (size_t i = 0; i < dataPoint.features.size(); ++i) {
        const auto& feature = std::get<double>(dataPoint.features[i]);
        minValues(i) = std::min(minValues(i), feature);
        maxValues(i) = std::max(maxValues(i), feature);
      }
    }

    for (auto& dataPoint : dataPoints) {
      if (dataPoint.type == DataType::DOUBLE) {
        for (size_t i = 0; i < dataPoint.features.size(); ++i) {
          double normalizedValue = (std::get<double>(dataPoint.features[i]) - minValues(i)) / (maxValues(i) - minValues(i));
          dataPoint.features[i] = normalizedValue;
        }
      }
    }
  } else if (dataPoints[0].type == DataType::CATEGORICAL) {
    std::vector<std::unordered_set<std::string>> uniqueCategories(dataPoints[0].features.size());
    for (const auto& dataPoint : dataPoints) {
      for (size_t i = 0; i < dataPoint.features.size(); ++i) {
        uniqueCategories[i].insert(std::get<std::string>(dataPoint.features[i]));
      }
    }

    // One-hot encode each ...feature
    int numCategories = 0;
    for (const auto& categorySet : uniqueCategories) {
      numCategories = std::max(numCategories, static_cast<int>(categorySet.size()));
    }
    for (auto& dataPoint : dataPoints) {
      if (dataPoint.type == DataType::CATEGORICAL) {
        std::vector<double> oneHotEncoded(numCategories, 0.0);
        for (size_t i = 0; i < dataPoint.features.size(); ++i) {
          int categoryIdx = 0;
          for (const auto& category : uniqueCategories[i]) {
            if (category == std::get<std::string>(dataPoint.features[i])) {
              oneHotEncoded[categoryIdx] = 1.0;
              break;
            }
            ++categoryIdx;
          }
        }
        dataPoint.features.clear();
        dataPoint.features = std::vector<std::variant<double, int, std::string>>(numCategories, 0.0);
        for (int i = 0; i < numCategories; ++i) {
          dataPoint.features[i] = oneHotEncoded[i];
        }
      }
    }
  }
}

void augmentData(std::vector<DataPoint>& dataPoints) {
// ...
}

int main() {
  std::string dataFilePath = "data.csv";
  std::string dataFormat = "CSV";

  std::vector<DataPoint> dataPoints = loadData(dataFilePath, dataFormat);

  preProcessData(dataPoints);

  // Data augmentation
  augmentData(dataPoints);

  std::vector<std::vector<double>> dataVectors;
  std::vector<std::vector<double>> targets;
  for (const auto& dataPoint : dataPoints) {
    if (dataPoint.type == DataType::DOUBLE) {
      std::vector<double> features;
      for (const auto& feature : dataPoint.features) {
        features.push_back(std::get<double>(feature));
      }
      dataVectors.push_back(features);
    } else {
     
      std::vector<double> features;
      dataVectors.push_back(features);
    }
    targets.push_back(dataPoint.target);
  }
  // ...

  return 0;
}
