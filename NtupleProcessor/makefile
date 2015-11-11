# Make file for ZCAnalysis/NtupleProcessor package

CC=g++
CFLAGS=-c -Wall 
ROOTFLAGS=`root-config --cflags --glibs`
OBJDIR=NtupleProcessor/obj
OBJS=$(OBJDIR)/main.o $(OBJDIR)/NtupleProcessor.o $(OBJDIR)/NtupleProcConfig.o $(OBJDIR)/TreeIterator.o $(OBJDIR)/EventHandler.o $(OBJDIR)/ControlPlotExtractor.o $(OBJDIR)/AnalysisConfig.o $(OBJDIR)/ControlPlotConfig.o
HDRDIR=NtupleProcessor/interface
SRCDIR=NtupleProcessor/src

NtupleProcessor : NtupleProcessor.exe

NtupleProcessor.exe : $(OBJS)
	$(CC) $(OBJS) -o NtupleProcessor.exe $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/main.o : NtupleProcessor/main.cpp ZCLibrary/timestamp.h $(HDRDIR)/NtupleProcessor.h $(HDRDIR)/NtupleProcConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) NtupleProcessor/main.cpp -o $(OBJDIR)/main.o $(ROOTFLAGS)

$(OBJDIR)/NtupleProcessor.o : $(SRCDIR)/NtupleProcessor.cpp $(HDRDIR)/NtupleProcessor.h $(HDRDIR)/NtupleProcConfig.h ZCLibrary/ConfigReader.h $(HDRDIR)/TreeIterator.h $(HDRDIR)/EventHandler.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/ControlPlotExtractor.h
	$(CC) $(CFLAGS) $(SRCDIR)/NtupleProcessor.cpp -o $(OBJDIR)/NtupleProcessor.o $(ROOTFLAGS)

$(OBJDIR)/TreeIterator.o : $(SRCDIR)/TreeIterator.cpp $(HDRDIR)/TreeIterator.h $(HDRDIR)/EventHandler.h $(HDRDIR)/HistogramExtractor.h
	$(CC) $(CFLAGS) $(SRCDIR)/TreeIterator.cpp -o $(OBJDIR)/TreeIterator.o $(ROOTFLAGS)

$(OBJDIR)/EventHandler.o : $(SRCDIR)/EventHandler.cpp $(HDRDIR)/EventHandler.h
	$(CC) $(CFLAGS) $(SRCDIR)/EventHandler.cpp -o $(OBJDIR)/EventHandler.o $(ROOTFLAGS)

$(OBJDIR)/ControlPlotExtractor.o : $(SRCDIR)/ControlPlotExtractor.cpp $(HDRDIR)/ControlPlotExtractor.h $(HDRDIR)/HistogramExtractor.h $(HDRDIR)/EventHandler.h
	$(CC) $(CFLAGS) $(SRCDIR)/ControlPlotExtractor.cpp -o $(OBJDIR)/ControlPlotExtractor.o $(ROOTFLAGS)

$(OBJDIR)/NtupleProcConfig.o : $(SRCDIR)/NtupleProcConfig.cpp $(HDRDIR)/NtupleProcConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/NtupleProcConfig.cpp -o $(OBJDIR)/NtupleProcConfig.o $(ROOTFLAGS)

$(OBJDIR)/AnalysisConfig.o : $(SRCDIR)/AnalysisConfig.cpp $(HDRDIR)/AnalysisConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/AnalysisConfig.cpp -o $(OBJDIR)/AnalysisConfig.o $(ROOTFLAGS)
	
$(OBJDIR)/ControlPlotConfig.o : $(SRCDIR)/ControlPlotConfig.cpp $(HDRDIR)/ControlPlotConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) $(SRCDIR)/ControlPlotConfig.cpp -o $(OBJDIR)/ControlPlotConfig.o $(ROOTFLAGS)

clean :
	rm NtupleProcessor/obj/*.o NtupleProcessor.exe