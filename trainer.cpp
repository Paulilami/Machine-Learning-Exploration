#include <iostream>
#include <vector>
#include "neuralnetwork.cpp" // Include the neural network definition

// Define an optimizer class (modify as needed)
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
  // Implement Adam optimizer with momentum and learning rate decay (consider libraries like Eigen for efficiency)
  void update(NeuralNetwork& net, const std::vector<double>& learningRates) const override {
    // ... (implementation details omitted for brevity) ...
  }
};

// Function to train the neural network with mini-batch training and regularization
void trainNetwork(NeuralNetwork& net, const std::vector<std::vector<double>>& dataVectors,
                   const std::vector<std::vector<double>>& targets, const Optimizer& optimizer,
                   int batchSize, double learningRate, double weightDecay = 0.0) {
  int numSamples = dataVectors.size();
  for (int epoch = 0; epoch < /* number of epochs */; ++epoch) {
    // Shuffle data for each epoch (consider using libraries like Eigen for efficiency)
    std::random_device rd;
    std::mt19937 gen(rd());
    for (int i = 0; i < numSamples; ++i) {
      std::swap(dataVectors[i], dataVectors[std::uniform_int_distribution<int>(0, numSamples - 1)(gen)]);
      std::swap(targets[i], targets[std::uniform_int_distribution<int>(0, numSamples - 1)(gen)]);
    }

    for (int i = 0; i < numSamples; i += batchSize) {
      // Perform forward propagation for a mini-batch
      std::vector<Eigen::VectorXd> activations;
      std::vector<Eigen::VectorXd> errors;
      for (int j = i; j < std::min(i + batchSize, numSamples); ++j) {
        Eigen::VectorXd input = Eigen::Map<const Eigen::VectorXd>(dataVectors[j].data(), dataVectors[j].size());
        Eigen::VectorXd activation = net.forward(input);
        activations.push_back(activation);
      }

      // Backpropagate through the network for a mini-batch
      Eigen::VectorXd outputError = activations.back() - Eigen::Map<const Eigen::VectorXd>(targets[i].data(), targets[i].size());
      errors.push_back(outputError);
      for (int layerIdx = net.layers.size() - 2; layerIdx >= 0; --layerIdx) {
        const auto& layer = net.layers[layerIdx];
        Eigen::VectorXd previousActivation = activations[layerIdx];
        Eigen::VectorXd delta = layer->backpropagate(errors.back(), previousActivation);
        errors.insert(errors.begin(), delta); // Insert delta at the beginning for efficient access
      }

      // Apply dropout regularization during training (consider other regularization techniques)
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

      // Update weights and biases using optimizer and regularization
      std::vector<double> learningRates = {learningRate};
      // c? adjusting learning rates for different layers (advanced-->beta 0.2)
      // learningRates = ...; // m? mod learning rates based on layer 

      optimizer.update(net, learningRates);

      // Reset dropout masks after each mini-batch update
      if (net.dropoutRate > 0.0) {
        for (auto& layer : net.layers) {
          layer->weights.array() *= 1.0 / (1.0 - net.dropoutRate); // Scale weights to compensate for dropout
        }
      }
    }
  }
}

int main() {
  // ... N.cpp ...
  // Define optimizer (modify as needed)
  std::unique_ptr<Optimizer> optimizer = std::make_unique<Adam>(); // Example using Adam optimizer

  // Train the network
  trainNetwork(net, dataVectors, targets, *optimizer, batchSize, learningRate, weightDecay);

  // ...
  
  return 0;
}
