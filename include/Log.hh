#pragma once

#include <utility>
#include <string>
#include <ostream>
#include "llvm/Support/raw_ostream.h"
// #include "TokLocation.hh"

using namespace llvm;

class Log {
public:

    // static raw_ostream &error(const TokLocation &Loc) {
    //     return error(Loc.Line, Loc.Column);
    // }

    static raw_ostream &error(unsigned Line, unsigned Col) {
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
