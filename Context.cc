//
// Created by Guilherme Souza on 11/22/18.
//

#include <Context.hh>

using namespace grace;

bool Context::typeCheck(llvm::Type *A, llvm::Type *B) {
  if (A->getTypeID() != B->getTypeID())
    return false;

  switch (A->getTypeID()) {
  case llvm::Type::TypeID::IntegerTyID:
    return A->getIntegerBitWidth() == B->getIntegerBitWidth();
  }

  return false;
}