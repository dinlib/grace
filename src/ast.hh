#pragma once

#include "context.hh"
#include <fstream>
#include <string>
#include <vector>

static std::string NestedLevel(unsigned level) {
  std::string str(level * 4, ' ');
  return str;
}

class StmtNode;
class VarDeclNode;
class SpecVar;
class LiteralNode;

typedef std::vector<StmtNode *> StmtList;
typedef std::vector<VarDeclNode *> VarDeclNodeList;
typedef std::vector<SpecVar *> SpecVarList;
typedef std::vector<LiteralNode *> LiteralNodeList;

class Node {
public:
  virtual ~Node() = default;
  virtual void DumpAST(std::ostream &os, unsigned level) const = 0;
  // virtual void SemanticAnalyses(Context &context) = 0;
  // virtual void CodeGen(llvm::Module &M) = 0;
};

class StmtNode : public Node {};
class ExprNode : public Node {};
class LiteralNode : public ExprNode {};

class BlockNode : public Node {
public:
  StmtList stmts;
  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(block" << std::endl;
    for (auto stmt : stmts) {
      stmt->DumpAST(os, level + 1);
    }
    os << NestedLevel(level) << ")" << std::endl;
  }
};

class AssignNode : public StmtNode {
public:
  std::string id;
  AssignNode(const std::string &id) : id(id) {}
};

class AssignSimpleNode : public AssignNode {
public:
  ExprNode *expr;
  AssignSimpleNode(const std::string &id, ExprNode *expr)
      : AssignNode(id), expr(expr) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(assign id: " << id
       << "; value: " << std::endl;
    expr->DumpAST(os, level + 1);
    os << std::endl << NestedLevel(level) << ")" << std::endl;
  }
};

class ArrayAssignNode : public AssignNode {
public:
  LiteralNodeList *literalList;
  ArrayAssignNode(const std::string &id,
                  std::vector<LiteralNode *> *literalList)
      : AssignNode(id), literalList(literalList) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(assign id: " << id << "; value: " << std::endl
       << NestedLevel(level + 1) << "[" << std::endl;

    for (auto lit : *literalList) {
      lit->DumpAST(os, level + 2);
      os << "," << std::endl;
    }

    os << NestedLevel(level + 1) << "]" << std::endl;
  }
};

class LiteralIntNode : public LiteralNode {
public:
  int value;
  LiteralIntNode(int value) : value(value) {}
  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << value << ")";
  }
};

class LiteralStringNode : public LiteralNode {
public:
  std::string str;
  LiteralStringNode(const std::string &str) : str(str) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << str << ")";
  }
};

class LiteralBoolNode : public LiteralNode {
public:
  bool b;
  LiteralBoolNode(bool b) : b(b) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << std::boolalpha << b
       << ")";
  }
};

class SpecVar {
public:
  std::string id;
  unsigned size;
  AssignNode *assign;

  SpecVar(const std::string &id, unsigned size, AssignNode *assign)
      : id(id), size(size), assign(assign) {}
};

class VarDeclNode : public StmtNode {
  std::string id;
  unsigned size;
  AssignNode *assign;
  std::string type;

public:
  VarDeclNode(const std::string &id, unsigned size, AssignNode *assign,
              const std::string &type)
      : id(id), size(size), assign(assign), type(type) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(varDecl id: " << id << "; size: " << size
       << "; type: " << type;

    if (assign) {
      os << std::endl;
      assign->DumpAST(os, level + 1);
      os << NestedLevel(level);
    }

    os << ")" << std::endl;
  }
};

class VarDeclNodeListStmt : public StmtNode {
public:
  VarDeclNodeList varDeclList;
  void DumpAST(std::ostream &os, unsigned level) const override {
    for (const auto &varDecl : varDeclList)
      varDecl->DumpAST(os, level);
  }
};

class VarInitNode : public StmtNode {
  std::string id;
  std::string type;
  std::string value = "uninitialized";

public:
  VarInitNode(const std::string &id, const std::string &type)
      : id(id), type(type) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << "variable: " << id << std::endl
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
  FuncDeclNode(const std::string &name, const std::string &returnType,
               BlockNode *block)
      : name(name), returnType(returnType), block(block) {}
  FuncDeclNode(const std::string &name, BlockNode *block)
      : name(name), returnType("void"), block(block) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(function name: " << name
       << "; returnType: " << returnType << std::endl;
    block->DumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }
};

class IfThenElseNode : public StmtNode {
  ExprNode *condition;
  BlockNode *thenBlock, *elseBlock;

public:
  IfThenElseNode(ExprNode *condition, BlockNode *thenBlock,
                 BlockNode *elseBlock)
      : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(if" << std::endl;
    condition->DumpAST(os, level + 1);
    os << std::endl;
    thenBlock->DumpAST(os, level + 1);
    if (elseBlock != NULL)
      elseBlock->DumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }
};

class WhileNode : public StmtNode {
  ExprNode *condition;
  BlockNode *whileBlock;

public:
  WhileNode(ExprNode *condition, BlockNode *whileBlock)
      : condition(condition), whileBlock(whileBlock) {}

  void DumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(while" << std::endl;
    condition->DumpAST(os, level + 1);
    os << std::endl;
    whileBlock->DumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }
};
