#pragma once

#include "Context.hh"
#include "llvm/IR/Module.h"
#include <fstream>
#include <string>
#include <vector>
#include "BinOp.hh"

using namespace llvm;

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

    virtual void dumpAST(std::ostream &os, unsigned level) const = 0;

    virtual Value *codegen(Context &C) = 0;
};

class StmtNode : public Node {
};

class ExprNode : public Node {
};

class LiteralNode : public ExprNode {
};

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

    Value *codegen(Context &C) override;
};

class AssignNode : public StmtNode {
public:
    std::string Id;

    AssignNode(std::string Id) : Id(std::move(Id)) {}
};

class AssignSimpleNode : public AssignNode {
public:
    ExprNode *expr;

    AssignSimpleNode(const std::string &id, ExprNode *expr)
            : AssignNode(id), expr(expr) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(Assign id: " << Id
           << "; value: " << std::endl;
        expr->dumpAST(os, level + 1);
        os << std::endl << NestedLevel(level) << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class CompoundAssignNode : public AssignNode {
public:
    ExprNode *expr;
    BinOp Op;

    CompoundAssignNode(const std::string &id, BinOp Op, ExprNode *expr)
        : AssignNode(id), Op(Op), expr(expr) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(assing id: " << Id
            << "; value: " << std::endl;
//        os << Op << std::endl;
        expr->dumpAST(os, level + 1);
    }

    Value *codegen(Context &C) override;
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

    Value *codegen(Context &C) override;
};

class LiteralIntNode : public LiteralNode {
public:
    int value;

    LiteralIntNode(int value) : value(value) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(literal value: " << value << ")";
    }

    Value *codegen(Context &C) override;
};

class LiteralStringNode : public LiteralNode {
public:
    std::string str;

    LiteralStringNode(const std::string &str) : str(str) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(literal value: " << str << ")";
    }

    Value *codegen(Context &C) override;
};

class LiteralBoolNode : public LiteralNode {
public:
    bool BVal;

    LiteralBoolNode(bool BVal) : BVal(BVal) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(literal value: " << std::boolalpha << BVal
           << ")";
    }

    Value *codegen(Context &C) override;
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
    std::string Id;
    unsigned Size;
    AssignNode *Assign;
    std::string Type;

public:
    VarDeclNode(std::string Id, unsigned Size, AssignNode *Assign,
                std::string Type)
            : Id(std::move(Id)), Size(Size), Assign(Assign), Type(std::move(Type)) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(varDecl id: " << Id << "; Size: " << Size
           << "; type: " << Type;

        if (Assign) {
            os << std::endl;
            Assign->dumpAST(os, level + 1);
            os << NestedLevel(level);
        }

        os << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class VarDeclNodeListStmt : public StmtNode {
public:
    VarDeclNodeList varDeclList;

    void dumpAST(std::ostream &os, unsigned level) const override {
        for (const auto &varDecl : varDeclList)
            varDecl->dumpAST(os, level);
    }

    Value *codegen(Context &C) override;
};

class FuncDeclNode : public StmtNode {
    std::string Name;
    std::string ReturnType;
    std::vector<std::tuple<std::string, std::string, bool>> Args; // <id, type, is_array>
    BlockNode *Body;

public:
    FuncDeclNode(std::string Name, std::string ReturnType,
                 BlockNode *Body)
            : Name(std::move(Name)), ReturnType(std::move(ReturnType)), Body(Body) {}

    FuncDeclNode(std::string Name, BlockNode *Body)
            : Name(std::move(Name)), ReturnType("void"), Body(Body) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(function Name: " << Name
           << "; ReturnType: " << ReturnType << std::endl;
        Body->dumpAST(os, level + 1);
        os << NestedLevel(level) << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class IfThenElseNode : public StmtNode {
    ExprNode *condition;
    BlockNode *thenBlock, *elseBlock;

public:
    IfThenElseNode(ExprNode *condition, BlockNode *thenBlock,
                   BlockNode *elseBlock)
            : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(if" << std::endl;
        condition->dumpAST(os, level + 1);
        os << std::endl;
        thenBlock->dumpAST(os, level + 1);
        if (elseBlock != NULL)
            elseBlock->dumpAST(os, level + 1);
        os << NestedLevel(level) << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class ExprIdentifierNode : public ExprNode {
    std::string id;

public:
    ExprIdentifierNode(std::string id) : id(id) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(var " << id << " )" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class ExprNegativeNode : public ExprNode {
    ExprNode *expr;

public:
    ExprNegativeNode(ExprNode *expr) : expr(expr) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(-" << std::endl;
        expr->dumpAST(os, level + 1);
        os << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
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

    Value *codegen(Context &C) override;
};

class CallExprNode : public ExprNode {
    std::string Callee;
    std::vector<ExprNode *> Args;

public:
    CallExprNode(const std::string &Callee, std::vector<ExprNode *> Args) : Callee(Callee), Args(std::move(Args)) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        // TODO: implement
    }

    Value *codegen(Context &C) override;
};

class WhileNode : public StmtNode {
    ExprNode *condition;
    BlockNode *whileBlock;

public:
    WhileNode(ExprNode *condition, BlockNode *whileBlock)
            : condition(condition), whileBlock(whileBlock) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(while" << std::endl;
        condition->dumpAST(os, level + 1);
        os << std::endl;
        whileBlock->dumpAST(os, level + 1);
        os << NestedLevel(level) << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class ForNode : public StmtNode {
    VarDeclNodeListStmt *initialization;
    ExprNode *condition, *step;
    BlockNode *forBlock;

public:
    ForNode(VarDeclNodeListStmt *initialization, ExprNode *condition,
            ExprNode *step, BlockNode *forBlock)
            : initialization(initialization), condition(condition), step(step),
              forBlock(forBlock) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(for" << std::endl;
        initialization->dumpAST(os, level + 1);
        os << std::endl;
        condition->dumpAST(os, level + 1);
        os << std::endl;
        step->dumpAST(os, level + 1);
        os << std::endl;
        forBlock->dumpAST(os, level + 1);
        os << NestedLevel(level) << ")" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class ReturnNode : public StmtNode {
    ExprNode *expr;

public:
    explicit ReturnNode(ExprNode *expr) : expr(expr) {}

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(return ";

        if (expr != NULL) {
            os << std::endl;
            expr->dumpAST(os, level + 1);
            os << std::endl << NestedLevel(level) << ")" << std::endl;
        } else {
            os << ")" << std::endl;
        }
    }

    Value *codegen(Context &C) override;
};

class StopNode : public StmtNode {
public:
    StopNode() = default;

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(stop)" << std::endl;
    }

    Value *codegen(Context &C) override;
};

class SkipNode : public StmtNode {
public:
    SkipNode() = default;

    void dumpAST(std::ostream &os, unsigned level) const override {
        os << NestedLevel(level) << "(skip)" << std::endl;
    }

    Value *codegen(Context &C) override;
};
