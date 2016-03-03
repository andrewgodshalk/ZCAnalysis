# Make file for ZCAnalysis

CC=g++
CFLAGS=-c -Wall 
ROOTFLAGS=`root-config --cflags --glibs`

all : NtupleProcessor ControlPlotMaker EffPlotAndCalc
	make -f NtupleProcessor/makefile
	make -f ControlPlotMaker/makefile

EffPlotAndCalc :
	$(CC) EffPlotAndCalc.cpp -o EffPlotAndCalc.exe $(ROOTFLAGS) -lboost_program_options

clean :
	make clean -f NtupleProcessor/makefile
	make clean -f ControlPlotMaker/makefile

