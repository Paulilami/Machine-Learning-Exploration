#include <iostream>
#include <vector>
#include <memory> 
#include <random> 
#include <Eigen/Dense> 
#include <functional> 
#include "vectorization.cpp"
#include "merkletree.cpp"

struct ActivationFunction {
  virtual Eigen::VectorXd operator()(const Eigen::VectorXd& input) const = 0;
};

struct Sigmoid : public ActivationFunction {
  Eigen::VectorXd operator()(const Eigen::VectorXd& input) const override {
    return input.array().logistic();
  }
};

struct ReLU : public ActivationFunction {
  Eigen::VectorXd operator()(const Eigen::VectorXd& input) const override {
    return Eigen::maximum(input.array(), 0.0).matrix();
  }
};

struct NeuralNetworkLayer {
  int inputSize;
  int outputSize;
  Eigen::MatrixXd weights;
  Eigen::VectorXd biases;
  std::unique_ptr<ActivationFunction> activation;

  NeuralNetworkLayer(int inputSize, int outputSize, std::unique_ptr<ActivationFunction> activation) :
      inputSize(inputSize), outputSize(outputSize), activation(std::move(activation)) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(-0.1, 0.1);

    weights = Eigen::MatrixXd::Random(outputSize, inputSize);
    biases = Eigen::VectorXd::Zero(outputSize);

    for (int i = 0; i < outputSize; ++i) {
      for (int j = 0; j < inputSize; ++j) {
        weights(i, j) = distribution(gen);
      }
    }
  }

  Eigen::VectorXd forward(const Eigen::VectorXd& input) const {
    Eigen::VectorXd output = weights * input + biases;
    return (*activation)(output); 
  }
};

struct NeuralNetwork {
  int inputSize;
  std::vector<std::unique_ptr<NeuralNetworkLayer>> layers;

  NeuralNetwork(int inputSize, const std::vector<int>& hiddenLayerSizes, std::unique_ptr<ActivationFunction> activation) :
      inputSize(inputSize) {
    for (int hiddenSize : hiddenLayerSizes) {
      layers.push_back(std::make_unique<NeuralNetworkLayer>(inputSize, hiddenSize, std::move(activation->clone())));
      inputSize = hiddenSize;
    }
    layers.push_back(std::make_unique<NeuralNetworkLayer>(inputSize, /* output size */, std::make_unique<Sigmoid>()));
  }

  Eigen::VectorXd predict(const std::vector<double>& dataVector) const {
    Eigen::VectorXd activation = Eigen::Map<const Eigen::VectorXd>(dataVector.data(), dataVector.size());
    for (const auto& layer : layers) {
      activation = layer->forward(activation);
    }
    return activation;
  }
  void train(const std::vector<std::vector<double>>& dataVectors, const std::vector<std::vector<double>>& targets,
             double learningRate) {
    for (int epoch = 0; epoch < /* number of epochs */; ++epoch) {
      for (size_t i = 0; i < dataVectors.size(); ++i) {
        Eigen::VectorXd input = Eigen::Map<const Eigen::VectorXd>(dataVectors[i].data(), dataVectors[i].size());
        Eigen::VectorXd target = Eigen::Map<const Eigen::VectorXd>(targets[i].data(), targets[i].size());

        Eigen::VectorXd activation = input;
        std::vector<Eigen::VectorXd> activations; 
        activations.push_back(activation);
        for (const auto& layer : layers) {
          activation = layer->forward(activation);
          activations.push_back(activation);
        }

        Eigen::VectorXd outputError = activations.back() - target;
        std::vector<Eigen::VectorXd> deltas;
        deltas.push_back(outputError.array() * activations.back().array().deriv()); /
        for (int layerIdx = layers.size() - 2; layerIdx >= 0; --layerIdx) {
          const auto& layer = layers[layerIdx];
          Eigen::VectorXd previousActivation = activations[layerIdx];
          Eigen::VectorXd delta = layer->weights.transpose() * deltas.back().array() * previousActivation.array().deriv();
          deltas.insert(deltas.begin(), delta);
        }

        for (int layerIdx = 0; layerIdx < layers.size(); ++layerIdx) {
          auto& layer = layers[layerIdx];
          layer->weights -= learningRate * deltas[layerIdx] * activations[layerIdx].transpose();
          layer->biases -= learningRate * deltas[layerIdx];
        }
      }
    }
  }
};

int main() {
  
  std::vector<DataPoint> preprocessedPoints = {/* preprocessed data points */};

  int vectorDimension = 1000;

  std::vector<std::vector<double>> dataVectors = vectorizeDataRandomProjection(preprocessedPoints, vectorDimension);

  std::unique_ptr<MerkleTreeNode> root = constructMerkleTree(dataVectors);

  int inputSize = vectorDimension;
  int hiddenSize = 500;
  int outputSize = /* number of output classes */
  NeuralNetwork net(inputSize, hiddenSize, outputSize);
  
  return 0;
}
