#!/bin/bash

# Set up env and unpack all files
source /cvmfs/cms.cern.ch/cmsset_default.sh 
export SCRAM_ARCH="slc6_amd64_gcc530" 
cd ${_CONDOR_SCRATCH_DIR} 
pwd 
cd /uscms_data/d2/godshalk/root6SetupDir/CMSSW_8_0_25/src 
eval `scramv1 runtime -sh` 
cd - 
tar -zxf zcfiles.tgz 


# Set up variables to select files to run cmsRun, ZCAnalyzer over
let jobNum=$1+1
dsInfo=`sed "${jobNum}q;d" dsList.txt`
echo "  Dataset Info: $dsInfo"
read ntuple dataset file <<< $dsInfo
echo $ntuple
echo $dataset
echo $file

# Run the main process
#./NtupleProcessor.exe -d $dsName -n zcNPC_runII.ini -a zcAnalysisConfig_runII.ini
#./NtupleProcessor.exe -d $dsName -n zcNPC_2016.ini -a zcAC_runII_noSF.ini
#./NtupleProcessor.exe -d $dsName -n zcNPC_2016_SingleLepSets.ini -a zcAC_runII_muonIDISO_2016-12-22.ini
./NtupleProcessor.exe -N $file -L $ntuple -d $dataset -n zcNPC.ini -a zcAC.ini

# Clean up
rm NtupleProcessor.exe
rm -r NtupleProcessor
rm ZCLibrary/ -r
rm zcAC*
rm zcNPC*
rm dsList.txt
rm zcfiles.tgz
rm zcControlPlotConfig_default.ini
