#ifndef GUARD_NtupleProcConfig_h
#define GUARD_NtupleProcConfig_h

/*------------------------------------------------------------------------------
   NtupleProcConfig Class

 Created : 2015-09-15  godshalk
 Modified: 2016-09-08  godshalk

 Class designed to contain and provide standard variables for NtupleProcessor
 and EntryHandler classes.
 
 Child class of ConfigReader.h. Takes a ini-like configuration file with the
 desired variables to be entered.

------------------------------------------------------------------------------*/

#include <map>
#include <utility>
#include <string>
#include <vector>
#include <TString.h>
#include "../../ZCLibrary/ConfigReader.h"

class NtupleProcConfig : public ConfigReader
{
  public:
    NtupleProcConfig( TString fnc = "NtupleProcessor/etc/ntupleProcConfig_default.ini");
   ~NtupleProcConfig(){}

  // Config variables
    std::map<std::string, TString> fn_ntuple;    // Ntuple locations.
    TString     fn_output;                       // Location of output plots.
    int         maxNumJets      =  130      ;    // Max number of raw jets  stored in ntuples per event.
    int         maxNumMuons     =  100      ;    // Max number of raw muons stored in ntuples.
    int         maxNumElecs     =  100      ;

    std::vector<TString> hExtractorStrs; // List of histogram extracters requested by config.

};



#endif
