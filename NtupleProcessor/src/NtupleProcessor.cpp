/*------------------------------------------------------------------------------
NtupleProcessor.cpp

 Created : 2015-09-14  godshalk
 Modified: 2015-10-28  godshalk

------------------------------------------------------------------------------*/

#include <iostream>
#include <TFile.h>
#include <TString.h>
#include "../interface/NtupleProcessor.h"
#include "../interface/ControlPlotExtractor.h"
#include "../interface/EffPlotExtractor.h"

using std::cout;   using std::endl;   using std::vector;

NtupleProcessor::NtupleProcessor(TString ds, TString fnpc, TString fnac, TString o, int me)
:  dataset(ds), runParams(fnpc), eHandler(fnac,o), tIter(eHandler, hExtractors), options(o), maxEvents(me)
{
  // TEST output
    cout << "  NtupleProcessor: Created.\n"
            "    Ntuple Processing Config File: " << fnpc << "\n"
            "    Dataset:                       " << dataset   << "\n"
            "    Options:                       " << options   << "\n"
         << endl;

  // Extract options
    usingSim = (options.Contains("Sim", TString::kIgnoreCase) ? true : false);
    usingDY  = (options.Contains("DY" , TString::kIgnoreCase) ? true : false);

  // Open and set up appropriate file & tree
    ntupleFile = TFile::Open(runParams.fn_ntuple[dataset.Data()]);
    if(!ntupleFile) cout << "NtupleProcessor: ERROR: Unable to open file " << runParams.fn_ntuple[dataset.Data()] << endl;
    TTree *ntuple   = (TTree*) ntupleFile->Get("tree");

  // Extra, kind of slap-dash step to add the PAT processing count to cut table...
    eHandler.patEventsAnalyzed = ((TH1F*) ntupleFile->Get("Count"))->GetBinContent(1);

  // Open output file
    outputFile = TFile::Open(runParams.fn_output, "UPDATE");

  // Create HistogramExtractors from strings from NtupleProcConfig
    for(auto& heStr : runParams.hExtractorStrs) createHistogramExtractorFromString(heStr);

  // Process the ntuple using the given tree iterator.
    if(maxEvents!=-1) ntuple->Process(&tIter, options, maxEvents);
    else              ntuple->Process(&tIter, options);
    cout << endl;

  // When finished, save to file.
    for(auto& he : hExtractors)
    { // Find/create output directory and save to it.
        he.second->saveToFile();
    }

    outputFile->Close();
}


void NtupleProcessor::createHistogramExtractorFromString(TString& inputString)
{ // Create histogram extractors

  // Create different options based on dataset.
    vector<TString> dsOptions;

    // For each sim set, split by Zuu and Zee
    if(dataset=="muon" || usingSim) dsOptions.push_back("Zuu");
    if(dataset=="elec" || usingSim) dsOptions.push_back("Zee");
    // For DY, split by {Zuu, Zee}x{Total, Ztautau, Z+l, Z+c, Z+b}
    if(usingDY)
    {
        dsOptions.push_back("Zuu_Ztautau");
        dsOptions.push_back("Zee_Ztautau");
        dsOptions.push_back("Zuu_Zl"    );
        dsOptions.push_back("Zuu_Zc"    );
        dsOptions.push_back("Zuu_Zb"    );
        dsOptions.push_back("Zee_Zl"    );
        dsOptions.push_back("Zee_Zc"    );
        dsOptions.push_back("Zee_Zb"    );
    }

    if(inputString == "ControlPlotExtractor")
    {
        TString dirName = "control_plots/";
        for(TString dsOp : dsOptions)
        {
            dirName = TString("control_plots/")+dataset+"/"+dsOp;
            cout << "  NtupleProcessor::createHistogramExtractorFromString: Adding ControlPlotExtractor (" << dirName << ")" << endl;

          // Check if desired directory already exists. If not, create it.
            cout << "    Checking file: " << outputFile->GetName() << endl;
            TDirectory* outDir = outputFile->GetDirectory(dirName);
            if(outDir) cout << "     Found directory: " << outDir->GetName() << endl;
            else {     cout << "     Directory " << dirName << " not found in file. Creating new directory." << endl;
                outputFile->mkdir(dirName);
                outDir = outputFile->GetDirectory(dirName);
                if(outDir) cout << "     Directory created: " << outDir->GetName() << endl;
                else       cout << "     Directory " << dirName << " still not working. Fix your code." << endl;
            }

            hExtractors[dirName] = new ControlPlotExtractor(eHandler, outDir, options+dsOp);
        }
    }
    if(inputString == "EffPlotExtractor")
    {
        TString dirName = "raw_eff_plots/";
        for(TString dsOp : dsOptions)
        {
            dirName = TString("raw_eff_plots/")+dataset+"/"+dsOp;
            cout << "  NtupleProcessor::createHistogramExtractorFromString: Adding EffPlotExtractor (" << dirName << ")" << endl;

          // Check if desired directory already exists. If not, create it.
            cout << "    Checking file: " << outputFile->GetName() << endl;
            TDirectory* outDir = outputFile->GetDirectory(dirName);
            if(outDir) cout << "     Found directory: " << outDir->GetName() << endl;
            else {     cout << "     Directory " << dirName << " not found in file. Creating new directory." << endl;
                outputFile->mkdir(dirName);
                outDir = outputFile->GetDirectory(dirName);
                if(outDir) cout << "     Directory created: " << outDir->GetName() << endl;
                else       cout << "     Directory " << dirName << " still not working. Fix your code." << endl;
            }

            hExtractors[dirName] = new EffPlotExtractor(eHandler, outDir, options+dsOp);
        }
    }
}
