
//
// Created by Guilherme Souza on 12/7/18.
//

#ifndef GRACE_SYMBOLTABLE_HH
#define GRACE_SYMBOLTABLE_HH

#include <list>
#include <unordered_map>
#include <utility>

#include "Type.hh"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"

namespace grace {

class Symbol {
public:
  virtual ~Symbol() = default;
};

class VariableSymbol : public Symbol {
public:
  llvm::AllocaInst *Alloca;
  Type *Ty;

  VariableSymbol(llvm::AllocaInst *Alloca, Type *Ty) : Alloca(Alloca), Ty(Ty) {}
};

class BlockSymbol : public Symbol {
public:
  llvm::BasicBlock *BB;

  BlockSymbol(llvm::BasicBlock *BB) : BB(BB) {}
};

class FuncSymbol : public Symbol {
public:
  llvm::Value *Value;
  Type *ReturnTy;
  std::vector<Type *> Args;

  FuncSymbol(llvm::Value *Value, Type *ReturnTy, std::vector<Type *> Args)
      : Value(Value), ReturnTy(ReturnTy), Args(std::move(Args)) {}
};

class SymbolTable {
  std::list<std::unordered_map<std::string, Symbol *>> Scopes;

public:
  Symbol *get(const std::string &Identifier) const {
    for (const auto &Scope : Scopes) {
      auto it = Scope.find(Identifier);
      if (it != Scope.end())
        return it->second;
    }

    return nullptr;
  }

  void set(const std::string &Identifier, Symbol *Sym) {
    if (get(Identifier) == nullptr)
      Scopes.back()[Identifier] = Sym;
  }

  void enterScope() {
    Scopes.push_back(std::unordered_map<std::string, Symbol *>());
  }

  void leaveScope() { Scopes.pop_back(); }
};
}; // namespace grace

#endif // GRACE_SYMBOLTABLE_HH
