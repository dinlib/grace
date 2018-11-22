
#include <AST.hh>

#include "llvm/IR/Verifier.h"

using namespace llvm;

/// LogError* - These are little helper functions for error handling.
Node *LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}

#define INT_SIZE sizeof(int)
#define BOOL_SIZE 1

Value *BlockNode::codegen(Context &C) {
    for (auto &stmt : stmts)
        stmt->codegen(C);

    return nullptr;
}

Value *LiteralIntNode::codegen(Context &C) {
    return ConstantInt::get(C.getContext(), APInt(INT_SIZE, value));
}

Value *LiteralBoolNode::codegen(Context &C) {
    LLVMContext &TheContext = C.getContext();
    return ConstantInt::get(TheContext, APInt(BOOL_SIZE, BVal ? 1 : 0));
}

Value *IfThenElseNode::codegen(Context &C) {
//  IRBuilder<> &Builder = C.getBuilder();
//  LLVMContext &TheContext = C.getContext();
//
//  Value *CondV = condition->codegen(C);
//  if (!CondV) // TODO: log error
//    return nullptr;
//
//  CondV = Builder.CreateICmpNE(CondV, ConstantInt::get(TheContext, APInt(INT_SIZE, 0)), "ifcond");
//
//  Function *TheFunction = Builder.GetInsertBlock()->getParent();
//
//  BasicBlock *ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
//  BasicBlock *ElseBB = BasicBlock::Create(TheContext, "else", TheFunction);
//  BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont");
//
//  Builder.CreateCondBr(CondV, ThenBB, ElseBB);
//
//  Builder.SetInsertPoint(ThenBB);
//
//  Value *ThenV = thenBlock->codegen(C);
//  if (!ThenV) // TODO: log error
//    return nullptr;
//
//  Builder.CreateBr(MergeBB);
//
//  ThenBB = Builder.GetInsertBlock();
//
//  TheFunction->getBasicBlockList().push_back(ElseBB);
//  Builder.SetInsertPoint(MergeBB);

    return nullptr;
}

Value *FuncDeclNode::codegen(Context &C) {
    Function *F = C.getModule().getFunction(Name);

    // Check if function is already defined.
    if (F)
        return LogErrorV("function already defined");

    // Create vector with llvm types for args.
    std::vector<Type *> ArgsType;
    ArgsType.reserve(Args.size());

    for (auto &Arg : Args)
        ArgsType.push_back(C.getLLVMType(Arg.second));

    // Create function signature.
    FunctionType *FT = FunctionType::get(C.getLLVMType(ReturnType), ArgsType, false);
    F = Function::Create(FT, GlobalValue::LinkageTypes::ExternalLinkage, Name, &C.getModule());

    // set args
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(Args[Idx++].first);

    BasicBlock *BB = BasicBlock::Create(C.getContext(), "entry", F);
    C.getBuilder().SetInsertPoint(BB);

    // insert args into scope
    for (auto &Arg : F->args()) {
        C.insertNamedValueIntoScope(Arg.getName(), &Arg);
    }

    // generate function body
    Body->codegen(C);

    return nullptr;
}

Value *ReturnNode::codegen(Context &C) {
    IRBuilder<> &Builder = C.getBuilder();

    if (expr)
        return Builder.CreateRet(expr->codegen(C));

    return Builder.CreateRetVoid();
}
