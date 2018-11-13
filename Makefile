# This Makefile is designed to be simple and readable.  It does not
# aim at portability.  It requires GNU Make.

BISON = bison
CXX = g++
FLEX = flex

SRCDIR = src

LLVM_MODULES =

LLVM_CXXFLAGS := `llvm-config --cxxflags $(LLVM_MODULES)`
LLVM_LDFLAGS := `llvm-config --ldflags --libs --system-libs`

all: grace

$(SRCDIR)/%.cc $(SRCDIR)/%.hh: $(SRCDIR)/%.yy
	$(BISON) $(BISONFLAGS) -o $(SRCDIR)/$*.cc $<

$(SRCDIR)/%.cc: $(SRCDIR)/%.ll
	$(FLEX) $(FLEXFLAGS) -o$@ $<

$(SRCDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -std=c++11 -c -g -o$@ $<

grace: $(SRCDIR)/main.o $(SRCDIR)/driver.o $(SRCDIR)/parser.o $(SRCDIR)/scanner.o
	$(CXX) -o $@ $^ $(LLVM_LDFLAGS)

$(SRCDIR)/main.o: $(SRCDIR)/parser.hh
$(SRCDIR)/parser.o: $(SRCDIR)/parser.hh
$(SRCDIR)/scanner.o: $(SRCDIR)/parser.hh

clean:
	rm -f grace $(SRCDIR)/*.o $(SRCDIR)/parser.hh $(SRCDIR)/parser.cc $(SRCDIR)/scanner.cc

