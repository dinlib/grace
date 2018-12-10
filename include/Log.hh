#pragma once

#include "llvm/Support/raw_ostream.h"
#include <ostream>
#include <string>
#include <utility>

using namespace llvm;

extern bool hasError;

class Log {
public:
  // static raw_ostream &error(const TokLocation &Loc) {
  //     return error(Loc.Line, Loc.Column);
  // }

  static raw_ostream &error(unsigned Line, unsigned Col) {
    hasError = true;
    errs() << Line << "," << Col << ": \033[1;31merror\033[0m: ";
    return errs();
  }

  // static raw_ostream &warning(const TokLocation &Loc) {
  //     return warning(Loc.Line, Loc.Column);
  // }

  static raw_ostream &warning(unsigned Line, unsigned Col) {
    errs() << Line << "," << Col << ": \033[1;33mwarning\033[0m: ";
    return errs();
  }
};
