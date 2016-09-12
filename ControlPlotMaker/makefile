# Make file for ZCAnalysis/ControlPlotMaker package

CC=g++
CFLAGS=-c -Wall
ROOTFLAGS=`root-config --cflags --glibs`
LIBDIR=ZCLibrary
PRGDIR=ControlPlotMaker
OBJDIR=$(PRGDIR)/obj
HDRDIR=$(PRGDIR)/interface
SRCDIR=$(PRGDIR)/src
OBJS=$(OBJDIR)/main.o $(OBJDIR)/ControlPlotMaker.o $(OBJDIR)/AnalysisConfig.o $(OBJDIR)/LeptonSFData.o 

ControlPlotMaker : createObjDir ControlPlotMaker.exe

createObjDir : 
	mkdir -p ControlPlotMaker/obj
	mkdir -p ControlPlotMaker/plots
	mkdir -p ControlPlotMaker/epsplots

ControlPlotMaker.exe : $(OBJS)
	$(CC) $(OBJS) -o ControlPlotMaker.exe $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/main.o : $(PRGDIR)/main.cpp $(HDRDIR)/ControlPlotMaker.h
	$(CC) $(CFLAGS) $(PRGDIR)/main.cpp -o $(OBJDIR)/main.o $(ROOTFLAGS)

$(OBJDIR)/ControlPlotMaker.o : $(SRCDIR)/ControlPlotMaker.cpp $(HDRDIR)/ControlPlotMaker.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h
	$(CC) $(CFLAGS) $(SRCDIR)/ControlPlotMaker.cpp -o $(OBJDIR)/ControlPlotMaker.o $(ROOTFLAGS)

$(OBJDIR)/AnalysisConfig.o : $(LIBDIR)/AnalysisConfig.cpp $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h
	$(CC) $(CFLAGS) $(LIBDIR)/AnalysisConfig.cpp -o $(OBJDIR)/AnalysisConfig.o $(ROOTFLAGS)

$(OBJDIR)/LeptonSFData.o : $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LeptonSFData.cpp
	$(CC) $(CFLAGS) $(LIBDIR)/LeptonSFData.cpp -o $(OBJDIR)/LeptonSFData.o $(ROOTFLAGS) -lboost_program_options

clean :
	rm ControlPlotMaker/obj/*.o ControlPlotMaker.exe
