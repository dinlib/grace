#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <assert.h>

class StmtNode;
class VarDeclNode;

typedef std::vector<StmtNode *> StmtList;
typedef std::vector<VarDeclNode *> VarDeclNodeList;

class Node {
public:
  Node() {}
  virtual ~Node() {}
  virtual void DumpAST(unsigned level) const = 0;
};

class StmtNode : public Node {};

class ExprNode : public Node {};

class BlockNode : public Node {
public:
  StmtList stmts;
  BlockNode() {}
  void DumpAST(unsigned level) const override {
    std::cout << "aqui" << std::endl;
    for (auto stmt : stmts) {
      stmt->DumpAST(level + 1);
    }
  }
};

class AssignNode : public StmtNode {
public:
  std::string id;
  AssignNode(std::string id) : id(id) {}
};

class StringAssignNode : public AssignNode {
public:
  std::string str;
  StringAssignNode(std::string id, std::string str)
      : AssignNode(id), str(str) {}

  void DumpAST(unsigned level) const override {
    std::cout << "assign" << std::endl
              << "id: " << id << std::endl
              << "value: " << str << std::endl;
  }
};

class NumberAssignNode : public AssignNode {
public:
  int number;
  NumberAssignNode(std::string id, int number)
      : AssignNode(id), number(number) {}
  void DumpAST(unsigned level) const override {}
};

class ArrayAssignNode : public AssignNode {};

class SpecVar : public Node {
  std::string id;
  unsigned size;
  AssignNode *assign;

public:
  SpecVar(std::string id, unsigned size, AssignNode *assign)
      : id(id), size(size), assign(assign) {}

  void DumpAST(unsigned level) const override {}
};

typedef std::vector<SpecVar *> SpecVarList;

class VarDeclNode : public StmtNode {
  SpecVar *spec;
  std::string type;

public:
  VarDeclNode(SpecVar *spec, std::string type) : spec(spec), type(type) {}

  void DumpAST(unsigned level) const override {
    spec->DumpAST(level);
    std::cout << "type: " << type << std::endl;
  }
};

class VarDeclNodeListStmt : public StmtNode {
public:
  VarDeclNodeList varDeclList;
  void DumpAST(unsigned level) const override {
    for (const auto &varDecl : varDeclList)
      varDecl->DumpAST(level);
  }
};

class VarInitNode : public StmtNode {
  std::string id;
  std::string type;
  std::string value = "uninitialized";

public:
  VarInitNode(std::string &id, std::string &type) : id(id), type(type) {}

  void DumpAST(unsigned level) const override {
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

  void DumpAST(unsigned level) const override {
    std::cout << "function: " << name << std::endl
              << "return type: " << returnType << std::endl
              << "block: " << std::endl;

    block->DumpAST(level);

    std::cout << std::endl;
  }
};
