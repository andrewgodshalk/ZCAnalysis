#!/bin/bash

# Set up env and unpack all files

#source /cvmfs/cms.cern.ch/cmsset_default.csh
#setenv SCRAM_ARCH slc6_amd64_gcc481
export SCRAM_ARCH="slc6_amd64_gcc491"
cd ${_CONDOR_SCRATCH_DIR}
pwd
cd /uscms_data/d2/godshalk/root6SetupDir/CMSSW_7_5_4/src
eval `scramv1 runtime -sh`
cd -

# Set up variables to select files to run cmsRun, ZCAnalyzer over
let jobNum=$1+1
dsName=`sed "${jobNum}q;d" dsList.txt`
echo "  Using dataset $dsName"

# Run the main process
./NtupleProcessor.exe -d $dsName -n zcNPC_runII.ini -a zcAnalysisConfig_runII.ini

# Clean up
rm NtupleProcessor.exe
rm zcNPC_RunII.ini
rm zcAnalysisConfig_runII.ini 
rm zcControlPlotConfig_default.ini
