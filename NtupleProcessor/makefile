# Make file for ZCAnalysis/NtupleProcessor package

CC=g++
CFLAGS=-c -Wall
ROOTFLAGS=`root-config --cflags --glibs`
OBJDIR=NtupleProcessor/obj
OBJS=$(OBJDIR)/main.o $(OBJDIR)/NtupleProcessor.o $(OBJDIR)/NtupleProcConfig.o

NtupleProcessor : NtupleProcessor.exe

NtupleProcessor.exe : $(OBJS)
	$(CC) $(OBJS) -o NtupleProcessor.exe $(ROOTFLAGS)

$(OBJDIR)/main.o : NtupleProcessor/main.cpp ZCLibrary/timestamp.h NtupleProcessor/interface/NtupleProcessor.h NtupleProcessor/interface/NtupleProcConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) NtupleProcessor/main.cpp -o $(OBJDIR)/main.o $(ROOTFLAGS)

$(OBJDIR)/NtupleProcessor.o : NtupleProcessor/src/NtupleProcessor.cpp NtupleProcessor/interface/NtupleProcessor.h NtupleProcessor/interface/NtupleProcConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) NtupleProcessor/src/NtupleProcessor.cpp -o $(OBJDIR)/NtupleProcessor.o $(ROOTFLAGS)

$(OBJDIR)/NtupleProcConfig.o : NtupleProcessor/src/NtupleProcConfig.cpp NtupleProcessor/interface/NtupleProcConfig.h ZCLibrary/ConfigReader.h
	$(CC) $(CFLAGS) NtupleProcessor/src/NtupleProcConfig.cpp -o $(OBJDIR)/NtupleProcConfig.o $(ROOTFLAGS)

clean :
	rm NtupleProcessor/obj/*.o NtupleProcessor.exe
