#pragma once

#include "Log.hh"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include <list>
#include <llvm/IR/PassManager.h>

using namespace llvm;

static const int INT_SIZE = 32;
static const int BOOL_SIZE = 1;

class Context {
  LLVMContext TheContext;
  IRBuilder<> TheBuilder;
  Module TheModule;

  //    std::unique_ptr<FunctionPassManager> FPM;

  std::list<std::map<std::string, AllocaInst *>> NamedValues;

public:
  Context() : TheBuilder(TheContext), TheModule("grace lang", TheContext) {
    ExpectReturn = false;
    ReturnFound = false;
    IsInsideLoop = false;

    enterScope();

    initializePassManager();
  }

  Module &getModule() { return TheModule; }

  LLVMContext &getContext() { return TheModule.getContext(); }

  IRBuilder<> &getBuilder() { return TheBuilder; }

  Type *getLLVMType(std::string &TypeRepresentation);

  std::string getType(Type *T);

  bool typeCheck(Type *A, Type *B);

  void dumpIR() const { TheModule.print(errs(), nullptr); }

  void enterScope();

  void leaveScope();

  void insertNamedValueIntoScope(const std::string &Name, AllocaInst *V);

  AllocaInst *getNamedValueInScope(std::string &Name);

  bool ExpectReturn;
  bool ReturnFound;
  bool IsInsideLoop;

private:
  void initializePassManager() {}
};
