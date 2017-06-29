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

#include <array>
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

    static const std::vector<TString> HFTags;
    static const std::vector<TString> SVType;
    static const std::vector<TString> UncertVariations;

  // Methods
    bool  mapTree(TTree*);                         // Maps class variables to an input TTree.
    void  evalCriteria();                          // Evaluates the class' list of event selection criteria
    bool  triggered(std::vector<int>&);            // Returns if this event triggered one of the listed triggers.
    void  resetSelectionVariables();               // Resets all selection variables to false.
    void  printJets();                             // Test function that prints jets and their properties.
    float calculatePUReweight(int);
    float calculateJetMSVQuickCorrection(int);
    float calculateJetTagEvtWeight(std::string, std::string, std::string uncert = "central", bool debug=false);
    void  processLeptons();
    bool  jetSatisfiesTag(Index, TString);

  // Running Variables
    TString options;         // Options input with TreeIterator.

  // Variables for mapping tree entries. Public so they can be referred to by outside classes.
    float m_Z_mass;     int   m_nJets;                 int   m_nLeps;                     float m_MET_et      ;
    float m_Z_pt  ;     float m_jet_pt [maxNumJets];   float m_lep_pt    [maxNumElecs];   float m_MET_phi     ;
    float m_Z_eta ;     float m_jet_eta[maxNumJets];   float m_lep_eta   [maxNumElecs];   float m_MET_sumet   ;
    float m_Z_phi ;     float m_jet_phi[maxNumJets];   float m_lep_phi   [maxNumElecs];   float m_MET_sig     ;
    float m_Vtype ;     float m_jet_csv[maxNumJets];   int   m_lep_charge[maxNumElecs];   bool  m_triggers[54];
    int   m_zdecayMode; float m_jet_msv[maxNumJets];   float m_lep_iso03 [maxNumElecs];   float m_json        ;
                        int   m_jet_flv[maxNumJets];   float m_lep_iso04 [maxNumElecs];   int   m_event       ;
                                                       int   m_lep_pdgId [maxNumElecs];
    float m_new_lep_charge[2];
    float m_jet_ctagVsL[maxNumJets];
    float m_jet_ctagVsB[maxNumJets];
    int   m_jet_hadflv[maxNumJets];
    int   m_jet_parflv[maxNumJets];
    float m_jet_vtx_px[maxNumJets];
    float m_jet_vtx_py[maxNumJets];
    float m_jet_vtx_pz[maxNumJets];
    float m_jet_vtx_x[maxNumJets];
    float m_jet_vtx_y[maxNumJets];
    float m_jet_vtx_z[maxNumJets];
    float m_jet_vtxCat_IVF[maxNumJets];
    float m_jet_vtxMassCorr_IVF[maxNumJets];
    float m_jet_msv_new[maxNumJets];
    float m_jet_msv_inc[maxNumJets];
    int   m_npv_array ;
    float m_pv_x[maxNumJets];
    float m_pv_y[maxNumJets];
    float m_pv_z[maxNumJets];
    float m_ht;
    float m_mht;
    float m_mht_phi;
    float m_wt_diEle;
    float m_wt_diMuon;
    int   m_trig_dimuon1;
    int   m_trig_dimuon2;
    int   m_trig_dimuon3;
    int   m_trig_dimuon4;
    int   m_trig_dielec1;
    float m_nPVs;
    float m_genWeight;
    float m_puWeight;
    unsigned int m_run;
    unsigned int m_lumi;
    float m_lheNj;
    std::vector<int> m_muon_trig;
    std::vector<int> m_elec_trig;

  // 2017-03-30 - Added for lepton combination.
  int   m_nvLeps;
  float m_vlep_pt    [maxNumElecs];
  float m_vlep_eta   [maxNumElecs];
  float m_vlep_phi   [maxNumElecs];
  int   m_vlep_charge[maxNumElecs];
  float m_vlep_iso03 [maxNumElecs];
  float m_vlep_iso04 [maxNumElecs];
  int   m_vlep_pdgId [maxNumElecs];
  int   m_nselLeps;
  float m_sellep_pt    [maxNumElecs];
  float m_sellep_eta   [maxNumElecs];
  float m_sellep_phi   [maxNumElecs];
  int   m_sellep_charge[maxNumElecs];
  float m_sellep_iso03 [maxNumElecs];
  float m_sellep_iso04 [maxNumElecs];
  int   m_sellep_pdgId [maxNumElecs];
  float m_sellep_m     [maxNumElecs];
  int   m_sellep_eleMVAIdSppring16GenPurp [maxNumElecs];
  float m_old_Z_mass;
  float m_old_Z_pt  ;
  float m_old_Z_eta ;
  float m_old_Z_phi ;
  float m_old_Vtype ;

// Calculated variables
    float Z_DelR, Z_DelPhi, Z_DelEta;
    // std::array<float, maxNumJets> jet_msv_quickCorr;
    float jet_msv_quickCorr[maxNumJets];
    float evtWeight;
    // std::map< TString, std::map<TString, float> > jetTagEvtWeight;    // Additional event weight calculated for each hf/sv operating point.
    std::map< TString, std::map<TString, std::map<TString, float> > > jetTagEvtWeight;   // jetTagEvtWeight[HFType][SVType][UncertaintyVariation]
      // Additional event weight calculated for each hf/sv operating point, with variations to various uncertainties

  // Selection Variables
    bool usingSim; // Simulation events. For plotting sim-truth information.
    bool usingDY ;
    bool noTrigOnMC;

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
    bool usingOddEventDY    ;
    bool hasVtypeFix        ;
    bool hasFixedVtypeCharge;

  // Lepton Selection Variables
    std::vector<Index> validLeptons, validMuons, validElectrons;    // List of the indexes of leptons, eventually ordered by pt.
    // std::vector<Index> validLeptons;    // List of the indexes of muon objects, eventually ordered by pt.

  // Jet Selection Variables
    std::vector<Index> validJets;       // lists all "valid" jets from standard cuts.
    // subsequent vectors contain true/false conditions of jets as ordered in above validjets vector.
    // HF/SVT-tagged jets,
    std::map<TString, std::map<TString, std::vector<bool> > > HFJets   ;
    std::map<TString, std::map<TString,              bool > > hasHFJets;
    std::map<TString, std::map<TString,             Index > > leadHFJet;
    // MC truth information
    std::vector<bool> bMCJets;
    std::vector<bool> cMCJets;
    std::vector<bool> lMCJets;
    Index leadBJet, leadCJet;

  // Variable pointers for checking HFTags and SVMasses.
    std::map<TString, float*> SVVariable;
    std::map<TString, float > SVMinimumVal;
    std::map<TString, float*> HFTagDiscrimVar;
    std::map<TString, float > HFTagDiscrimOP ;

  // Extra data
    long patEventsAnalyzed;     // Number of events that were processed to make the Ntuple.
    long entriesInNtuple  ;     // Number of events that were processed to make the Ntuple.
    // Need to figure out how to do within this scope... might need to handle externally

};

#endif
