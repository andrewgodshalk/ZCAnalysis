# Make file for ZCAnalysis

CC=g++
CFLAGS=-c -Wall 
ROOTFLAGS=`root-config --cflags --glibs`

all : NtupleProcessor ControlPlotMaker
	$(CC) EffPlotAndCalc.cpp -o EffPlotAndCalc.exe $(ROOTFLAGS) -lboost_program_options
	make -f NtupleProcessor/makefile
	make -f ControlPlotMaker/makefile

clean :
	make clean -f NtupleProcessor/makefile
	make clean -f ControlPlotMaker/makefile

