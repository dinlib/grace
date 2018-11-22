#pragma once

#include <list>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

class Context {
    LLVMContext TheContext;
    IRBuilder<> TheBuilder;
    Module TheModule;

    std::list<std::map<std::string, Value *>> NamedValues;

public:
    Context() : TheBuilder(TheContext), TheModule("grace lang", TheContext) {}

    Module &getModule() { return TheModule; }

    LLVMContext &getContext() { return TheModule.getContext(); }

    IRBuilder<> &getBuilder() { return TheBuilder; }

    Type *getLLVMType(std::string &TypeRepresentation);

    void dumpIR() const { TheModule.print(errs(), nullptr); }

    void enterScope();

    void leaveScope();

    void insertNamedValueIntoScope(const std::string &Name, Value *V);

    Value *getNamedValueInScope(std::string &Name);
};
