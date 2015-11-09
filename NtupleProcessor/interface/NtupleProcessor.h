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
#include "NtupleProcConfig.h"


class NtupleProcessor
{
  public:
    NtupleProcessor(TString, TString o = "");  // Primary constructor.
    NtupleProcessor() : NtupleProcessor("ZCLibrary/zc_standard_config.ini") {}   // Default constructor. Call primary constructor w/ a default file.
   ~NtupleProcessor(){}

    NtupleProcConfig runParams;    // Contains options specific for NtupleProcessor to run

  private: 
    TString options;    // Options for this processing.

};

#endif
