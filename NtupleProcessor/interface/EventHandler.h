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

  // Methods
    bool mapTree(TTree*);                         // Maps class variables to an input TTree.
    void evalCriteria();                          // Evaluates the class' list of event selection criteria
    bool triggered(std::vector<int>&);            // Returns if this event triggered one of the listed triggers.
    void resetSelectionVariables();               // Resets all selection variables to false.
    void printJets();                             // Test function that prints jets and their properties.
    float calculatePUReweight(int);
    float calculateJetMSVQuickCorrection(int);
    float calculateJetTagEvtWeight(std::string, std::string, bool debug=false);

  // Running Variables
    TString options;         // Options input with TreeIterator.

// Variables for mapping tree entries. Public so they can be referred to by outside classes.
    float m_Z_mass;     int   m_nJets;                 int   m_nLeps;                     float m_MET_et      ;
    float m_Z_pt  ;     float m_jet_pt [maxNumJets];   float m_lep_pt    [maxNumElecs];   float m_MET_phi     ;
    float m_Z_eta ;     float m_jet_eta[maxNumJets];   float m_lep_eta   [maxNumElecs];   float m_MET_sumet   ;
    float m_Z_phi ;     float m_jet_phi[maxNumJets];   float m_lep_phi   [maxNumElecs];   float m_MET_sig     ;
    float m_Vtype ;     float m_jet_csv[maxNumJets];   int   m_lep_charge[maxNumElecs];   bool  m_triggers[54];
    int   m_zdecayMode; float m_jet_msv[maxNumJets];   float m_lep_iso   [maxNumElecs];   float m_json        ;
                        int   m_jet_flv[maxNumJets];                                      int   m_event       ;
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

// Calculated variables
    float Z_DelR, Z_DelPhi, Z_DelEta;
    // std::array<float, maxNumJets> jet_msv_quickCorr;
    float jet_msv_quickCorr[maxNumJets];
    float evtWeight;
    std::map< TString, std::map<TString, float> > jetTagEvtWeight;    // Additional event weight calculated for each hf/sv operating point.

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

  // Lepton Selection Variables
//    std::vector<Index> validLeptons, validMuons, validElectrons;    // List of the indexes of muon objects, eventually ordered by pt.
    std::vector<Index> validLeptons;    // List of the indexes of muon objects, eventually ordered by pt.

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

  // Variables for the event reweighting.
    const double mc2[52] =
    { 4.8551E-07,
      1.74806E-06,
      3.30868E-06,
      1.62972E-05,
      4.95667E-05,
      0.000606966,
      0.003307249,
      0.010340741,      0.022852296,      0.041948781,      0.058609363,      0.067475755,      0.072817826,      0.075931405,      0.076782504,      0.076202319,      0.074502547,      0.072355135,      0.069642102,      0.064920999,      0.05725576,      0.047289348,      0.036528446,      0.026376131,      0.017806872,      0.011249422,      0.006643385,      0.003662904,      0.001899681,      0.00095614,      0.00050028,      0.000297353,      0.000208717,      0.000165856,      0.000139974,      0.000120481,      0.000103826,      8.88868E-05,      7.53323E-05,      6.30863E-05,      5.21356E-05,      4.24754E-05,      3.40876E-05,      2.69282E-05,      2.09267E-05,      1.5989E-05,      4.8551E-06,      2.42755E-06,      4.8551E-07,      2.42755E-07,      1.21378E-07,      4.8551E-08    };    const double data2[52] =
    {      4.7027e-05,      0.000281565,      0.00028437,      0.00038727,      0.000569421,      0.000952123,      0.00319069,      0.0203182,      0.0699736,      0.130068,      0.180077,      0.198876,      0.174006,      0.118772,      0.06317,      0.026531,      0.00902068,      0.00258006,      0.000659883,      0.000164919,      4.46606e-05,      1.44451e-05,      5.83791e-06,      2.78026e-06,      1.40517e-06,      7.0225e-07,      3.36679e-07,      1.53294e-07,      6.60997e-08,      2.69735e-08,      1.04154e-08,      3.80539e-09,      1.31553e-09,      4.30311e-10,      1.3318e-10,      3.90006e-11,      1.08063e-11,      2.83309e-12,      7.02782e-13,      1.64952e-13,      3.66335e-14,      7.69806e-15,      1.53064e-15,      2.87972e-16,      5.12673e-17,      8.63513e-18,      1.37688e-18,      2.04286e-19,      3.72485e-20,      0,      0,      0    };    const double data2_P[52] =
    {      4.02952e-05,      0.000254497,      0.000280989,      0.000335717,      0.00050839,      0.000746426,      0.00186153,      0.0100804,      0.0445114,      0.0982319,      0.150143,      0.184585,      0.184905,      0.148911,      0.0954775,      0.0489179,      0.020324,      0.00701388,      0.00208482,      0.000564834,      0.000150948,      4.35228e-05,      1.47302e-05,      6.1078e-06,      2.96971e-06,      1.54517e-06,      8.03286e-07,      4.03918e-07,      1.94127e-07,      8.88486e-08,      3.86863e-08,      1.60215e-08,      6.31045e-09,      2.36387e-09,      8.42156e-10,      2.8534e-10,      9.19462e-11,      2.81777e-11,      8.21254e-12,      2.27642e-12,      6.00106e-13,      1.50456e-13,      3.58753e-14,      8.13565e-15,      1.75469e-15,      3.59936e-16,      7.02215e-17,      1.30278e-17,      2.29864e-18,      3.87514e-19,      7.22359e-20,      7.82805e-22    };

    const double data2_M[52] =
    {
      5.48234e-05,
      0.00031101,
      0.000294382,
      0.000447642,
      0.000648535,
      0.00130872,
      0.00642627,
      0.0386726,
      0.102155,
      0.165772,
      0.205682,
      0.198571,
      0.146521,
      0.0819434,
      0.0350992,
      0.0118095,
      0.00325042,
      0.000781694,
      0.000181477,
      4.58499e-05,
      1.41184e-05,
      5.55293e-06,
      2.58233e-06,
      1.26088e-06,
      6.01759e-07,
      2.73079e-07,
      1.16867e-07,
      4.70691e-08,
      1.78332e-08,
      6.35519e-09,
      2.13024e-09,
      6.71619e-10,
      1.99164e-10,
      5.55507e-11,
      1.45733e-11,
      3.59601e-12,
      8.34591e-13,
      1.82189e-13,
      3.74081e-14,
      7.22453e-15,
      1.31237e-15,
      2.24241e-16,
      3.60383e-17,
      5.44906e-18,
      7.76605e-19,
      1.04021e-19,
      1.24685e-20,
      0,
      0,
      0,
      0,
      0
    };
};

#endif
