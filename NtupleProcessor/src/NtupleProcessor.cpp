/*------------------------------------------------------------------------------
NtupleProcessor.cpp

 Created : 2015-09-14  godshalk
 Modified: 2015-10-28  godshalk

------------------------------------------------------------------------------*/

#include <iostream>
#include <TFile.h>
#include <TString.h>
#include "../interface/NtupleProcessor.h"

using std::cout;   using std::endl;   using std::vector;

NtupleProcessor::NtupleProcessor(TString ds, TString fnpc, TString fnac, TString o, int me)
:  dataset(ds), runParams(fnpc), eHandler(fnac,o), tIter(eHandler, hExtractors), options(o), maxEvents(me)
{
  // TEST output
    cout << "    NtupleProcessor: Created.\n"
            "      Ntuple Processing Config File: " << fnpc << "\n"
            "      Options:                       " << options   << "\n"
         << endl;

  // Open and set up appropriate file & tree
    TFile *testFile = TFile::Open(runParams.fn_ntuple[dataset.Data()]);
    if(!testFile) cout << "NtupleProcessor: ERROR: Unable to open file " << runParams.fn_ntuple[dataset.Data()] << endl;
    TTree *ntuple   = (TTree*) testFile->Get("tree");

  // Create HistogramExtractors from strings from NtupleProcConfig
    TString testString = "";
    hExtractors.push_back(createHistogramExtractorFromString(testString));

  // Process the ntuple using the given tree iterator.
    if(maxEvents!=-1) ntuple->Process(&tIter, options, maxEvents);
    else              ntuple->Process(&tIter, options);

}


HistogramExtractor* NtupleProcessor::createHistogramExtractorFromString(TString& inputString)
{

return NULL;
}
