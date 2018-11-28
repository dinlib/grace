#pragma once

#include <utility>
#include <string>
#include <ostream>
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class Log {
public:
    static raw_ostream &error(unsigned Line, unsigned Col) {
        errs() << Line << "," << Col << ": \033[1;31merror\033[0m: ";
        return errs();
    }

    static raw_ostream &warning(unsigned Line, unsigned Col) {
        errs() << Line << "," << Col << ": \033[1;33mwarning\033[0m: ";
        return errs();
    }
};
