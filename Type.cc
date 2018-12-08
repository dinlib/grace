//
// Created by Guilherme Souza on 12/7/18.
//


#include <Type.hh>
#include "llvm/IR/Type.h"
#include "Context.hh"

using namespace grace;

llvm::Type *IntType::emit(Context &C) {
    return llvm::Type::getInt32Ty(C.getContext());
}

llvm::Type *BoolType::emit(Context &C) {
    return llvm::Type::getInt1Ty(C.getContext());
}

llvm::Type *StringType::emit(Context &C) {
    llvm::errs() << "unimplemented\n";
    return nullptr;
}

llvm::Type *grace::ArrayType::emit(Context &C) {
    return llvm::ArrayType::get(InnerType->emit(C), Size);
}

bool grace::Type::isArray() {
    return dynamic_cast<ArrayType *>(this) != nullptr;
}