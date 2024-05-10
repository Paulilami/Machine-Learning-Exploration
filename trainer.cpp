#include <iostream>
#include <vector>
#include "neuralnetwork.cpp"

struct Optimizer {
  virtual void update(NeuralNetwork& net, const std::vector<double>& learningRates) const = 0;
};

struct SGD : public Optimizer {
  void update(NeuralNetwork& net, const std::vector<double>& learningRates) const override {
    for (auto& layer : net.layers) {
      layer->weights -= learningRates[0] * layer->weights.array() * layer->weightDecay;
      layer->biases -= learningRates[0] * layer->biases.array();
    }
  }
};

struct Adam : public Optimizer {
  void update(NeuralNetwork& net, const std::vector<double>& learningRates) const override {
  }
};

void trainNetwork(NeuralNetwork& net, const std::vector<std::vector<double>>& dataVectors,
                   const std::vector<std::vector<double>>& targets, const Optimizer& optimizer,
                   int batchSize, double learningRate, double weightDecay = 0.0) {
  int numSamples = dataVectors.size();
  for (int epoch = 0; epoch < /* number of epochs */; ++epoch) {
    std::random_device rd;
    std::mt19937 gen(rd());
    for (int i = 0; i < numSamples; ++i) {
      std::swap(dataVectors[i], dataVectors[std::uniform_int_distribution<int>(0, numSamples - 1)(gen)]);
      std::swap(targets[i], targets[std::uniform_int_distribution<int>(0, numSamples - 1)(gen)]);
    }

    for (int i = 0; i < numSamples; i += batchSize) {
      std::vector<Eigen::VectorXd> activations;
      std::vector<Eigen::VectorXd> errors;
      for (int j = i; j < std::min(i + batchSize, numSamples); ++j) {
        Eigen::VectorXd input = Eigen::Map<const Eigen::VectorXd>(dataVectors[j].data(), dataVectors[j].size());
        Eigen::VectorXd activation = net.forward(input);
        activations.push_back(activation);
      }

      Eigen::VectorXd outputError = activations.back() - Eigen::Map<const Eigen::VectorXd>(targets[i].data(), targets[i].size());
      errors.push_back(outputError);
      for (int layerIdx = net.layers.size() - 2; layerIdx >= 0; --layerIdx) {
        const auto& layer = net.layers[layerIdx];
        Eigen::VectorXd previousActivation = activations[layerIdx];
        Eigen::VectorXd delta = layer->backpropagate(errors.back(), previousActivation);
        errors.insert(errors.begin(), delta); 
      }

      if (net.dropoutRate > 0.0) {
        std::random_device rd;
        std::mt19937 dropoutGen(rd());
        std::bernoulli_distribution dropout(1.0 - net.dropoutRate);
        for (auto& layer : net.layers) {
          for (int i = 0; i < layer->weights.rows(); ++i) {
            for (int j = 0; j < layer->weights.cols(); ++j) {
              if (dropout(dropoutGen)) {
                layer->weights(i, j) = 0.0;
              }
            }
          }
        }
      }

      std::vector<double> learningRates = {learningRate};
    
      optimizer.update(net, learningRates);

      if (net.dropoutRate > 0.0) {
        for (auto& layer : net.layers) {
          layer->weights.array() *= 1.0 / (1.0 - net.dropoutRate); 
        }
      }
    }
  }
}

int main() {

  trainNetwork(net, dataVectors, targets, *optimizer, batchSize, learningRate, weightDecay);

  // ...
  
  return 0;
}
