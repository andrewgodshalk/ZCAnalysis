#ifndef GUARD_NtupleProcessor_h
#define GUARD_NtupleProcessor_h

/*------------------------------------------------------------------------------
   NtupleProcessor

 Created : 2015-09-14  godshalk
 Modified: 2015-09-14  godshalk

 Main class of NtupleProcessor program. Created to handle input and running of
 ntuple processing.

 Takes input variables (datasets, configurations, etc) and sets up the
 appropriate classes to handle each portion of the analysis process.

------------------------------------------------------------------------------*/
#include <TString.h>
#include <vector>
#include "EventHandler.h"
#include "HistogramExtractor.h"
#include "NtupleProcConfig.h"
#include "TreeIterator.h"


class NtupleProcessor
{
  public:
    NtupleProcessor(TString, TString fnnp="NtupleProcessor/etc/zcNtupleProcConfig_default.ini", TString fnac="NtupleProcessor/etc/zcAnalysisConfig_default.ini", TString o = "", int me=-1);  // Primary constructor.
   ~NtupleProcessor(){}

  private:
    TString           dataset;
    NtupleProcConfig  runParams;      // Contains options specific for NtupleProcessor to run
    EventHandler      eHandler;       // TreeIterator and EventHandler for running over the ntuple and extracting the desired information.
    TreeIterator      tIter;
    TString           options;        // Extra options for this processing.
    int               maxEvents;      // Max number of events to run over in this ntuple
    TFile*            ntupleFile;     // File from which the ntuple is pulled.
    TFile*            outputFile;     // ROOT output file where histograms are stored.

    bool usingSim;
    bool usingDY;

    std::map<TString, HistogramExtractor*> hExtractors;    // Map of histogram extractors. Key is the output directory within the output file where histograms are stored.

    void createHistogramExtractorFromString(TString&);
    void getInputFileList(TString, std::vector<TString>& );

};

#endif
