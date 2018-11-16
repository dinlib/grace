#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

class Context {
  llvm::LLVMContext TheContext;
  llvm::IRBuilder<> TheBuilder;
  llvm::Module TheModule;

public:
  Context() : TheBuilder(TheContext), TheModule("grace lang", TheContext) {}
  llvm::Module &getModule() { return TheModule; }
  llvm::LLVMContext &getContext() { return TheModule.getContext(); }

  void dumpIR() const { TheModule.print(llvm::errs(), nullptr); }
};