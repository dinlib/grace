#include "AST.hh"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"

using namespace llvm;

Value *LiteralIntNode::codegen(Context &C) {
    return ConstantInt::get(C.getContext(), APInt(sizeof(value), value));
}