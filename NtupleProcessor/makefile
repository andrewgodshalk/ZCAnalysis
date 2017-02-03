# Make file for ZCAnalysis/NtupleProcessor package

#CC=g++
CC=g++-4.9
#CFLAGS=-std=c++14 -c -Wall
CFLAGS=-c -Wall -fdiagnostics-color
ROOTFLAGS=`root-config --cflags --glibs`
OBJDIR=NtupleProcessor/obj
OBJS=$(OBJDIR)/main.o $(OBJDIR)/NtupleProcessor.o $(OBJDIR)/NtupleProcConfig.o $(OBJDIR)/TreeIterator.o $(OBJDIR)/EventHandler.o $(OBJDIR)/ControlPlotExtractor.o $(OBJDIR)/TemplateExtractor.o $(OBJDIR)/EffPlotExtractor.o $(OBJDIR)/AnalysisConfig.o $(OBJDIR)/ControlPlotConfig.o $(OBJDIR)/LeptonSFData.o $(OBJDIR)/LumiJSONReader.o
HDRDIR=NtupleProcessor/interface
SRCDIR=NtupleProcessor/src
LIBDIR=ZCLibrary


NtupleProcessor : createObjDir NtupleProcessor.exe

createObjDir :
	mkdir -p NtupleProcessor/obj

NtupleProcessor.exe : $(OBJS)
	$(CC) $(OBJS) -o NtupleProcessor.exe $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/main.o : NtupleProcessor/main.cpp $(HDRDIR)/NtupleProcessor.h $(HDRDIR)/ControlPlotExtractor.h $(HDRDIR)/TemplateExtractor.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(HDRDIR)/ControlPlotConfig.h $(LIBDIR)/ConfigReader.h $(HDRDIR)/NtupleProcConfig.h $(HDRDIR)/TreeIterator.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) NtupleProcessor/main.cpp -o $(OBJDIR)/main.o $(ROOTFLAGS)

$(OBJDIR)/NtupleProcessor.o : $(SRCDIR)/NtupleProcessor.cpp $(HDRDIR)/NtupleProcessor.h $(HDRDIR)/ControlPlotExtractor.h $(HDRDIR)/TemplateExtractor.h $(HDRDIR)/EffPlotExtractor.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(HDRDIR)/ControlPlotConfig.h $(LIBDIR)/ConfigReader.h $(HDRDIR)/NtupleProcConfig.h $(HDRDIR)/TreeIterator.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/NtupleProcessor.cpp -o $(OBJDIR)/NtupleProcessor.o $(ROOTFLAGS)

$(OBJDIR)/TreeIterator.o : $(SRCDIR)/TreeIterator.cpp $(HDRDIR)/TreeIterator.h $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(HDRDIR)/HistogramExtractor.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/TreeIterator.cpp -o $(OBJDIR)/TreeIterator.o $(ROOTFLAGS)

$(OBJDIR)/EventHandler.o : $(SRCDIR)/EventHandler.cpp $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/EventHandler.cpp -o $(OBJDIR)/EventHandler.o $(ROOTFLAGS)

$(OBJDIR)/ControlPlotExtractor.o : $(SRCDIR)/ControlPlotExtractor.cpp $(HDRDIR)/ControlPlotExtractor.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(HDRDIR)/ControlPlotConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/ControlPlotExtractor.cpp -o $(OBJDIR)/ControlPlotExtractor.o $(ROOTFLAGS)

$(OBJDIR)/TemplateExtractor.o : $(SRCDIR)/TemplateExtractor.cpp $(HDRDIR)/TemplateExtractor.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/TemplateExtractor.cpp -o $(OBJDIR)/TemplateExtractor.o $(ROOTFLAGS)

$(OBJDIR)/EffPlotExtractor.o : $(SRCDIR)/EffPlotExtractor.cpp $(HDRDIR)/EffPlotExtractor.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/EventHandler.h $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/EffPlotExtractor.cpp -o $(OBJDIR)/EffPlotExtractor.o $(ROOTFLAGS)

$(OBJDIR)/NtupleProcConfig.o : $(SRCDIR)/NtupleProcConfig.cpp $(HDRDIR)/NtupleProcConfig.h $(LIBDIR)/ConfigReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/NtupleProcConfig.cpp -o $(OBJDIR)/NtupleProcConfig.o $(ROOTFLAGS)

$(OBJDIR)/AnalysisConfig.o : $(LIBDIR)/AnalysisConfig.cpp $(LIBDIR)/AnalysisConfig.h $(LIBDIR)/ConfigReader.h $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LumiJSONReader.h
	$(CC) $(CFLAGS) $(LIBDIR)/AnalysisConfig.cpp -o $(OBJDIR)/AnalysisConfig.o $(ROOTFLAGS)

$(OBJDIR)/LeptonSFData.o : $(LIBDIR)/LeptonSFData.h $(LIBDIR)/LeptonSFData.cpp
	$(CC) $(CFLAGS) $(LIBDIR)/LeptonSFData.cpp -o $(OBJDIR)/LeptonSFData.o $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/LumiJSONReader.o : $(LIBDIR)/LumiJSONReader.h $(LIBDIR)/LumiJSONReader.cpp
	$(CC) $(CFLAGS) $(LIBDIR)/LumiJSONReader.cpp -o $(OBJDIR)/LumiJSONReader.o $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/ControlPlotConfig.o : $(SRCDIR)/ControlPlotConfig.cpp $(HDRDIR)/ControlPlotConfig.h $(LIBDIR)/ConfigReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/ControlPlotConfig.cpp -o $(OBJDIR)/ControlPlotConfig.o $(ROOTFLAGS)

clean :
	rm NtupleProcessor/obj/*.o NtupleProcessor.exe
