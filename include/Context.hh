#pragma once

#include "Log.hh"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include <SymbolTable.hh>
#include <list>
#include <llvm/IR/PassManager.h>

namespace grace {
static const int INT_SIZE = 32;
static const int BOOL_SIZE = 1;

class Context {
  llvm::LLVMContext TheContext;
  llvm::IRBuilder<> TheBuilder;
  llvm::Module TheModule;

public:
  SymbolTable ST;

  Context() : TheBuilder(TheContext), TheModule("grace lang", TheContext) {
    ExpectReturn = false;
    ReturnFound = false;
    IsInsideLoop = false;

    // initialize global scope
    ST.enterScope();

    initializePassManager();
  }

  llvm::Module &getModule() { return TheModule; }

  llvm::LLVMContext &getContext() { return TheModule.getContext(); }

  llvm::IRBuilder<> &getBuilder() { return TheBuilder; }

  bool typeCheck(llvm::Type *A, llvm::Type *B);

  void dumpIR() const { TheModule.print(errs(), nullptr); }

  bool ExpectReturn;
  bool ReturnFound;
  bool IsInsideLoop;

private:
  void initializePassManager() {}
};

}; // namespace grace