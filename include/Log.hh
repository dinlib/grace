#pragma once

#include "llvm/Support/raw_ostream.h"
#include "location.hh"
#include <ostream>
#include <string>
#include <utility>

using namespace llvm;

extern bool hasError;

class Log {
public:
   static raw_ostream &error(const yy::position &pos) {
       return error(pos.line, pos.column);
   }

  static raw_ostream &error(unsigned Line, unsigned Col) {
    hasError = true;
    errs() << Line << "," << Col << ": \033[1;31merror\033[0m: ";
    return errs();
  }

   static raw_ostream &warning(const yy::position &pos) {
       return warning(pos.line, pos.column);
   }

  static raw_ostream &warning(unsigned Line, unsigned Col) {
    errs() << Line << "," << Col << ": \033[1;33mwarning\033[0m: ";
    return errs();
  }
};
