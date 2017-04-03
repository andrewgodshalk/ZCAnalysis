#!/bin/bash

# Set up env and unpack all files
echo " ---> SETTING UP ENVIRONMENT"
source /cvmfs/cms.cern.ch/cmsset_default.sh 
export SCRAM_ARCH="slc6_amd64_gcc530" 
cd ${_CONDOR_SCRATCH_DIR} 
eval `scramv1 project CMSSW CMSSW_8_0_25`
cd CMSSW_8_0_25/src/
eval `scram runtime -sh` 
echo "CMSSW: $CMSSW_BASE"
echo "Current directory: "`pwd`
cd - 
pwd 
tar -zxf zcfiles.tgz 
mkdir -p condor_logs 

# Print infomation on environment
printenv
echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
echo "System software: `cat /etc/redhat-release`" #Operating System on that node
echo "hostname: " `hostname`

# Set up variables to select files to run cmsRun, ZCAnalyzer over
let jobNum=$1+1
dsInfo=`sed "${jobNum}q;d" dsList.txt`
echo "  Dataset Info: $dsInfo"
read ntuple dataset file <<< $dsInfo
echo $ntuple
echo $dataset
echo $file

# Run the main process
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
rm CMSSW_8_0_25 -r
