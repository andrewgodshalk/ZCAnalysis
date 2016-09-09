# Make file for ZCAnalysis/NtupleProcessor/test/leptonSF_test

CC=g++
CFLAGS=-c -Wall 
ROOTFLAGS=`root-config --cflags --glibs`
OBJDIR=obj
OBJS=$(OBJDIR)/main.o $(OBJDIR)/LeptonSFData.o
HDRDIR=.
SRCDIR=.

leptonSF_test : createObjDir leptonSF_test.exe

createObjDir : 
	mkdir -p obj

leptonSF_test.exe : $(OBJS)
	$(CC) $(OBJS) -o leptonSF_test.exe $(ROOTFLAGS) -lboost_program_options

$(OBJDIR)/main.o : main.cpp $(HDRDIR)/LeptonSFData.h
	$(CC) $(CFLAGS) main.cpp -o $(OBJDIR)/main.o $(ROOTFLAGS)

$(OBJDIR)/LeptonSFData.o : $(SRCDIR)/LeptonSFData.cpp $(HDRDIR)/LeptonSFData.h
	$(CC) $(CFLAGS) $(SRCDIR)/LeptonSFData.cpp -o $(OBJDIR)/LeptonSFData.o $(ROOTFLAGS)

clean :
	rm $(OBJS) LeptonSFData.exe
