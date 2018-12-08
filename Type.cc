//
// Created by Guilherme Souza on 12/7/18.
//

#include "llvm/IR/Type.h"
#include "Context.hh"
#include <Type.hh>

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
