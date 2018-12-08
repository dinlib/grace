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
//  case Type::TypeID::PointerTyID:
//    return typeCheck(A->getPointerElementType(), B->getPointerElementType());
//  case Type::VoidTyID:
//    break;
//  case Type::HalfTyID:
//    break;
//  case Type::FloatTyID:
//    break;
//  case Type::DoubleTyID:
//    break;
//  case Type::X86_FP80TyID:
//    break;
//  case Type::FP128TyID:
//    break;
//  case Type::PPC_FP128TyID:
//    break;
//  case Type::LabelTyID:
//    break;
//  case Type::MetadataTyID:
//    break;
//  case Type::X86_MMXTyID:
//    break;
//  case Type::TokenTyID:
//    break;
//  case Type::FunctionTyID:
//    break;
//  case Type::StructTyID:
//    break;
//  case Type::ArrayTyID:
//    break;
//  case Type::VectorTyID:
//    break;
  }

  return false;
}