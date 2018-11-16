#include "AST.hh"

Value *BlockNode::codegen(Context &C) {
  for (auto &stmt : stmts)
    stmt->codegen(C);

  return NULL;
}