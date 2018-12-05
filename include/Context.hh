#pragma once

#include <list>
#include <llvm/IR/PassManager.h>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "Log.hh"

using namespace llvm;

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

    void dumpIR() const { TheModule.print(errs(), nullptr); }

    void enterScope();

    void leaveScope();

    void insertNamedValueIntoScope(const std::string &Name, AllocaInst *V);

    AllocaInst *getNamedValueInScope(std::string &Name);

    bool ExpectReturn;
    bool ReturnFound;
    bool IsInsideLoop;

private:
    void initializePassManager() {

    }
};
