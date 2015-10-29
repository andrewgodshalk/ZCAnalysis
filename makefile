# Make file for ZCAnalysis

all : NtupleProcessor
	make -f NtupleProcessor/makefile

clean :
	make clean -f NtupleProcessor/makefile
	rm *.o
