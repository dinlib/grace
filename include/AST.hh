
#pragma once

#include "BinOp.hh"
#include "llvm/IR/Value.h"
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace grace {

class Context;
class Type;

static std::string NestedLevel(unsigned level) {
  std::string str(level * 4, ' ');
  return str;
}

class StmtNode;

class VarDeclNode;

class SpecVar;

class LiteralNode;

class ExprNode;

typedef std::vector<StmtNode *> StmtList;
typedef std::vector<VarDeclNode *> VarDeclNodeList;
typedef std::vector<SpecVar *> SpecVarList;
typedef std::vector<LiteralNode *> LiteralNodeList;

class Param {
public:
  std::string Id;
  Type *Ty;
  bool IsArray;

  Param(std::string Id, Type *Ty, bool IsArray)
      : Id(std::move(Id)), Ty(Ty), IsArray(IsArray) {}
};

typedef std::vector<Param *> ParamList;

typedef std::vector<ExprNode *> ExprList;

class Node {
public:
  virtual ~Node() = default;

  virtual void dumpAST(std::ostream &os, unsigned level) const = 0;

  virtual llvm::Value *codegen(Context &C) = 0;
};

class StmtNode : public Node {};

class ExprNode : public Node {};

class LiteralNode : public ExprNode {};

class BlockNode : public Node {
public:
  StmtList stmts;

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(Body" << std::endl;
    for (auto stmt : stmts) {
      stmt->dumpAST(os, level + 1);
    }
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class AssignNode : public StmtNode {
protected:
  std::string Id;

public:
  AssignNode(std::string Id) : Id(std::move(Id)) {}
};

class AssignSimpleNode : public AssignNode {
public:
  ExprNode *Assign;

  AssignSimpleNode(const std::string &id, ExprNode *Assign)
      : AssignNode(id), Assign(Assign) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(Assign id: " << Id
       << "; value: " << std::endl;
    Assign->dumpAST(os, level + 1);
    os << std::endl << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class AssignArrayIdxNode : public AssignSimpleNode {
  ExprNode *Idx;

public:
  AssignArrayIdxNode(const std::string &Id, ExprNode *Assign, ExprNode *Idx)
      : AssignSimpleNode(Id, Assign), Idx(Idx) {}

  llvm::Value *codegen(Context &C) override;
};

class ArrayAssignNode : public AssignNode {
public:
  LiteralNodeList *literalList;

  ArrayAssignNode(const std::string &id,
                  std::vector<LiteralNode *> *literalList)
      : AssignNode(id), literalList(literalList) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(Assign id: " << Id << "; value: " << std::endl
       << NestedLevel(level + 1) << "[" << std::endl;

    for (auto lit : *literalList) {
      lit->dumpAST(os, level + 2);
      os << "," << std::endl;
    }

    os << NestedLevel(level + 1) << "]" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class LiteralIntNode : public LiteralNode {
public:
  int value;

  explicit LiteralIntNode(int value) : value(value) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << value << ")";
  }

  llvm::Value *codegen(Context &C) override;
};

class LiteralStringNode : public LiteralNode {
public:
  std::string Str;

  explicit LiteralStringNode(const std::string &Str) : Str(Str) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << Str << ")";
  }

  llvm::Value *codegen(Context &C) override;
};

class LiteralBoolNode : public LiteralNode {
public:
  bool BVal;

  explicit LiteralBoolNode(bool BVal) : BVal(BVal) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << std::boolalpha << BVal
       << ")";
  }

  llvm::Value *codegen(Context &C) override;
};

class SpecVar {
public:
  std::string Id;
  unsigned Size;
  AssignNode *Assign;

  SpecVar(std::string Id, unsigned Size, AssignNode *Assign)
      : Id(std::move(Id)), Size(Size), Assign(Assign) {}
};

class VarDeclNode : public StmtNode {
protected:
  std::string Id;
  AssignNode *Assign;
  Type *Ty;

public:
  VarDeclNode(std::string Id, AssignNode *Assign, Type *Ty)
      : Id(std::move(Id)), Assign(Assign), Ty(Ty) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(varDecl id: " << Id << "; type: " << Ty;

    if (Assign) {
      os << std::endl;
      Assign->dumpAST(os, level + 1);
      os << NestedLevel(level);
    }

    os << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

//    class ArrayDeclNode : public VarDeclNode {
//    public:
//        ArrayDeclNode(const std::string &Id, AssignNode *Assign, ArrayType
//        *Ty) : VarDeclNode(
//                Id, Assign, Ty), Size(Size) {}
//
//        llvm::Value *codegen(Context &C) override;
//    };

class VarDeclNodeListStmt : public StmtNode {
public:
  VarDeclNodeList varDeclList;

  void dumpAST(std::ostream &os, unsigned level) const override {
    for (const auto &varDecl : varDeclList)
      varDecl->dumpAST(os, level);
  }

  llvm::Value *codegen(Context &C) override;
};

class FuncDeclNode : public StmtNode {
  std::string Name;
  Type *ReturnTy;
  ParamList *Args;
  BlockNode *Body;

public:
  FuncDeclNode(std::string Name, Type *ReturnTy, ParamList *Args,
               BlockNode *Body)
      : Name(std::move(Name)), ReturnTy(ReturnTy), Args(Args), Body(Body) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(function Name: " << Name
       << "; ReturnType: " << ReturnTy << std::endl;
    Body->dumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ProcDeclNode : public StmtNode {
  std::string Name;
  ParamList *Args;
  BlockNode *Body;

public:
  ProcDeclNode(std::string Name, ParamList *Args, BlockNode *Body)
      : Name(std::move(Name)), Args(Args), Body(Body) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(function Name: " << Name << "; ReturnType: "
       << "void" << std::endl;
    Body->dumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class IfThenElseNode : public StmtNode {
  ExprNode *Condition;
  BlockNode *Then, *Else;

public:
  IfThenElseNode(ExprNode *Condition, BlockNode *Then, BlockNode *Else)
      : Condition(Condition), Then(Then), Else(Else) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(if" << std::endl;
    Condition->dumpAST(os, level + 1);
    os << std::endl;
    Then->dumpAST(os, level + 1);
    if (Else != nullptr)
      Else->dumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class VariableExprNode : public ExprNode {
protected:
  std::string Id;

public:
  VariableExprNode(std::string Id) : Id(std::move(Id)) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(var " << Id << " )" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ArrayVariableExprNode : public ExprNode {
protected:
  std::string Id;
  ExprNode *IdxExpr;

public:
  ArrayVariableExprNode(const std::string &Id, ExprNode *IdxExpr)
      : Id(Id), IdxExpr(IdxExpr) {}

  void dumpAST(std::ostream &os, unsigned level) const override {}

  llvm::Value *codegen(Context &C) override;
};

class ExprNegativeNode : public ExprNode {
  ExprNode *Expr;

public:
  explicit ExprNegativeNode(ExprNode *Expr) : Expr(Expr) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(-" << std::endl;
    Expr->dumpAST(os, level + 1);
    os << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ExprOperationNode : public ExprNode {
  ExprNode *LHS, *RHS;
  BinOp Op;

public:
  ExprOperationNode(ExprNode *LHS, BinOp Op, ExprNode *RHS)
      : LHS(LHS), Op(Op), RHS(RHS) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(expr" << std::endl;
    LHS->dumpAST(os, level + 1);
    //        os << Op << std::endl;
    RHS->dumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class CallExprNode : public ExprNode {
  std::string Callee;
  ExprList *Args;

public:
  CallExprNode(std::string Callee, ExprList *Args)
      : Callee(std::move(Callee)), Args(Args) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(call " << Callee << std::endl
       << NestedLevel(level + 1) << "args: " << std::endl;

    for (auto Arg : *Args) {
      Arg->dumpAST(os, level + 2);
      os << "," << std::endl;
    }

    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class WhileNode : public StmtNode {
  ExprNode *Condition;
  BlockNode *Block;

public:
  WhileNode(ExprNode *Condition, BlockNode *Block)
      : Condition(Condition), Block(Block) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(while" << std::endl;
    Condition->dumpAST(os, level + 1);
    os << std::endl;
    Block->dumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ForNode : public StmtNode {
  AssignNode *Start;
  ExprNode *End;
  AssignNode *Step;
  BlockNode *Body;

public:
  ForNode(AssignNode *Start, ExprNode *End, AssignNode *Step, BlockNode *Body)
      : Start(Start), End(End), Step(Step), Body(Body) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(for" << std::endl;
    Start->dumpAST(os, level + 1);
    os << std::endl;
    End->dumpAST(os, level + 1);
    os << std::endl;
    Step->dumpAST(os, level + 1);
    os << std::endl;
    Body->dumpAST(os, level + 1);
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ReturnNode : public StmtNode {
  ExprNode *expr;

public:
  explicit ReturnNode(ExprNode *expr) : expr(expr) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(return ";

    if (expr != nullptr) {
      os << std::endl;
      expr->dumpAST(os, level + 1);
      os << std::endl << NestedLevel(level) << ")" << std::endl;
    } else {
      os << ")" << std::endl;
    }
  }

  llvm::Value *codegen(Context &C) override;
};

class StopNode : public StmtNode {
public:
  StopNode() = default;

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(stop)" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class SkipNode : public StmtNode {
public:
  SkipNode() = default;

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(skip)" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class WriteNode : public StmtNode {
  ExprList *Exprs;

public:
  explicit WriteNode(ExprList *Exprs) : Exprs(Exprs) {}

  void dumpAST(std::ostream &os, unsigned level) const override {}

  llvm::Value *codegen(Context &C) override;
};
//
// class ReadNode : public StmtNode {
//
//};
}; // namespace grace