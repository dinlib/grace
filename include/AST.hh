#include <utility>


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

template <typename T>
static void safe_delete(T **ptr) {
//    if (*ptr) {
//        delete *ptr;
//        *ptr = nullptr;
//    }
}

static std::string NestedLevel(unsigned level) {
  std::string str(level * 4, ' ');
  return str;
}

static std::string to_string(const BinOp &Op) {
  switch (Op) {
  case BinOp::PLUS:
    return "+";
  case BinOp::MINUS:
    return "-";
  case BinOp::TIMES:
    return "*";
  case BinOp::DIV:
    return "/";
  case BinOp::MOD:
    return "%";
  case BinOp::LT:
    return "<";
  case BinOp::LTEQ:
    return "<=";
  case BinOp::GT:
    return ">";
  case BinOp::GTEQ:
    return ">=";
  case BinOp::EQ:
    return "==";
  case BinOp::DIFF:
    return "!=";
  case BinOp::AND:
    return "&&";
  case BinOp::OR:
    return "||";
  }
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

  Param(std::string Id, Type *Ty) : Id(std::move(Id)), Ty(Ty) {}
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
    ~BlockNode() override {
        for (auto Stmt : Stmts)
            safe_delete(&Stmt);
    }

    StmtList Stmts;

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(Body" << std::endl;
    for (auto Stmt : Stmts) {
      Stmt->dumpAST(os, level + 1);
    }
    os << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class AssignNode : public StmtNode {
protected:
  std::string Id;
  ExprNode *Assign;

public:
  AssignNode(std::string Id, ExprNode *Assign)
      : Id(std::move(Id)), Assign(Assign) {}

    ~AssignNode() override {
        safe_delete(&Assign);
    }

    void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(Assign id: " << Id
       << "; value: " << std::endl;
    Assign->dumpAST(os, level + 1);
    os << std::endl << NestedLevel(level) << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class CompoundAssignNode : public AssignNode {
public:
  BinOp Op;

  CompoundAssignNode(std::string id, BinOp Op, ExprNode *Assign)
      : AssignNode(id, Assign), Op(Op) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(assing id: " << Id
       << "; value: " << std::endl;
    os << to_string(Op) << std::endl;
    Assign->dumpAST(os, level + 1);
  }

  llvm::Value *codegen(Context &C) override;
};

class LiteralIntNode : public LiteralNode {
public:
  int IVal;

  explicit LiteralIntNode(int value) : IVal(value) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(literal value: " << IVal << ")";
  }

  llvm::Value *codegen(Context &C) override;
};

class LiteralStringNode : public LiteralNode {
public:
  std::string Str;

  explicit LiteralStringNode(std::string Str) : Str(std::move(Str)) {}

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
  AssignNode *Assign;

  SpecVar(std::string Id, AssignNode *Assign)
      : Id(std::move(Id)), Assign(Assign) {}

      ~SpecVar() {
          safe_delete(&Assign);
  }
};

class VarDeclNode : public StmtNode {
protected:
  std::string Id;
  AssignNode *Assign;
  Type *Ty;

public:
  VarDeclNode(std::string Id, AssignNode *Assign, Type *Ty)
      : Id(std::move(Id)), Assign(Assign), Ty(Ty) {}

      ~VarDeclNode() override {
          safe_delete(&Assign);
          safe_delete(&Ty);
  }

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

class VarDeclNodeListStmt : public StmtNode {
public:
  VarDeclNodeList varDeclList;

    ~VarDeclNodeListStmt() override {
        for (auto Decl : varDeclList)
            safe_delete(&Decl);
    }

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

    ~FuncDeclNode() override  {
        safe_delete(&ReturnTy);
        safe_delete(&Body);

        for (auto Param : *Args)
            safe_delete(&Param);

        safe_delete(&Args);
    }

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

    ~ProcDeclNode() override {
        safe_delete(&Body);

        for (auto Param : *Args)
            safe_delete(&Param);

        safe_delete(&Args);
    }

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

    ~IfThenElseNode() override {
        safe_delete(&Condition);
        safe_delete(&Then);
        safe_delete(&Else);
    }

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
  explicit VariableExprNode(std::string Id) : Id(std::move(Id)) {}

  void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(var " << Id << " )" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ExprNegativeNode : public ExprNode {
  ExprNode *RHS;

public:
  explicit ExprNegativeNode(ExprNode *RHS) : RHS(RHS) {}

    ~ExprNegativeNode() override {
    safe_delete(&RHS);
    }

    void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(-" << std::endl;
    RHS->dumpAST(os, level + 1);
    os << ")" << std::endl;
  }

  llvm::Value *codegen(Context &C) override;
};

class ExprNotNode : public ExprNode {
  ExprNode *RHS;

public:
  ExprNotNode(ExprNode *RHS) : RHS(RHS) {}

    ~ExprNotNode() override {
    safe_delete(&RHS);
    }

    void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(NOT " << std::endl;
    RHS->dumpAST(os, level + 1);
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

    ~ExprOperationNode() override {
        safe_delete(&LHS);
        safe_delete(&RHS);
    }

    void dumpAST(std::ostream &os, unsigned level) const override {
    os << NestedLevel(level) << "(expr" << std::endl;
    LHS->dumpAST(os, level + 1);
    os << to_string(Op) << std::endl;
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

    ~CallExprNode() override {
        for (auto Arg : *Args)
            safe_delete(&Arg);
        safe_delete(&Args);
    }

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

    ~WhileNode() override {
        safe_delete(&Condition);
        safe_delete(&Block);
    }

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

    ~ForNode() override {
        safe_delete(&Start);
        safe_delete(&End);
        safe_delete(&Step);
        safe_delete(&Body);
    }

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

    ~ReturnNode() override {
        safe_delete(&expr);
    }

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

    ~WriteNode() override {
        for (auto Expr : *Exprs)
            safe_delete(&Expr);
        safe_delete(&Exprs);
    }

    void dumpAST(std::ostream &os, unsigned level) const override {}

  llvm::Value *codegen(Context &C) override;
};

}; // namespace grace