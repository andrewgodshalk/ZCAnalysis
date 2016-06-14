#ifndef GUARD_EventHandler_h
#define GUARD_EventHandler_h

/*------------------------------------------------------------------------------
   EventHandler Class

 Created : 2015-11-09  godshalk
 Modified: 2015-02-23  godshalk

 Class used to interact with the event handed to it by a TTree iterator.
 Does the bulk work with selection and calculation of secondary variables.
 Interacts directly with the AnalysisConfig object in order to select events
   and objects.

TO DO
- Add cut table functions, or make a dedicated class to handle this.

------------------------------------------------------------------------------*/

#include <list>
#include <sstream>
#include <vector>
#include <TTree.h>
#include <TString.h>
#include "../../ZCLibrary/AnalysisConfig.h"

typedef unsigned int Index;
typedef unsigned long counter;

class EventHandler {

public:
    EventHandler(TString, TString o="");
    virtual ~EventHandler(){}

    const static int maxNumJets  = 130;   /// TEMPORARY WORKING VARIABLE - will be included in config file?
    const static int maxNumMuons = 100;   /// TEMPORARY WORKING VARIABLE - will be included in config file?
    const static int maxNumElecs = 100;   /// TEMPORARY WORKING VARIABLE - will be included in config file?

    AnalysisConfig anCfg;

  // Methods
    bool mapTree(TTree*);                         // Maps class variables to an input TTree.
    void evalCriteria();                          // Evaluates the class' list of event selection criteria
    bool triggered(std::vector<int>&);            // Returns if this event triggered one of the listed triggers.
    void resetSelectionVariables();               // Resets all selection variables to false.
    void printJets();                             // Test function that prints jets and their properties.

  // Running Variables
    TString options;         // Options input with TreeIterator.

// Variables for mapping tree entries. Public so they can be referred to by outside classes.
    float m_Z_mass;     int   m_nJets;                 int   m_nMuons;                     int   m_nElecs;                     float m_MET_et      ;
    float m_Z_pt  ;     float m_jet_pt [maxNumJets];   float m_muon_pt    [maxNumMuons];   float m_elec_pt    [maxNumElecs];   float m_MET_phi     ;
    float m_Z_eta ;     float m_jet_eta[maxNumJets];   float m_muon_eta   [maxNumMuons];   float m_elec_eta   [maxNumElecs];   float m_MET_sumet   ;
    float m_Z_phi ;     float m_jet_phi[maxNumJets];   float m_muon_phi   [maxNumMuons];   float m_elec_phi   [maxNumElecs];   float m_MET_sig     ;
    int   m_Vtype ;     float m_jet_csv[maxNumJets];   int   m_muon_charge[maxNumMuons];   int   m_elec_charge[maxNumElecs];   bool  m_triggers[54];
    int   m_zdecayMode; float m_jet_msv[maxNumJets];   float m_muon_iso   [maxNumMuons];   float m_elec_iso   [maxNumElecs];   float m_json      ;
                        int   m_jet_flv[maxNumJets];                                                                           int   m_event;
    float m_wt_diEle;
    float m_wt_diMuon;

// Calculated variables
    float Z_DelR, Z_DelPhi, Z_DelEta;

  // Selection Variables
    bool usingSim; // Simulation events. For plotting sim-truth information.
    bool usingDY ;

    bool inJSON             ;
    bool isElTriggered      ;
    bool isMuTriggered      ;
    bool hasValidElectrons  ;
    bool hasValidMuons      ;
    bool hasValidZBosonMass ;
    bool isZllEvent         ; // Combination of all above checks
    bool isZeeEvent         ; //   Don't care about jet checks by themselves
    bool isZuuEvent         ;
    bool zIsFromTaus        ;
    bool hasValidMET        ;
    bool isZpJEvent         ;
    bool isZHFEvent         ;
    bool hasBJet            ;
    bool hasCJet            ;
    bool zBosonFromTaus     ;
    bool usingTopHalfDY     ;
    bool usingBottomHalfDY  ;
    bool usingEvenEventDY   ;
    bool usingOddEventDY  ;

  // Lepton Selection Variables
    std::vector<Index> validMuons, validElectrons;    // List of the indexes of muon objects, eventually ordered by pt.

  // Jet Selection Variables
    std::vector<Index> validJets;       // lists all "valid" jets from standard cuts.
    // subsequent vectors contain true/false conditions of jets as ordered in above validjets vector.
    std::map<TString, std::vector<bool> >    HFJets;
    std::map<TString, bool>               hasHFJets;
    std::vector<bool>                       bMCJets;
    std::vector<bool>                       cMCJets;
    std::vector<bool>                       lMCJets;
    Index leadBJet, leadCJet;
    std::map<TString, Index> leadHFJet;

  // Extra data
    long patEventsAnalyzed;     // Number of events that were processed to make the Ntuple.
    long entriesInNtuple  ;     // Number of events that were processed to make the Ntuple.
    // Need to figure out how to do within this scope... might need to handle externally

};

#endif
