# Make file for ZCAnalysis

all : createObjDir NtupleProcessor
	make -f NtupleProcessor/makefile

createObjDir : 
	mkdir -p NtupleProcessor/obj

clean :
	make clean -f NtupleProcessor/makefile
