#include "driver.hh"
#include <iostream>

int main(int argc, char **argv) {
  int res = 0;
  Driver drv;
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == std::string("-p")) {
      drv.trace_parsing = true;
    } else if (argv[i] == std::string("-s")) {
      drv.trace_scanning = true;
    } else if (!drv.parse(argv[i])) {
      drv.program->PrintAST();
    } else {
      res = 1;
    }
  }

  return res;
}