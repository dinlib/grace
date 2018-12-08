//
// Created by Guilherme Souza on 12/7/18.
//

#pragma once

#include "AST.hh"
#include <string>

namespace llvm {
class Type;
};

namespace grace {
class Context;
class ArrayType;

class Type {
public:
  virtual ~Type() = default;

  virtual llvm::Type *emit(Context &C) = 0;

  virtual std::string str() = 0;
};

class IntType : public Type {
public:
  llvm::Type *emit(Context &C) override;

  std::string str() override { return "int"; }
};

class BoolType : public Type {
public:
  llvm::Type *emit(Context &C) override;

  std::string str() override { return "bool"; }
};

class StringType : public Type {
public:
  llvm::Type *emit(Context &C) override;

  std::string str() override { return "string"; }
};

}; // namespace grace
