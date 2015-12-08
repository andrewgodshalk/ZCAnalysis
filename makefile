# Make file for ZCAnalysis

all : NtupleProcessor ControlPlotMaker
	make -f NtupleProcessor/makefile
	make -f ControlPlotMaker/makefile

clean :
	make clean -f NtupleProcessor/makefile
	make clean -f ControlPlotMaker/makefile
	