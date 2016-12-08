#!/bin/bash

# Set up env and unpack all files
source /cvmfs/cms.cern.ch/cmsset_default.sh 
export SCRAM_ARCH="slc6_amd64_gcc530" 
cd ${_CONDOR_SCRATCH_DIR} 
pwd 
cd /uscms_data/d2/godshalk/root6SetupDir/CMSSW_8_0_19/src 
eval `scramv1 runtime -sh` 
cd - 
tar -zxf zcfiles.tgz 


# Set up variables to select files to run cmsRun, ZCAnalyzer over
let jobNum=$1+1
dsName=`sed "${jobNum}q;d" dsList.txt`
echo "  Using dataset $dsName"

# Run the main process
#./NtupleProcessor.exe -d $dsName -n zcNPC_runII.ini -a zcAnalysisConfig_runII.ini
./NtupleProcessor.exe -d $dsName -n zcNPC_2016.ini -a zcAC_runII_noSF.ini

# Clean up
rm NtupleProcessor.exe
rm -r NtupleProcessor
rm ZCLibrary/ -r
rm zcAC_runII_noSF.ini
rm zcNPC_2016.ini
rm zcfiles.tgz
rm zcControlPlotConfig_default.ini
