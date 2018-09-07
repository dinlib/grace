#pragma once

#include <iostream>
#include <string>
#include <vector>

class StmtNode;

typedef std::vector<StmtNode *> StmtList;

class Node {
public:
  Node() {}
  virtual ~Node() {}
  virtual void PrintAST() const {}
};

class StmtNode : public Node {
public:
  void PrintAST() const override {}
};

class ExprNode : public Node {};

class BlockNode : public Node {
public:
  StmtList stmts;
  BlockNode() {}
  void PrintAST() const override {
    for (auto &stmt : stmts)
      stmt->PrintAST();
  }
};

class VarDeclNode : public StmtNode {
  std::string id;
  unsigned size;
  std::string type;

public:
  VarDeclNode(std::string &id, unsigned size, std::string &type)
      : id(id), size(size), type(type) {}

  void PrintAST() const override {
    std::cout << "variable: " << id << std::endl
              << "size: " << size << std::endl
              << "type: " << type << std::endl
              << std::endl;
  }
};

class VarInitNode : public StmtNode {
  std::string id;
  std::string type;
  std::string value = "uninitialized";

public:
  VarInitNode(std::string &id, std::string &type) 
    : id(id), type(type) {}

  void PrintAST() const override {
    std::cout << "variable: " << id << std::endl
              << "type: " << type << std::endl
              << "value: " << value << std::endl
              << std::endl;
  }
};

class FuncDeclNode : public StmtNode {
  std::string name;
  std::string returnType;
  BlockNode *block;

public:
  FuncDeclNode(std::string &name, std::string &returnType, BlockNode *block)
      : name(name), returnType(returnType), block(block) {}

  void PrintAST() const override {
    std::cout << "function: " << name << std::endl
              << "return type: " << returnType << std::endl
              << "block: " << std::endl;

    block->PrintAST();

    std::cout << std::endl;
  }
};
