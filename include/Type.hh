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

  bool isArray();
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

class ArrayType : public Type {
public:
  Type *InnerType;
  uint64_t Size;

  explicit ArrayType(Type *InnerType, uint64_t Size)
      : InnerType(InnerType), Size(Size) {}

  llvm::Type *emit(Context &C) override;

  std::string str() override {
    return InnerType->str() + "[" + std::to_string(Size) + "]";
  }
};

}; // namespace grace
