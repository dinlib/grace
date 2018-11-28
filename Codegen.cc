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

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
static AllocaInst *
CreateEntryBlockAlloca(Function *TheFunction, LLVMContext &TheContext, const std::string &Name, Type *T) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(T, nullptr, Name.c_str());
}

#define INT_SIZE sizeof(int)
#define BOOL_SIZE sizeof(bool)

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
    //  CondV = Builder.CreateICmpNE(CondV, ConstantInt::get(TheContext,
    //  APInt(INT_SIZE, 0)), "ifcond");
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
    if (F) {
        Log::error(0, 0) << "function " << Name << " already defined\n";
        return nullptr;
    }

    // Create vector with llvm types for args.
    std::vector<Type *> ArgsType;
    ArgsType.reserve(Args.size());

    for (auto &Arg : Args)
        ArgsType.push_back(C.getLLVMType(Arg.second));

    // Create function signature.
    FunctionType *FT =
            FunctionType::get(C.getLLVMType(ReturnType), ArgsType, false);
    F = Function::Create(FT, GlobalValue::LinkageTypes::ExternalLinkage, Name,
                         &C.getModule());

    // set args
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(Args[Idx++].first);

    BasicBlock *BB = BasicBlock::Create(C.getContext(), "entry", F);
    C.getBuilder().SetInsertPoint(BB);

    // insert args into scope
    for (auto &Arg : F->args()) {
        AllocaInst *Alloca = CreateEntryBlockAlloca(F, C.getContext(), Arg.getName(), Arg.getType());

        C.getBuilder().CreateStore(&Arg, Alloca);

        C.insertNamedValueIntoScope(Arg.getName(), Alloca);
    }

    // generate function body
    C.ExpectReturn = true;
    C.ReturnFound = false;
    Body->codegen(C);

    if (!C.ReturnFound) {
        Log::warning(0, 0) << "expected a return statement inside function body, but none was found.\n";
        return nullptr;
    }

    return nullptr;
}

Value *VarDeclNode::codegen(Context &C) {
    if (C.getNamedValueInScope(id)) {
        Log::error(0, 0) << "variable " << id << " already declared.\n";
        return nullptr;
    }

    Function *TheFunction = C.getBuilder().GetInsertBlock()->getParent();

    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, C.getContext(), id, C.getLLVMType(type));

    if (assign) {
        assign->codegen(C);
    }

    C.insertNamedValueIntoScope(id, Alloca);

    return nullptr;
}

Value *ReturnNode::codegen(Context &C) {
    C.ReturnFound = true;

    IRBuilder<> &Builder = C.getBuilder();

    if (expr)
        return Builder.CreateRet(expr->codegen(C));

    return Builder.CreateRetVoid();
}


Value *SkipNode::codegen(Context &C) {
    std::cout << "SkipNode unimplemented" << std::endl;
    return nullptr;
}

Value *StopNode::codegen(Context &C) {
    std::cout << "StopNode unimplemented" << std::endl;
    return nullptr;
}

Value *ForNode::codegen(Context &C) {
    std::cout << "ForNode unimplemented" << std::endl;
    return nullptr;
}

Value *WhileNode::codegen(Context &C) {
    std::cout << "WhileNode unimplemented" << std::endl;
    return nullptr;
}

Value *ExprIdentifierNode::codegen(Context &C) {
    AllocaInst *Alloca = C.getNamedValueInScope(id);
    if (!Alloca) {
        Log::error(0, 0) << "variable " << id << " not found.\n";
        return nullptr;
    }

    return C.getBuilder().CreateLoad(Alloca, id);
}

Value *VarInitNode::codegen(Context &C) {
    std::cout << "VarInitNode unimplemented" << std::endl;
    return nullptr;
}

Value *LiteralStringNode::codegen(Context &C) {
    std::cout << "LiteralStringNode unimplemented" << std::endl;
    return nullptr;
}

Value *ArrayAssignNode::codegen(Context &C) {
    std::cout << "ArrayAssignNode unimplemented" << std::endl;
    return nullptr;
}

Value *AssignSimpleNode::codegen(Context &C) {
    AllocaInst *Alloca = C.getNamedValueInScope(id);
    if (!Alloca)
        return nullptr;

    Value *Store = expr->codegen(C);

    C.getBuilder().CreateStore(Store, Alloca);

    return Store;
}

Value *VarDeclNodeListStmt::codegen(Context &C) {
    for (auto VarDecl : varDeclList)
        VarDecl->codegen(C);

    return nullptr;
}

Value *ExprNegativeNode::codegen(Context &C) {
    std::cout << "ExprNegativeNode unimplemented" << std::endl;
    return nullptr;
}

Value *ExprOperationNode::codegen(Context &C) {
    Value *LHS = expr1->codegen(C);
    Value *RHS = expr2->codegen(C);

    switch (Op) {
        case BinOp::PLUS:
            return C.getBuilder().CreateAdd(LHS, RHS, "addtmp");
        case BinOp::MINUS:
            return C.getBuilder().CreateSub(LHS, RHS, "subtmp");
        case BinOp::TIMES:
            return C.getBuilder().CreateMul(LHS, RHS, "multmp");
        case BinOp::DIV:
            return C.getBuilder().CreateFDiv(LHS, RHS, "divtmp");
        case BinOp::MOD:
            break;
        case BinOp::LT:
            break;
        case BinOp::LTEQ:
            break;
        case BinOp::GT:
            break;
        case BinOp::GTEQ:
            break;
        case BinOp::EQ:
            break;
        case BinOp::DIFF:
            break;
        case BinOp::AND:
            break;
        case BinOp::OR:
            break;
    }

    return nullptr;
}
