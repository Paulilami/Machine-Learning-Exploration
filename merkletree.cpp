#include <iostream>
#include <vector>
#include <string>
#include <memory> 
#include <stdexcept> 
#include <crypto++/sha.h> 
#include <crypto++/hex.h>
#include "vectorization.cpp"
std::string hashString(const std::string& data) {
  CryptoPP::SHA256 hash;
  byte digest[CryptoPP::SHA256::DIGESTSIZE];

  hash.CalculateDigest(digest, reinterpret_cast<const byte*>(data.c_str()), data.length());

  std::string hashedData;
  CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hashedData));
  encoder.Put(digest, sizeof(digest));
  encoder.MessageEnd();

  return hashedData;
}

std::string hashDataVector(const std::vector<double>& dataVector) {
  int vectorSize = dataVector.size();
  std::vector<byte> dataBytes(vectorSize * sizeof(double));
  std::memcpy(dataBytes.data(), dataVector.data(), vectorSize * sizeof(double));

  return hashString(std::string(dataBytes.begin(), dataBytes.end())); // Use hashString for consistency
}

struct MerkleTreeNode {
  std::string hash;
  std::unique_ptr<MerkleTreeNode> left;
  std::unique_ptr<MerkleTreeNode> right;

  MerkleTreeNode(const std::string& hash) : hash(hash), left(nullptr), right(nullptr) {}
};

std::unique_ptr<MerkleTreeNode> constructMerkleTree(const std::vector<std::vector<double>>& dataVectors) {
  if (dataVectors.empty()) {
    throw std::runtime_error("Empty data provided for Merkle tree construction");
  }

  if (dataVectors.size() == 1) {
    return std::make_unique<MerkleTreeNode>(hashDataVector(dataVectors[0])); // Leaf node with hashed data vector
  }

  std::vector<std::vector<double>> leftHalf(dataVectors.begin(), dataVectors.begin() + dataVectors.size() / 2);
  std::vector<std::vector<double>> rightHalf(dataVectors.begin() + dataVectors.size() / 2, dataVectors.end());
  std::unique_ptr<MerkleTreeNode> leftNode = constructMerkleTree(leftHalf);
  std::unique_ptr<MerkleTreeNode> rightNode = constructMerkleTree(rightHalf);

  std::string parentHash = hashString(leftNode->hash + rightNode->hash);
  return std::make_unique<MerkleTreeNode>(parentHash, std::move(leftNode), std::move(rightNode));
}

std::vector<std::string> getMerkleProof(const std::unique_ptr<MerkleTreeNode>& root, size_t dataIndex, const std::vector<std::vector<double>>& dataVectors) {
  if (!root || dataIndex >= dataVectors.size()) {
    throw std::invalid_argument("Invalid root node or data index for proof generation");
  }

  std::vector<std::string> proof;
  std::unique_ptr<MerkleTreeNode> currentNode = root.get();
  size_t siblingIndex;

  while (currentNode->left && currentNode->right) {
    siblingIndex = (dataIndex % 2 == 0) ? dataIndex + 1 : dataIndex - 1;
    if (dataIndex < siblingIndex) {
      proof.push_back(currentNode->right->hash); 
      currentNode = currentNode->left.get();
    } else {
      proof.push_back(currentNode->left->hash);
      currentNode = currentNode->right.get();
    }
    dataIndex /= 2;
  }

  if (!currentNode->left && !currentNode->right) {
    proof.clear();
  }

  return proof;
}

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
  
  std::vector<std::vector<double>> dataVectors = {/* data vectors */};

  std::unique_ptr<MerkleTreeNode> root = constructMerkleTree(dataVectors);

  
  size_t dataIndex = 2;
  std::vector<std::string> proof = getMerkleProof(root, dataIndex, dataVectors);

  std::cout << "Merkle proof for data point " << dataIndex << ":" << std::endl;
  for (const std::string& hash : proof) {
    std::cout << hash << std::endl;
  }

  return 0;
}
