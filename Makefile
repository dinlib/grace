# This Makefile is designed to be simple and readable.  It does not
# aim at portability.  It requires GNU Make.

BISON = bison
CXX = g++
FLEX = flex

SRCDIR = src

all: czin

$(SRCDIR)/%.cc $(SRCDIR)/%.hh: $(SRCDIR)/%.yy
	$(BISON) $(BISONFLAGS) -o $(SRCDIR)/$*.cc $<

$(SRCDIR)/%.cc: $(SRCDIR)/%.ll
	$(FLEX) $(FLEXFLAGS) -o$@ $<

$(SRCDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -c -o$@ $<

czin: $(SRCDIR)/main.o $(SRCDIR)/driver.o $(SRCDIR)/parser.o $(SRCDIR)/scanner.o
	$(CXX) -o $@ $^

$(SRCDIR)/main.o: $(SRCDIR)/parser.hh
$(SRCDIR)/parser.o: $(SRCDIR)/parser.hh
$(SRCDIR)/scanner.o: $(SRCDIR)/parser.hh

clean:
	rm -f czin $(SRCDIR)/*.o $(SRCDIR)/parser.hh $(SRCDIR)/parser.cc $(SRCDIR)/scanner.cc

