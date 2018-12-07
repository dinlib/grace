#include "llvm/IR/Verifier.h"
#include <AST.hh>
#include <Error.hh>
#include <iostream>

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
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          LLVMContext &TheContext,
                                          const std::string &Name, Type *T) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(T, nullptr, Name.c_str());
}

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

  C.enterScope();

  // Create vector with llvm types for args.
  std::vector<Type *> ArgsType;
  ArgsType.reserve(Args->size());
  for (auto Arg : *Args)
    ArgsType.push_back(C.getLLVMType(Arg->Type));

  // Create function signature.
  FunctionType *FT =
      FunctionType::get(C.getLLVMType(ReturnType), ArgsType, false);
  F = Function::Create(FT, GlobalValue::LinkageTypes::ExternalLinkage, Name,
                       &C.getModule());

  // set args
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName((*Args)[Idx++]->Id);

  BasicBlock *BB = BasicBlock::Create(C.getContext(), "entry", F);
  C.getBuilder().SetInsertPoint(BB);

  // insert args into scope
  for (auto &Arg : F->args()) {
    AllocaInst *Alloca =
        CreateEntryBlockAlloca(F, C.getContext(), Arg.getName(), Arg.getType());

    C.getBuilder().CreateStore(&Arg, Alloca);

    C.insertNamedValueIntoScope(Arg.getName(), Alloca);
  }

  // generate function body
  C.ExpectReturn = true;
  C.ReturnFound = false;
  Body->codegen(C);

  C.leaveScope();

  if (!C.ReturnFound) {
    Log::warning(0, 0) << "expected a return statement inside function body, "
                          "but none was found.\n";
    return nullptr;
  }

  return nullptr;
}

Value *VarDeclNode::codegen(Context &C) {
  if (C.getNamedValueInScope(Id)) {
    // ::Error::typeMismatch("asfas", "asdfasdf");
    Log::error(0, 0) << "variable " << Id << " already declared.\n";
    return nullptr;
  }

  Function *TheFunction = C.getBuilder().GetInsertBlock()->getParent();

  AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, C.getContext(), Id,
                                              C.getLLVMType(Type));

  if (Assign) {
    Assign->codegen(C);
  }

  C.insertNamedValueIntoScope(Id, Alloca);

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

  if (!C.IsInsideLoop)
    Log::error(0, 0) << "skip command can appear only inside loops.\n";

  // TODO: implement

  return nullptr;
}

Value *StopNode::codegen(Context &C) {
  if (!C.IsInsideLoop)
    Log::error(0, 0) << "stop command can appear only inside loops.\n";

  // TODO: implement

  return nullptr;
}

Value *ForNode::codegen(Context &C) {
  std::cout << "ForNode unimplemented" << std::endl;

  C.IsInsideLoop = true;

  forBlock->codegen(C);

  C.IsInsideLoop = false;

  return nullptr;
}

Value *WhileNode::codegen(Context &C) {
  std::cout << "WhileNode unimplemented" << std::endl;

  // TODO: implement

  C.IsInsideLoop = true;

  Block->codegen(C);

  C.IsInsideLoop = false;

  return nullptr;
}

Value *ExprIdentifierNode::codegen(Context &C) {
  AllocaInst *Alloca = C.getNamedValueInScope(Id);
  if (!Alloca) {
    Log::error(0, 0) << "variable " << Id << " not found.\n";
    return nullptr;
  }

  return C.getBuilder().CreateLoad(Alloca, Id);
}

Value *LiteralStringNode::codegen(Context &C) {
  return C.getBuilder().CreateGlobalStringPtr(Str);
}

Value *ArrayAssignNode::codegen(Context &C) {
  std::cout << "ArrayAssignNode unimplemented" << std::endl;
  return nullptr;
}

Value *AssignSimpleNode::codegen(Context &C) {
  AllocaInst *Alloca = C.getNamedValueInScope(Id);
  if (!Alloca)
    return nullptr;

  Value *Store = expr->codegen(C);

  Type *AllocaTy = Alloca->getAllocatedType();
  Type *StoreTy = Store->getType();

  if (!C.typeCheck(AllocaTy, StoreTy)) {
    Log::error(0, 0) << "cannot assign value of type '" << C.getType(StoreTy)
                     << "', expected '" << C.getType(AllocaTy) << "'\n";
    return nullptr;
  }

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
  Value *LHSV = LHS->codegen(C);
  Value *RHSV = RHS->codegen(C);

  switch (Op) {
  case BinOp::PLUS:
    return C.getBuilder().CreateAdd(LHSV, RHSV, "addtmp");
  case BinOp::MINUS:
    return C.getBuilder().CreateSub(LHSV, RHSV, "subtmp");
  case BinOp::TIMES:
    return C.getBuilder().CreateMul(LHSV, RHSV, "multmp");
  case BinOp::DIV:
    return C.getBuilder().CreateFDiv(LHSV, RHSV, "divtmp");
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

Value *CallExprNode::codegen(Context &C) {
  Function *CalleeF = C.getModule().getFunction(Callee);
  if (!CalleeF) {
    Log::error(0, 0) << "function " << Callee << " not found.\n";
    return nullptr;
  }

  if (CalleeF->arg_size() != Args->size()) {
    Log::error(0, 0) << "incorrect number of arguments passed to function "
                     << Callee << "\n";
    return nullptr;
  }

  std::vector<Value *> ArgsV;
  for (auto Arg : *Args) {
    ArgsV.push_back(Arg->codegen(C));
    if (!ArgsV.back())
      return nullptr;
  }

  return C.getBuilder().CreateCall(CalleeF, ArgsV, "calltmp");
}

Value *ProcDeclNode::codegen(Context &C) {
  std::cout << "ProcDeclNode unimplemented" << std::endl;
  return nullptr;
}

Value *WriteNode::codegen(Context &C) {
  std::vector<Value *> Values;
  Values.reserve(Exprs->size());

  Function *Printf = C.getModule().getFunction("printf");
  if (!Printf) {
    FunctionType *FuncTy =
        FunctionType::get(IntegerType::get(C.getContext(), 32), true);

    Printf = Function::Create(FuncTy, GlobalValue::ExternalLinkage, "printf",
                              &C.getModule());
    Printf->setCallingConv(CallingConv::C);

    AttributeList PrintfPAL;
    Printf->setAttributes(PrintfPAL);
  }

  Value *StrFormat = C.getBuilder().CreateGlobalStringPtr(Format);
  Values.push_back(StrFormat);

  for (auto Expr : *Exprs)
    Values.push_back(Expr->codegen(C));

  C.getBuilder().CreateCall(Printf, Values, "callprintf");

  return nullptr;
}
