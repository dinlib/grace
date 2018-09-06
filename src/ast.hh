#pragma once

#include <iostream>
#include <string>
#include <vector>

class Node {
public:
  Node() {}
  virtual ~Node() {}
  virtual void PrintAST() const {}
};

class BlockNode : public Node {
public:
  std::vector<Node *> nodes;
  BlockNode() {}
  virtual void PrintAST() const {
    for (const auto &node : nodes)
      node->PrintAST();
  }
};

class VarDeclNode : public Node {
  std::string id;
  unsigned size;
  std::string type;

public:
  VarDeclNode(std::string &id, unsigned size, std::string &type)
      : id(id), size(size), type(type) {}

  virtual void PrintAST() const {
    std::cout << "variable declaration with name: " << id << std::endl
              << "size: " << size << std::endl
              << "type: " << type << std::endl
              << std::endl;
  }
};
