#ifndef GUARD_ControlPlotConfig_h
#define GUARD_ControlPlotConfig_h

/*------------------------------------------------------------------------------
   ControlPlotConfig Class

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
#include "../../ZCLibrary/ConfigReader.h"

class ControlPlotConfig : public ConfigReader
{
  public:
    ControlPlotConfig( TString fnc = "NtupleProcessor/etc/controlPlotConfig_default.ini");
   ~ControlPlotConfig(){}

  // Config variables
    std::map<std::string, TString>       fn_ntuple              ;    // Ntuple locations.
    std::string                          fn_output              ;
    std::map<std::string, float>         setWeight              ;    // Number used to scale simulated signal and background based on luminosity, cross section, and dataset size.
    std::map<std::string, float>         flatHFTagSF            ;    // Scale factor to apply heavy flavor tagged data.
    std::map<std::string, float>         stdCSVOpPts            ;
    float                                minSVT          =    0 ;
    float                                noSVT           = -100 ;
    int                                  maxNumJets      =  130 ;    // Max number of raw jets  stored in ntuples per event.
    int                                  maxNumMuons     =  100 ;    // Max number of raw muons stored in ntuples.
    int                                  maxNumElecs     =  100 ;
    float                                muonPtMin              ;
    float                                muonEtaMax             ;
    float                                elecPtMin              ;
    float                                elecEtaInnerMax        ;
    float                                elecEtaOuterMin        ;
    float                                elecEtaOuterMax        ;
    float                                dilepInvMassMin        ;
    float                                dilepInvMassMax        ;
    std::string                          dilepMuonCharge        ;
    std::string                          dilepElecCharge        ;
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