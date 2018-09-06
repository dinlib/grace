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
  void PrintAST() const {}
};

class ExprNode : public Node {};

class BlockNode : public Node {
public:
  StmtList stmts;
  BlockNode() {}
  virtual void PrintAST() const {
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

  virtual void PrintAST() const {
    std::cout << "variable: " << id << std::endl
              << "size: " << size << std::endl
              << "type: " << type << std::endl
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

  void PrintAST() const {
    std::cout << "function: " << name << std::endl
              << "return type: " << returnType << std::endl
              << "block: " << std::endl;

    block->PrintAST();

    std::cout << std::endl;
  }
};