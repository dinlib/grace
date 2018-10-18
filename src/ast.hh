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
  virtual void DumpAST(unsigned level) const {}
};

class StmtNode : public Node {};
class ExprNode : public Node {};

class StringExprNode : public ExprNode {
public:
  string str;
  StringExprNode(const string &str) : str(str) {}
  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(expr value: " << str << ")" << endl;
  }
};

class IdentifierExprNode : public ExprNode {
public:
  string id;
  IdentifierExprNode(const string &id) : id(id) {}
  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(expr value: " << id << ")" << endl;
  }
};

class NumberExprNode : public ExprNode {
public:
  int value;
  NumberExprNode(int value) : value(value) {}
  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(expr value: " << value << ")" << endl;
  }
};

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

class AssignSimpleNode : public AssignNode {
public:
  ExprNode *expr;
  AssignSimpleNode(const string &id, ExprNode *expr)
      : AssignNode(id), expr(expr) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(assign id: " << id << "; value: " << endl;
    expr->DumpAST(level + 1);
    cout << NestedLevel(level) << ")" << endl;
  }
};

class LiteralNode : public ExprNode {};

class ArrayAssignNode : public AssignNode {
public:
  vector<LiteralNode *> *literalList;
  ArrayAssignNode(const string &id, vector<LiteralNode *> *literalList)
      : AssignNode(id), literalList(literalList) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(assign id: " << id << "; value: " << endl
         << NestedLevel(level + 1) << "[" << endl;

    for (auto lit : *literalList) {
      lit->DumpAST(level + 2);
      cout << "," << endl;
    }

    cout << NestedLevel(level + 1) << "]" << endl;
  }
};

class LiteralIntNode : public LiteralNode {
public:
  int value;
  LiteralIntNode(int value) : value(value) {}
  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(literal value: " << value << ")";
  }
};

class LiteralStringNode : public LiteralNode {
public:
  string str;
  LiteralStringNode(const string &str) : str(str) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(literal value: " << str << ")";
  }
};

class LiteralBoolNode : public LiteralNode {
public:
  bool b;
  LiteralBoolNode(bool b) : b(b) {}

  void DumpAST(unsigned level) const override {
    cout << NestedLevel(level) << "(literal value: " << boolalpha << b << ")";
  }
};

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
    cout << NestedLevel(level) << "(varDecl id: " << id << "; size: " << size
         << "; type: " << type;

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
