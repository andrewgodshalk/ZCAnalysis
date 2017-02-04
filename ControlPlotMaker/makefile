# Make file for ZCAnalysis/ControlPlotMaker package

CC=g++
CFLAGS=-c -Wall
ROOTFLAGS=`root-config --cflags --glibs`
LIBDIR=ZCLibrary
PRGDIR=ControlPlotMaker
OBJDIR=$(PRGDIR)/obj
HDRDIR=$(PRGDIR)/interface
SRCDIR=$(PRGDIR)/src
OBJS=$(OBJDIR)/main.o $(OBJDIR)/ControlPlotMaker.o $(OBJDIR)/AnalysisConfig.o $(OBJDIR)/LeptonSFData.o $(OBJDIR)/LumiJSONReader.o  $(OBJDIR)/BTagCalibrationStandalone.o $(OBJDIR)/JetTagWeight.o

ControlPlotMaker : createObjDir ControlPlotMaker.exe

createObjDir :
	mkdir -p ControlPlotMaker/obj
	mkdir -p ControlPlotMaker/plots
	mkdir -p ControlPlotMaker/epsplots

ControlPlotMaker.exe : $(OBJS)
	$(CC) $(OBJS) -o ControlPlotMaker.exe $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/main.o : $(PRGDIR)/main.cpp $(HDRDIR)/ControlPlotMaker.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h $(LIBDIR)/BTagCalibrationStandalone.h $(LIBDIR)/JetTagWeight.h
	$(CC) $(CFLAGS) $(PRGDIR)/main.cpp -o $(OBJDIR)/main.o $(ROOTFLAGS)

$(OBJDIR)/ControlPlotMaker.o : $(SRCDIR)/ControlPlotMaker.cpp $(HDRDIR)/ControlPlotMaker.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h $(LIBDIR)/BTagCalibrationStandalone.h $(LIBDIR)/JetTagWeight.h
	$(CC) $(CFLAGS) $(SRCDIR)/ControlPlotMaker.cpp -o $(OBJDIR)/ControlPlotMaker.o $(ROOTFLAGS)

$(OBJDIR)/AnalysisConfig.o : $(LIBDIR)/AnalysisConfig.cpp $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h $(LIBDIR)/BTagCalibrationStandalone.h $(LIBDIR)/JetTagWeight.h
	$(CC) $(CFLAGS) $(LIBDIR)/AnalysisConfig.cpp -o $(OBJDIR)/AnalysisConfig.o $(ROOTFLAGS)

$(OBJDIR)/LeptonSFData.o : $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LeptonSFData.cpp
	$(CC) $(CFLAGS) $(LIBDIR)/LeptonSFData.cpp -o $(OBJDIR)/LeptonSFData.o $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/LumiJSONReader.o : $(LIBDIR)/LumiJSONReader.h $(LIBDIR)/LumiJSONReader.cpp
	$(CC) $(CFLAGS) $(LIBDIR)/LumiJSONReader.cpp -o $(OBJDIR)/LumiJSONReader.o $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/JetTagWeight.o : $(LIBDIR)/JetTagWeight.cpp $(LIBDIR)/JetTagWeight.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h $(LIBDIR)/BTagCalibrationStandalone.h
	$(CC) $(CFLAGS) $(LIBDIR)/JetTagWeight.cpp -o $(OBJDIR)/JetTagWeight.o $(ROOTFLAGS)

$(OBJDIR)/BTagCalibrationStandalone.o : $(LIBDIR)/BTagCalibrationStandalone.cpp $(LIBDIR)/BTagCalibrationStandalone.h
	$(CC) $(CFLAGS) $(LIBDIR)/BTagCalibrationStandalone.cpp -o $(OBJDIR)/BTagCalibrationStandalone.o $(ROOTFLAGS)


clean :
	rm ControlPlotMaker/obj/*.o ControlPlotMaker.exe
