#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <assert.h>

using namespace std;

static string NestedLevel(unsigned level) {
  string str;
  for (unsigned i = 0; i < level * 4; i++)
    str += " ";
  return str;
}

class StmtNode;
class VarDeclNode;

typedef vector<StmtNode *> StmtList;
typedef vector<VarDeclNode *> VarDeclNodeList;

class Node {
public:
  virtual ~Node() = default;
  virtual void DumpAST(unsigned level) const = 0;
};

class StmtNode : public Node {};
class ExprNode : public Node {};

class BlockNode : public Node {
public:
  StmtList stmts;
  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(block" << endl;
    for (auto stmt : stmts) {
      stmt->DumpAST(level + 1);
    }
    cout << NestedLevel(level) << ")" << endl;
  }
};

class AssignNode : public StmtNode {
public:
  string id;
  AssignNode(const string &id) : id(id) {}
};

class StringAssignNode : public AssignNode {
public:
  string str;
  StringAssignNode(const string &id, const string &str)
      : AssignNode(id), str(str) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(assign id: " << id << "; value: " << str
         << ")" << endl;
  }
};

class NumberAssignNode : public AssignNode {
public:
  int number;
  NumberAssignNode(const string &id, int number)
      : AssignNode(id), number(number) {}
  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(assign id: " << id << "; value: " << number
         << ")" << endl;
  }
};

class ArrayAssignNode : public AssignNode {};

class SpecVar {
public:
  string id;
  unsigned size;
  AssignNode *assign;

  SpecVar(const string &id, unsigned size, AssignNode *assign)
      : id(id), size(size), assign(assign) {}
};

typedef vector<SpecVar *> SpecVarList;

class VarDeclNode : public StmtNode {
  string id;
  unsigned size;
  AssignNode *assign;
  string type;

public:
  VarDeclNode(const string &id, unsigned size, AssignNode *assign,
              const string &type)
      : id(id), size(size), assign(assign), type(type) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(varDecl id: " << id
         << "; size: " << size << "; type: " << type;

    if (assign) {
      cout << endl;
      assign->DumpAST(level + 1);
      cout << NestedLevel(level);
    }

    cout << ")" << endl;
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
  string id;
  string type;
  string value = "uninitialized";

public:
  VarInitNode(const string &id, const string &type) : id(id), type(type) {}

  void DumpAST(unsigned level) const override {
    cout << "variable: " << id << endl
         << "type: " << type << endl
         << "value: " << value << endl
         << endl;
  }
};

class FuncDeclNode : public StmtNode {
  string name;
  string returnType;
  BlockNode *block;

public:
  FuncDeclNode(const string &name, const string &returnType, BlockNode *block)
      : name(name), returnType(returnType), block(block) {}
  FuncDeclNode(const string &name, BlockNode *block)
      : name(name), returnType("void"), block(block) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(function name: " << name
         << "; returnType: " << returnType << endl;
    block->DumpAST(level + 1);
    cout << NestedLevel(level) << ")" << endl;
  }
};
