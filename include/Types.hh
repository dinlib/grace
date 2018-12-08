#pragma once

#include "llvm/IR/IRBuilder.h"

namespace grace {

class Type {
public:
  enum class TypeID {
    Int,
    Bool,
    String,
    ArrayInt,
    ArrayBool,
  };

  static const std::string &getStringRepresentation(TypeID ID) {
    switch (ID) {
    case TypeID::Int:
      return "int";
    case TypeID::Bool:
      return "bool";
    }
  }

  static llvm::Type *getType(TypeID ID, llvm::LLVMContext &C) {
    switch (ID) {
    case TypeID::Int:
      return llvm::Type::getInt32Ty(C);

    case TypeID::Bool:
      return llvm::Type::getInt1Ty(C);
    }
  }
};
} // namespace grace