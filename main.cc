#include "Context.hh"
#include "Driver.hh"

int main(int argc, char **argv) {
  int res = 0;
  Driver drv;
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == std::string("-p")) {
      drv.trace_parsing = true;
    } else if (argv[i] == std::string("-s")) {
      drv.trace_scanning = true;
    } else if (argv[i] == std::string("--dump-ast")) {
      drv.dump_ast = true;
    } else if (argv[i] == std::string("--dump-ir")) {
      drv.dump_ir = true;
    } else if (!drv.parse(argv[i])) {
      if (drv.dump_ast)
          drv.program->dumpAST(std::cout, 0);
    } else {
      res = 1;
    }
  }

  Context C;
  drv.program->codegen(C);

  if (drv.dump_ir) {
    C.dumpIR();
  }

  return res;
}