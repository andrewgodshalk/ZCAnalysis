#ifndef GUARD_AnalysisConfig_h
#define GUARD_AnalysisConfig_h

/*------------------------------------------------------------------------------
   AnalysisConfig Class

 Created : 2015-11-09  godshalk
 Modified: 2015-11-09  godshalk

 Class designed to contain and provide standard variables for Z+c Analysis code.
 
 Child class of ConfigReader.h. Takes a ini-like configuration file with the
 desired variables to be entered.

------------------------------------------------------------------------------*/

#include <map>
#include <utility>
#include <string>
#include <vector>
#include <TString.h>
#include "../ZCLibrary/ConfigReader.h"

class AnalysisConfig : public ConfigReader
{
  public:
    AnalysisConfig(TString fnc = "NtupleProcessor/etc/analysisConfig_default.ini");
   ~AnalysisConfig(){}

  // Config variables
    std::string                          fn_output              ;
    std::map<std::string, float>         setWeight              ;    // Number used to scale simulated signal and background based on luminosity, cross section, and dataset size.
    std::map<std::string, float>         flatHFTagSF            ;    // Scale factor to apply heavy flavor tagged data.
    std::map<std::string, float>         stdCSVOpPts            ;
    float                                minSVT          =    0 ;
    float                                noSVT           = -100 ;
    float                                muonPtMin              ;
    float                                muonEtaMax             ;
    float                                muonIsoMax             ;
    float                                elecPtMin              ;
    float                                elecEtaInnerMax        ;
    float                                elecEtaOuterMin        ;
    float                                elecEtaOuterMax        ;
    float                                elecIsoMax             ;
    float                                dilepInvMassMin        ;
    float                                dilepInvMassMax        ;
    bool                                 dilepMuonReqOppSign    ;
    bool                                 dilepElecReqOppSign    ;
    float                                metMax                 ;
    float                                nJetsAnalyzed          ;
    float                                jetPtMin               ;
    float                                jetEtaMax              ;
    bool                                 jsonSelect             ;
    std::vector<int>                     muonTriggers           ;
    std::vector<int>                     elecTriggers           ;
    std::vector<std::pair<float,float> > jetPtBins              ;
    std::vector<std::pair<float,float> > jetEtaBins             ;
    std::vector<std::pair<float,float> > dileptonPtBins         ;

  private: 
    void processBinString(std::vector<std::pair<float,float> >&, std::string&);  // For processing input string of bin variables. Outputs set of pairs with integrated pair first.
    void processTriggerString(std::vector<int>&, std::string&);              // For processing input string of trigger numbers.


};



#endif
