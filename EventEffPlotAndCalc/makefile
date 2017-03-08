# Make file for ZCAnalysis/EventEffPlotAndCalc package

CC := g++-4.9
#CC := g++
SRCDIR := src
LIBDIR := ../ZCLibrary
HDRDIR := include
BUILDDIR := build
TARGETDIR := bin
TARGET := bin/EventEffPlotAndCalc

SRCEXT := cpp
HDREXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
BOOSTLIB := -lboost_program_options
ROOTFLAGS=`root-config --cflags --glibs`
CFLAGS := -g -Wall -std=c++11 -fdiagnostics-color $(BOOSTLIB) $(ROOTFLAGS)
INC := -I $(HDRDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(HDRDIR)/%.$(HDREXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(TARGETDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(BOOSTLIB) $(ROOTFLAGS)"; $(CC) $^ -o $(TARGET) $(BOOSTLIB) $(ROOTFLAGS)

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGETDIR)"; $(RM) -r $(BUILDDIR) $(TARGETDIR)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

.PHONY: clean
