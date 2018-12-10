#include "Context.hh"
#include "iostream"
#include "llvm/IR/Verifier.h"

using namespace llvm;
using namespace grace;

/// LogError* - These are little helper functions for error handling.
Node *LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          LLVMContext &TheContext,
                                          const std::string &Name,
                                          llvm::Type *T) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(T, nullptr, Name);
}

Value *BlockNode::codegen(Context &C) {
  for (auto &stmt : stmts)
    stmt->codegen(C);

  return nullptr;
}

Value *LiteralIntNode::codegen(Context &C) {
  return ConstantInt::get(C.getContext(), APInt(INT_SIZE, IVal));
}

Value *LiteralBoolNode::codegen(Context &C) {
  LLVMContext &TheContext = C.getContext();
  return ConstantInt::get(TheContext, APInt(BOOL_SIZE, BVal ? 1 : 0));
}

Value *IfThenElseNode::codegen(Context &C) {

  auto &Builder = C.getBuilder();
  auto &TheContext = C.getContext();

  auto TheFunction = Builder.GetInsertBlock()->getParent();

  auto ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
  auto MergeBB = BasicBlock::Create(TheContext, "merge", TheFunction);
  auto LastBB = Else ? BasicBlock::Create(TheContext, "else", TheFunction) : MergeBB;


  auto CondV = Condition->codegen(C);
  Builder.CreateCondBr(CondV, ThenBB, LastBB);

  Builder.SetInsertPoint(ThenBB);
  // TODO: enter scope
  Then->codegen(C);
  // TODO: leave scope

    Builder.CreateBr(MergeBB);

  if (Else) {
      Builder.SetInsertPoint(LastBB);
      // TODO: enter scope
      Else->codegen(C);
      // TODO: leave scope

      Builder.CreateBr(MergeBB);
  }

  Builder.SetInsertPoint(MergeBB);

  return nullptr;
}

Value *FuncDeclNode::codegen(Context &C) {
  Function *F = C.getModule().getFunction(Name);

  // Check if function is already defined.
  if (F) {
    Log::error(0, 0) << "function " << Name << " already defined\n";
    return nullptr;
  }

  C.ST.enterScope();

  // Create vector with llvm types for args.
  std::vector<llvm::Type *> ArgsType;
  ArgsType.reserve(Args->size());
  for (auto Arg : *Args)
    ArgsType.push_back(Arg->Ty->emit(C));

  // Create function signature.
  FunctionType *FT = FunctionType::get(ReturnTy->emit(C), ArgsType, false);
  F = Function::Create(FT, GlobalValue::LinkageTypes::ExternalLinkage, Name,
                       &C.getModule());

  // set args
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName((*Args)[Idx++]->Id);

  BasicBlock *BB = BasicBlock::Create(C.getContext(), "entry", F);
  C.getBuilder().SetInsertPoint(BB);

  // insert args into scope
  Idx = 0;
  for (auto &Arg : F->args()) {
    AllocaInst *Alloca =
        CreateEntryBlockAlloca(F, C.getContext(), Arg.getName(), Arg.getType());

    C.getBuilder().CreateStore(&Arg, Alloca);

    C.ST.set(Arg.getName(), new VariableSymbol(Alloca, (*Args)[Idx++]->Ty));
  }

  // generate function body
  C.ExpectReturn = true;
  C.ReturnFound = false;
  Body->codegen(C);

  C.ST.leaveScope();

  if (!C.ReturnFound) {
    Log::warning(0, 0) << "expected a return statement inside function body, "
                          "but none was found.\n";
    return nullptr;
  }

  return nullptr;
}

Value *VarDeclNode::codegen(Context &C) {
  if (C.ST.get(Id)) {
    Log::error(0, 0) << "variable " << Id << " already declared.\n";
    return nullptr;
  }

  Function *TheFunction = C.getBuilder().GetInsertBlock()->getParent();

  AllocaInst *Alloca =
      CreateEntryBlockAlloca(TheFunction, C.getContext(), Id, Ty->emit(C));

  C.ST.set(Id, new VariableSymbol(Alloca, Ty));

  if (Assign) {
    Assign->codegen(C);
  }

  return nullptr;
}

// Value *ArrayDeclNode::codegen(Context &C) {
//    if (C.ST.get(Id)) {
//        Log::error(0, 0) << "variable " << Id << " already declared.\n";
//        return nullptr;
//    }
//
//    Function *TheFunction = C.getBuilder().GetInsertBlock()->getParent();
//
//    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, C.getContext(),
//    Id, Ty->emit(C));
//
//    C.ST.set(Id, new ArraySymbol(Alloca, dynamic_cast<ArrayType *>(Ty)));
//
//    if (Assign) {
//        Assign->codegen(C);
//    }
//
//    return nullptr;
//}

Value *ReturnNode::codegen(Context &C) {
  C.ReturnFound = true;

  IRBuilder<> &Builder = C.getBuilder();

  if (expr)
    return Builder.CreateRet(expr->codegen(C));

  return Builder.CreateRetVoid();
}

Value *SkipNode::codegen(Context &C) {
  auto Sym = dynamic_cast<BlockSymbol *>(C.ST.get("skip"));

  if (!Sym)
    Log::error(0, 0) << "skip command can appear only inside loops.\n";
  else
    C.getBuilder().CreateBr(Sym->BB);

  return nullptr;
}

Value *StopNode::codegen(Context &C) {
  auto Sym = dynamic_cast<BlockSymbol *>(C.ST.get("stop"));

  if (!Sym)
    Log::error(0, 0) << "stop command can appear only inside loops.\n";
  else
    C.getBuilder().CreateBr(Sym->BB);

  return nullptr;
}

Value *ForNode::codegen(Context &C) {
  auto &Builder = C.getBuilder();
  auto &TheContext = C.getContext();
  auto TheFunction = Builder.GetInsertBlock()->getParent();

  auto BeforeLoopBB =
      BasicBlock::Create(TheContext, "before_loop", TheFunction);
  auto LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
  auto StepBB = BasicBlock::Create(TheContext, "step", TheFunction);
  auto AfterLoopBB = BasicBlock::Create(TheContext, "after_loop", TheFunction);

  C.ST.set("skip", new BlockSymbol(StepBB));
  C.ST.set("stop", new BlockSymbol(AfterLoopBB));

  Start->codegen(C);

  Builder.CreateBr(BeforeLoopBB);

  Builder.SetInsertPoint(BeforeLoopBB);

  auto CondV = End->codegen(C);
  if (!CondV)
      return nullptr;

  auto CondTy = Type::from(CondV->getType());
  if (!CondTy) {
      Log::error(0, 0) << "'" << CondTy->str() << "' is not convertible to '" << Type::boolTy()->str() << "'";
      return nullptr;
  }

  Builder.CreateCondBr(CondV, LoopBB, AfterLoopBB);

  Builder.SetInsertPoint(LoopBB);

  C.ST.enterScope();
  Body->codegen(C);
  C.ST.leaveScope();

  Builder.SetInsertPoint(StepBB);
  Step->codegen(C);

  Builder.CreateBr(BeforeLoopBB);

  Builder.SetInsertPoint(AfterLoopBB);

  return nullptr;
}

Value *WhileNode::codegen(Context &C) {
  auto &Builder = C.getBuilder();
  auto &TheContext = C.getContext();

  auto TheFunction = Builder.GetInsertBlock()->getParent();

  auto BeforeLoopBB =
      BasicBlock::Create(TheContext, "before_loop", TheFunction);
  auto LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
  auto AfterLoopBB = BasicBlock::Create(TheContext, "after_loop", TheFunction);

  C.ST.set("skip", new BlockSymbol(LoopBB));
  C.ST.set("stop", new BlockSymbol(AfterLoopBB));

  Builder.CreateBr(BeforeLoopBB);
  Builder.SetInsertPoint(BeforeLoopBB);

  auto CondV = Condition->codegen(C);
  assert(CondV);

  Builder.CreateCondBr(CondV, LoopBB, AfterLoopBB);

  Builder.SetInsertPoint(LoopBB);

  C.ST.enterScope();
  Block->codegen(C);
  C.ST.leaveScope();

  Builder.CreateBr(BeforeLoopBB);

  Builder.SetInsertPoint(AfterLoopBB);

  return nullptr;
}

Value *VariableExprNode::codegen(Context &C) {
  auto Sym = dynamic_cast<VariableSymbol *>(C.ST.get(Id));
  if (!Sym) {
    Log::error(0, 0) << "variable " << Id << " not found.\n";
    return nullptr;
  }

  return C.getBuilder().CreateLoad(Sym->Alloca, Id);
}

Value *LiteralStringNode::codegen(Context &C) {
  return C.getBuilder().CreateGlobalStringPtr(Str);
}

llvm::Value *AssignNode::codegen(Context &C) {
  auto Sym = dynamic_cast<VariableSymbol *>(C.ST.get(Id));
  if (!Sym)
    return nullptr;

  Value *Store = Assign->codegen(C);
  if (!Store)
    return nullptr;

  auto AssignTy = Type::from(Store->getType());
  auto DeclaredTy = Sym->Ty;

  if (*AssignTy != *DeclaredTy) {
    Log::error(0, 0) << "cannot assign value of type '" << AssignTy->str()
                     << "', expected '" << DeclaredTy->str() << "'\n";
    return nullptr;
  }

  C.getBuilder().CreateStore(Store, Sym->Alloca);

  return Store;
}

Value *VarDeclNodeListStmt::codegen(Context &C) {
  for (auto VarDecl : varDeclList)
    VarDecl->codegen(C);

  return nullptr;
}

Value *ExprNegativeNode::codegen(Context &C) {
  Value *RHSV = RHS->codegen(C);
  return C.getBuilder().CreateNeg(RHSV, "negtmp");
}

Value *ExprNotNode::codegen(Context &C) {
  Value *RHSV = RHS->codegen(C);
  return C.getBuilder().CreateNot(RHSV, "nottmp");
}

Value *ExprOperationNode::codegen(Context &C) {
  Value *LHSV = LHS->codegen(C);
  Value *RHSV = RHS->codegen(C);

  switch (Op) {
  case BinOp::PLUS:
    return C.getBuilder().CreateAdd(LHSV, RHSV);
  case BinOp::MINUS:
    return C.getBuilder().CreateSub(LHSV, RHSV);
  case BinOp::TIMES:
    return C.getBuilder().CreateMul(LHSV, RHSV);
  case BinOp::DIV:
    return C.getBuilder().CreateUDiv(LHSV, RHSV);
  case BinOp::MOD:
    return C.getBuilder().CreateURem(LHSV, RHSV, "modtmp");
  case BinOp::LT:
    return C.getBuilder().CreateICmpSLT(LHSV, RHSV);
  case BinOp::LTEQ:
    return C.getBuilder().CreateICmpSLE(LHSV, RHSV);
  case BinOp::GT:
    return C.getBuilder().CreateICmpSGT(LHSV, RHSV);
  case BinOp::GTEQ:
    return C.getBuilder().CreateICmpSGE(LHSV, RHSV);
  case BinOp::EQ:
    return C.getBuilder().CreateICmpEQ(LHSV, RHSV);
  case BinOp::DIFF:
    return C.getBuilder().CreateICmpNE(LHSV, RHSV);
  case BinOp::AND:
    return  C.getBuilder().CreateAnd(LHSV, RHSV, "andtmp");
  case BinOp::OR:
    return C.getBuilder().CreateOr(LHSV, RHSV, "ortmp");
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
  //  std::vector<Value *> Values;
  //  Values.reserve(Exprs->size());
  //
  //  Function *Printf = C.getModule().getFunction("printf");
  //  if (!Printf) {
  //    FunctionType *FuncTy =
  //        FunctionType::get(IntegerType::get(C.getContext(), 32), true);
  //
  //    Printf = Function::Create(FuncTy, GlobalValue::ExternalLinkage,
  //    "printf",
  //                              &C.getModule());
  //    Printf->setCallingConv(CallingConv::C);
  //
  //    AttributeList PrintfPAL;
  //    Printf->setAttributes(PrintfPAL);
  //  }
  //
  //  Value *StrFormat = C.getBuilder().CreateGlobalStringPtr(Format);
  //  Values.push_back(StrFormat);
  //
  //  for (auto Expr : *Exprs)
  //    Values.push_back(Expr->codegen(C));
  //
  //  C.getBuilder().CreateCall(Printf, Values, "callprintf");

  return nullptr;
}

Value *CompoundAssignNode::codegen(Context &C) {
  auto Sym = dynamic_cast<VariableSymbol *>(C.ST.get(Id));

  if (!Sym) {
    Log::error(0, 0) << "variable " << Id << " not declared in this scope.\n";
    return nullptr;
  }

  auto Alloca = Sym->Alloca;
  Value *Store = Assign->codegen(C);
  //
  //  Type *AllocaTy = Alloca->getAllocatedType();
  //  Type *StoreTy = Store->getType();
  //
  //  if (!C.typeCheck(AllocaTy, StoreTy)) {
  //    Log::error(0, 0) << "cannot assign value of type '" <<
  //    C.getType(StoreTy)
  //                     << "', expected '" << C.getType(AllocaTy) << "'\n";
  //    return nullptr;
  //  }

  Value *AllocaValue = C.getBuilder().CreateLoad(Alloca, Id);
  Value *Result = nullptr;

  switch (Op) {
  case BinOp::PLUS:
    Result = C.getBuilder().CreateAdd(Store, AllocaValue);
    C.getBuilder().CreateStore(Result, Alloca);
    break;
  case BinOp::MINUS:
    Result = C.getBuilder().CreateSub(Store, AllocaValue);
    C.getBuilder().CreateStore(Result, Alloca);
    break;
  case BinOp::TIMES:
    Result = C.getBuilder().CreateMul(Store, AllocaValue);
    C.getBuilder().CreateStore(Result, Alloca);
    break;
  case BinOp::DIV:
    Result = C.getBuilder().CreateUDiv(Store, AllocaValue);
    C.getBuilder().CreateStore(Result, Alloca);
    break;
  }

  return nullptr;
}
