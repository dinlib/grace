#pragma once

#include <iostream>
#include <string>
#include <vector>

class StmtNode;
class VarDeclNode;

typedef std::vector<StmtNode *> StmtList;
typedef std::vector<VarDeclNode *> VarDeclNodeList;

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

class AssignNode : public StmtNode {};

class StringAssignNode : public AssignNode {
  std::string str;

public:
  StringAssignNode(std::string str) : str(str) {}
};

class NumberAssignNode : public AssignNode {};

class ArrayAssignNode : public AssignNode {};

class SpecVar {
  std::string id;
  unsigned size;
  AssignNode *assign;

public:
  SpecVar(std::string id, unsigned size, AssignNode *assign)
      : id(id), size(size), assign(assign) {}
};

typedef std::vector<SpecVar *> SpecVarList;

class VarDeclNode : public StmtNode {
  SpecVar *spec;
  std::string type;

public:
  VarDeclNode(SpecVar *spec, std::string type) : spec(spec), type(type) {}

  void PrintAST() const override {
    // std::cout << "variable: " << id << std::endl
    //           << "size: " << size << std::endl
    //           << "type: " << type << std::endl
    //           << std::endl;
  }
};

class VarDeclNodeListStmt : public StmtNode {
  VarDeclNodeList varDeclList;

public:
  VarDeclNodeListStmt(VarDeclNodeList list) : varDeclList(list) {}
};

class VarInitNode : public StmtNode {
  std::string id;
  std::string type;
  std::string value = "uninitialized";

public:
  VarInitNode(std::string &id, std::string &type) : id(id), type(type) {}

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
  FuncDeclNode(std::string &name, BlockNode *block)
      : name(name), returnType("void"), block(block) {}

  void PrintAST() const override {
    std::cout << "function: " << name << std::endl
              << "return type: " << returnType << std::endl
              << "block: " << std::endl;

    block->PrintAST();

    std::cout << std::endl;
  }
};
