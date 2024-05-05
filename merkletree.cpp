#include <iostream>
#include <vector>
#include <string>
#include <memory> // for smart pointers
#include <stdexcept> // for runtime exceptions
#include <crypto++/sha.h> // Include a cryptographic hash library (Crypto++)
#include <crypto++/hex.h>

// Assuming we'll use data vectors from vectorization.cpp
#include "vectorization.cpp" // Include the header file

// Function to calculate the SHA-256 hash of a string
std::string hashString(const std::string& data) {
  CryptoPP::SHA256 hash;
  byte digest[CryptoPP::SHA256::DIGESTSIZE];

  hash.CalculateDigest(digest, reinterpret_cast<const byte*>(data.c_str()), data.length());

  // Convert hash digest to a hexadecimal string
  std::string hashedData;
  CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hashedData));
  encoder.Put(digest, sizeof(digest));
  encoder.MessageEnd();

  return hashedData;
}

// Function to calculate the SHA-256 hash of a data vector
std::string hashDataVector(const std::vector<double>& dataVector) {
  // Convert data vector to a byte array (assuming double precision floats)
  int vectorSize = dataVector.size();
  std::vector<byte> dataBytes(vectorSize * sizeof(double));
  std::memcpy(dataBytes.data(), dataVector.data(), vectorSize * sizeof(double));

  return hashString(std::string(dataBytes.begin(), dataBytes.end())); // Use hashString for consistency
}

// Struct to represent a node in the Merkle tree
struct MerkleTreeNode {
  std::string hash;
  std::unique_ptr<MerkleTreeNode> left;
  std::unique_ptr<MerkleTreeNode> right;

  MerkleTreeNode(const std::string& hash) : hash(hash), left(nullptr), right(nullptr) {}
};

// Function to construct a Merkle tree from data vectors
std::unique_ptr<MerkleTreeNode> constructMerkleTree(const std::vector<std::vector<double>>& dataVectors) {
  if (dataVectors.empty()) {
    throw std::runtime_error("Empty data provided for Merkle tree construction");
  }

  if (dataVectors.size() == 1) {
    return std::make_unique<MerkleTreeNode>(hashDataVector(dataVectors[0])); // Leaf node with hashed data vector
  }

  // Recursively construct left and right subtrees
  std::vector<std::vector<double>> leftHalf(dataVectors.begin(), dataVectors.begin() + dataVectors.size() / 2);
  std::vector<std::vector<double>> rightHalf(dataVectors.begin() + dataVectors.size() / 2, dataVectors.end());
  std::unique_ptr<MerkleTreeNode> leftNode = constructMerkleTree(leftHalf);
  std::unique_ptr<MerkleTreeNode> rightNode = constructMerkleTree(rightHalf);

  // Create a parent node with the hash of its children's concatenation
  std::string parentHash = hashString(leftNode->hash + rightNode->hash);
  return std::make_unique<MerkleTreeNode>(parentHash, std::move(leftNode), std::move(rightNode));
}

// Function to get the Merkle proof for a specific data point (index)
std::vector<std::string> getMerkleProof(const std::unique_ptr<MerkleTreeNode>& root, size_t dataIndex, const std::vector<std::vector<double>>& dataVectors) {
  if (!root || dataIndex >= dataVectors.size()) {
    throw std::invalid_argument("Invalid root node or data index for proof generation");
  }

  std::vector<std::string> proof;
  std::unique_ptr<MerkleTreeNode> currentNode = root.get();
  size_t siblingIndex;

  // Traverse the tree until reaching the leaf node corresponding to the data point
  while (currentNode->left && currentNode->right) {
    siblingIndex = (dataIndex % 2 == 0) ? dataIndex + 1 : dataIndex - 1;
    if (dataIndex < siblingIndex) {
      proof.push_back(currentNode->right->hash); // Sibling hash for left child
      currentNode = currentNode->left.get();
    } else {
      proof.push_back(currentNode->left->hash); // Sibling hash for right child
      currentNode = currentNode->right.get();
    }
    dataIndex /= 2;
  }

  // Handle the case where the data point is the only element (root is the leaf node)
  if (!currentNode->left && !currentNode->right) {
    proof.clear(); // No siblings for the root (single element)
  }

  return proof;
}

// Function to verify a Merkle proof for a data point
bool verifyMerkleProof(const std::string& rootHash, const std::vector<std::string>& proof, const std::string& dataHash, size_t dataIndex) {
  if (proof.empty()) {
    return rootHash == dataHash; // Single element case (no siblings)
  }

  std::string currentHash = dataHash;
  for (const std::string& siblingHash : proof) {
    if (dataIndex % 2 == 0) {
      currentHash = hashString(siblingHash + currentHash);
    } else {
      currentHash = hashString(currentHash + siblingHash);
    }
    dataIndex /= 2;
  }

  return currentHash == rootHash;
}

int main() {
  // Assuming you have data vectors from vectorization.cpp
  // You might need to adjust this based on how you call/access the data vectors
  std::vector<std::vector<double>> dataVectors = {/* data vectors */};

  // Construct the Merkle tree
  std::unique_ptr<MerkleTreeNode> root = constructMerkleTree(dataVectors);

  // Example usage: Get Merkle proof for a specific data point
  size_t dataIndex = 2; // Assuming valid data index (modify as needed)
  std::vector<std::string> proof = getMerkleProof(root, dataIndex, dataVectors);

  std::cout << "Merkle proof for data point " << dataIndex << ":" << std::endl;
  for (const std::string& hash : proof) {
    std::cout << hash << std::endl;
  }

  // Example usage: Verify Merkle proof (assuming you have the data hash)
  std::string dataHash = hashDataVector(dataVectors[dataIndex]); // Calculate data hash
  bool verified = verifyMerkleProof(root->hash, proof, dataHash, dataIndex);
  std::cout << "Merkle proof verification: " << (verified ? "Success" : "Failed") << std::endl;

  return 0;
}
