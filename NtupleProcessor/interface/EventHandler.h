#ifndef GUARD_EventHandler_h
#define GUARD_EventHandler_h

/*------------------------------------------------------------------------------
   EventHandler Class

 Created : 2015-11-09  godshalk
 Modified: 2015-11-09  godshalk

 Class used to interact with the event handed to it by a TTree iterator.
 Does the bulk work with selection and calculation of secondary variables.
 Interacts directly with the AnalysisConfig object in order to select events
   and objects.

TO DO
- Complete class.
- Add cut table functions, or make a dedicated class to handle this.

------------------------------------------------------------------------------*/

#include <sstream>
#include <vector>
#include <TTree.h>

typedef unsigned int Index;

class EntryHandler {

public:
    EntryHandler(TTree*, TString);
    virtual ~EntryHandler(){}

    const static int maxNumJets  = 130;   /// TEMPORARY WORKING VARIABLE - will be included in config file?
    const static int maxNumMuons = 130;   /// TEMPORARY WORKING VARIABLE - will be included in config file?
    const static int maxNumElecs = 130;   /// TEMPORARY WORKING VARIABLE - will be included in config file?


  // Methods
    bool mapTree(TTree*);                         // Maps class variables to an input TTree.
    void evalCriteria();                          // Evaluates the class' list of event selection criteria
    bool triggered(std::vector<int>&);            // Returns if this event triggered one of the listed triggers.

  // Running Variables
    TString options;         // Options input with TreeIterator.
//    std::stringstream log;   // log stream for recording event-by-event output of histogram makers.

// Variables for mapping tree entries. Public so they can be referred to by outside classes.
    float m_Z_mass;   int   m_nJets;                 int   m_nMuons;                     int   m_nElecs;                     float m_MET_et      ;
    float m_Z_pt  ;   float m_jet_pt [maxNumJets];   float m_muon_pt    [maxNumMuons];   float m_elec_pt    [maxNumElecs];   float m_MET_phi     ;
    float m_Z_eta ;   float m_jet_eta[maxNumJets];   float m_muon_eta   [maxNumMuons];   float m_elec_eta   [maxNumElecs];   bool  m_triggers[54];
    float m_Z_phi ;   float m_jet_phi[maxNumJets];   float m_muon_phi   [maxNumMuons];   float m_elec_phi   [maxNumElecs];   int   m_json        ;
    int   m_Vtype ;   float m_jet_csv[maxNumJets];   float m_muon_charge[maxNumMuons];   float m_elec_charge[maxNumElecs];
                      float m_jet_msv[maxNumJets];   float m_muon_iso   [maxNumMuons];
                      float m_jet_flv[maxNumJets];

  // Selection Variables
    bool usingSim  ; // Simulation events. For plotting sim-truth information.
    bool usingZee  ; // Which Z decay chain we're interested in.
    bool usingZuu  ;

    bool eventinjson;
    bool validmuons;
    bool validelectrons;
    bool validzboson;
    bool validmet;
    bool validstdevent;
    bool validzeeevent;
    bool validzuuevent;
    bool validzpjevent;

  // jet selection variables
    std::vector<Index> validjets;       // lists all "valid" jets from standard cuts.
    // subsequent vectors contain true/false conditions of jets as ordered in above validjets vector.
    std::map<TString, std::vector<bool> >    hfjets;
    std::map<TString, bool>               hashfjets;
    std::vector<bool>                       bmcjets;   bool hasbjet;
    std::vector<bool>                       cmcjets;   bool hascjet;
    std::vector<bool>                       lmcjets;


};

#endif
