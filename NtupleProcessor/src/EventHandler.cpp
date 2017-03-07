/*
 * EventHandler.cpp
 *
 *  Created on: May 14, 2015
 *      Author: godshalk
 */

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <TBranch.h>
#include <TLeaf.h>
#include <TMath.h>
#include "TVector3.h"
#include "../interface/EventHandler.h"

using std::cout;   using std::endl;   using std::vector;   using std::swap;
using std::setw;   using std::setprecision;
using std::sqrt;   using std::string;

const vector<TString> EventHandler::HFTags = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS"};
// const vector<TString> EventHandler::SVType = {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
const vector<TString> EventHandler::SVType = {"noSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};

EventHandler::EventHandler(TString fnac, TString o) : anCfg(fnac), options(o)
{
  // Check the option to see if we're working with Simulation or Data
    usingSim           = (options.Contains("Sim"        , TString::kIgnoreCase) ? true : false);
    usingDY            = (options.Contains("DY"         , TString::kIgnoreCase) ? true : false);
    noTrigOnMC         = (options.Contains("NoTrigOnMC" , TString::kIgnoreCase) ? true : false);
    usingTopHalfDY     = (options.Contains("TOPHALF"    , TString::kIgnoreCase) ? true : false);
    usingBottomHalfDY  = (options.Contains("BOTTOMHALF" , TString::kIgnoreCase) ? true : false);
    usingEvenEventDY   = (options.Contains("EVEN"       , TString::kIgnoreCase) ? true : false);
    usingOddEventDY    = (options.Contains("ODD"        , TString::kIgnoreCase) ? true : false);

    patEventsAnalyzed = 0;
    entriesInNtuple   = 0;

    for(int i=0; i<maxNumJets; i++) jet_msv_quickCorr[i] = -10.0;
    // jet_msv_quickCorr.fill(-1.0);
    evtWeight = 1.0;

  // Set up trigger map objects to the same size as the list of triggers specified for selection in the analysis config file.
    m_muon_trig = vector<int>(anCfg.muonTriggers.size(), 0);
    m_elec_trig = vector<int>(anCfg.elecTriggers.size(), 0);

  // Hard code in some pointers for HF/SV usage, because who has time for config files?
    HFTagDiscrimVar["NoHF"] = m_jet_csv;
    HFTagDiscrimVar["CSVL"] = m_jet_csv;
    HFTagDiscrimVar["CSVM"] = m_jet_csv;
    HFTagDiscrimVar["CSVT"] = m_jet_csv;
    HFTagDiscrimVar["CSVS"] = m_jet_csv;
    HFTagDiscrimOP ["NoHF"] = anCfg.stdCSVOpPts["NoHF"];
    HFTagDiscrimOP ["CSVL"] = anCfg.stdCSVOpPts["CSVL"];
    HFTagDiscrimOP ["CSVM"] = anCfg.stdCSVOpPts["CSVM"];
    HFTagDiscrimOP ["CSVT"] = anCfg.stdCSVOpPts["CSVT"];
    HFTagDiscrimOP ["CSVS"] = anCfg.stdCSVOpPts["CSVS"];

    SVVariable["noSV" ] = m_jet_msv             ;
    SVVariable["oldSV"] = m_jet_msv             ;
    SVVariable["pfSV" ] = m_jet_msv_new         ;
    SVVariable["pfISV"] = m_jet_msv_inc         ;
    SVVariable["qcSV" ] = jet_msv_quickCorr     ;
    SVVariable["cISV" ] = m_jet_vtxMassCorr_IVF ;
    SVVariable["cISVf"] = m_jet_vtxMassCorr_IVF ;
    SVVariable["cISVp"] = m_jet_vtxMassCorr_IVF ;
    SVMinimumVal["noSV" ] = anCfg. noSVT ;
    SVMinimumVal["oldSV"] = anCfg.minSVT ;
    SVMinimumVal["pfSV" ] = anCfg.minSVT ;
    SVMinimumVal["pfISV"] = anCfg.minSVT ;
    SVMinimumVal["qcSV" ] = anCfg.minSVT ;
    SVMinimumVal["cISV" ] = anCfg.minSVT ;
    SVMinimumVal["cISVf"] = anCfg.minSVT ;
    SVMinimumVal["cISVp"] = anCfg.minSVT ;

}

bool EventHandler::mapTree(TTree* tree)
{
  // Maps TTree to class' variables.
  // TO DO: Implement check for correct mapping, return result?
  //    - Set up exception handling for negative result.

    entriesInNtuple = tree->GetEntries();

    TBranch * temp_branch;  // Temporary branch to get at members of struct-branches.

  // Deactivate all branches, reactivate as necessary.
    tree->SetBranchStatus("*",0);
    vector<TString> branches_to_reactivate = {
      //  "Vtype"      , "nallMuons"         , "nallElectrons"     , "nallJets"          ,
      //  "V*"         , "allMuon_pt"        , "allElectron_pt"    , "allJet_pt"         ,
      //  "zdecayMode" , "allMuon_eta"       , "allElectron_eta"   , "allJet_eta"        ,
      //  "EVENT*"     , "allMuon_phi"       , "allElectron_phi"   , "allJet_phi"        ,
        // "MET*"      , "allMuon_charge"    , "allElectron_charge", "allJet_csv"        ,
                      // "allMuon_pfCorrIso" ,                       "allJet_vtxMass"    ,
      //  "triggerFlags",                                            "allJet_flavour"    ,
      //  "weightTrig2012DiEle",
      //  "weightTrig2012DiMuon"
        "Vtype" , "nvLeptons"          , "nJet"       , "met_pt"   ,
        "V_mass", "vLeptons_pt"        , "Jet_pt"     , "met_phi"  ,
        "V_pt"  , "vLeptons_eta"       , "Jet_eta"    , "met_sumEt",
        "V_eta" , "vLeptons_phi"       , "Jet_phi"    ,
        "V_phi" , "vLeptons_charge"    , "Jet_btagCSV",
        "json"  , "vLeptons_pfRelIso04", "Jet_vtxMass",
        "Jet_vtxPx",
        "Jet_vtxPy",
        "Jet_vtxPz",
        "Jet_vtxPosX",
        "Jet_vtxPosY",
        "Jet_vtxPosZ",
        "Jet_vtxCat_IVF",
        "Jet_vtxMassCorr_IVF",
        "Jet_newVtxMass",
        "Jet_incVtxMass",
        "evt"        ,
        "htJet30"    ,
        "mhtJet30"   ,
        "mhtPhiJet30",
        "nprimaryVertices" ,
        "primaryVertices_x",
        "primaryVertices_y",
        "primaryVertices_z",
        //"HLT_BIT_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v",
        //"HLT_BIT_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v",
        //"HLT_BIT_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v",
        //"HLT_BIT_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v",
        //"HLT_BIT_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
        "nPVs",
        "run", "lumi",
        //"lheNj"
    };
    if(usingSim)
    {
        branches_to_reactivate.push_back("puWeight"         );
        branches_to_reactivate.push_back("genWeight"        );
        branches_to_reactivate.push_back("Jet_mcFlavour"    );
        branches_to_reactivate.push_back("Jet_partonFlavour");
        branches_to_reactivate.push_back("Jet_hadronFlavour");
        branches_to_reactivate.push_back("lheNj"            );
    }

  // Reactivate branches specified above, as well as branches for triggers named in the analysis config.
    for(TString br : branches_to_reactivate) tree->SetBranchStatus(br.Data(), 1);
    for(TString br : anCfg.muonTriggers    ) tree->SetBranchStatus(br.Data(), 1);
    for(TString br : anCfg.elecTriggers    ) tree->SetBranchStatus(br.Data(), 1);

  // Z variables
    m_zdecayMode = 0;
  if(tree->GetListOfBranches()->FindObject("zdecayMode"))
    tree->SetBranchAddress("zdecayMode",      &m_zdecayMode  );
    tree->SetBranchAddress("Vtype"     ,          &m_Vtype       );
  //  temp_branch = tree->GetBranch("V");
  //  temp_branch->GetLeaf( "mass" )->SetAddress(   &m_Z_mass      );
  //  temp_branch->GetLeaf( "pt"   )->SetAddress(   &m_Z_pt        );
  //  temp_branch->GetLeaf( "eta"  )->SetAddress(   &m_Z_eta       );
  //  temp_branch->GetLeaf( "phi"  )->SetAddress(   &m_Z_phi       );
    tree->SetBranchAddress( "V_mass", &m_Z_mass      );
    tree->SetBranchAddress( "V_pt"  , &m_Z_pt        );
    tree->SetBranchAddress( "V_eta" , &m_Z_eta       );
    tree->SetBranchAddress( "V_phi" , &m_Z_phi       );

  // JSON
  //  temp_branch = tree->GetBranch("EVENT");
  //  temp_branch->GetLeaf( "json"  )->SetAddress(   &m_json        );
  //  temp_branch->GetLeaf( "event" )->SetAddress(   &m_event       );
    tree->SetBranchAddress( "json", &m_json );
    tree->SetBranchAddress( "evt" , &m_event);
    tree->SetBranchAddress( "nPVs", &m_nPVs );
    tree->SetBranchAddress( "run" , &m_run  );
    tree->SetBranchAddress( "lumi", &m_lumi );


  // Muon variables
  //  tree->SetBranchAddress( "nallMuons"         , &m_nMuons      );
  //  tree->SetBranchAddress( "allMuon_pt"        ,  m_muon_pt     );
  //  tree->SetBranchAddress( "allMuon_eta"       ,  m_muon_eta    );
  //  tree->SetBranchAddress( "allMuon_phi"       ,  m_muon_phi    );
  //  tree->SetBranchAddress( "allMuon_charge"    ,  m_muon_charge );
  //  tree->SetBranchAddress( "allMuon_pfCorrIso" ,  m_muon_iso    );
    tree->SetBranchAddress( "nvLeptons"           , &m_nLeps      );
    tree->SetBranchAddress( "vLeptons_pt"         ,  m_lep_pt     );
    tree->SetBranchAddress( "vLeptons_eta"        ,  m_lep_eta    );
    tree->SetBranchAddress( "vLeptons_phi"        ,  m_lep_phi    );
    tree->SetBranchAddress( "vLeptons_charge"     ,  m_lep_charge );
    tree->SetBranchAddress( "vLeptons_pfRelIso04" ,  m_lep_iso    );

  // Muon variables
  //  tree->SetBranchAddress( "nallElectrons"        , &m_nElecs      );
  //  tree->SetBranchAddress( "allElectron_pt"       ,  m_elec_pt     );
  //  tree->SetBranchAddress( "allElectron_eta"      ,  m_elec_eta    );
  //  tree->SetBranchAddress( "allElectron_phi"      ,  m_elec_phi    );
  //  tree->SetBranchAddress( "allElectron_charge"   ,  m_elec_charge );
  //  tree->SetBranchAddress( "allElectron_pfCorrIso",  m_elec_iso    );

  // Jet variables
  //  tree->SetBranchAddress( "nallJets"          , &m_nJets       );
  //  tree->SetBranchAddress( "allJet_pt"         ,  m_jet_pt      );
  //  tree->SetBranchAddress( "allJet_eta"        ,  m_jet_eta     );
  //  tree->SetBranchAddress( "allJet_phi"        ,  m_jet_phi     );
  //  tree->SetBranchAddress( "allJet_csv"        ,  m_jet_csv     );
  //  tree->SetBranchAddress( "allJet_vtxMass"    ,  m_jet_msv     );
  //  tree->SetBranchAddress( "allJet_flavour"    ,  m_jet_flv     );
    tree->SetBranchAddress( "nJet"           , &m_nJets       );
    tree->SetBranchAddress( "Jet_pt"         ,  m_jet_pt      );
    tree->SetBranchAddress( "Jet_eta"        ,  m_jet_eta     );
    tree->SetBranchAddress( "Jet_phi"        ,  m_jet_phi     );
    tree->SetBranchAddress( "Jet_btagCSV"    ,  m_jet_csv     );
    tree->SetBranchAddress( "Jet_vtxMass"    ,  m_jet_msv     );
    tree->SetBranchAddress( "Jet_vtxPx"  , m_jet_vtx_px );
    tree->SetBranchAddress( "Jet_vtxPy"  , m_jet_vtx_py );
    tree->SetBranchAddress( "Jet_vtxPz"  , m_jet_vtx_pz );
    tree->SetBranchAddress( "Jet_vtxPosX", m_jet_vtx_x  );
    tree->SetBranchAddress( "Jet_vtxPosY", m_jet_vtx_y  );
    tree->SetBranchAddress( "Jet_vtxPosZ", m_jet_vtx_z  );
    tree->SetBranchAddress( "Jet_vtxCat_IVF"     , m_jet_vtxCat_IVF     );
    tree->SetBranchAddress( "Jet_vtxMassCorr_IVF", m_jet_vtxMassCorr_IVF);
    tree->SetBranchAddress( "Jet_newVtxMass"     , m_jet_msv_new        );
    tree->SetBranchAddress( "Jet_incVtxMass"     , m_jet_msv_inc        );
    tree->SetBranchAddress("nprimaryVertices" , &m_npv_array );
    tree->SetBranchAddress("primaryVertices_x",  m_pv_x      );
    tree->SetBranchAddress("primaryVertices_y",  m_pv_y      );
    tree->SetBranchAddress("primaryVertices_z",  m_pv_z      );
  if(usingSim)
  { tree->SetBranchAddress( "Jet_mcFlavour"    , m_jet_flv    );
    tree->SetBranchAddress( "Jet_hadronFlavour", m_jet_hadflv );
    tree->SetBranchAddress( "Jet_partonFlavour", m_jet_parflv );
    tree->SetBranchAddress( "lheNj"          , &m_lheNj       );
    tree->SetBranchAddress( "genWeight"      , &m_genWeight   );
    tree->SetBranchAddress( "puWeight"       , &m_puWeight    );
  }

  // MET variables
  //  temp_branch = tree->GetBranch("MET");
  //  temp_branch->GetLeaf( "et"   )->SetAddress( &m_MET_et    );
  //  temp_branch->GetLeaf( "phi"  )->SetAddress( &m_MET_phi   );
  //  temp_branch->GetLeaf( "sumet")->SetAddress( &m_MET_sumet );
  //  temp_branch->GetLeaf( "sig"  )->SetAddress( &m_MET_sig   );
  //  temp_branch = tree->GetBranch("MET");
    tree->SetBranchAddress( "met_pt"     , &m_MET_et    );
    tree->SetBranchAddress( "met_phi"    , &m_MET_phi   );
    tree->SetBranchAddress( "met_sumEt"  , &m_MET_sumet );
    tree->SetBranchAddress( "htJet30"    , &m_ht        );
    tree->SetBranchAddress( "mhtJet30"   , &m_mht       );
    tree->SetBranchAddress( "mhtPhiJet30", &m_mht_phi   );

  //  temp_branch->GetLeaf( "met_rawPt"  )->SetAddress( &m_MET_sig   );

  // Trigger variables
    //tree->SetBranchAddress( "triggerFlags",         m_triggers   );
    //tree->SetBranchAddress( "weightTrig2012DiEle" , &m_wt_diEle  );
    //tree->SetBranchAddress( "weightTrig2012DiMuon", &m_wt_diMuon );
    //tree->SetBranchAddress("HLT_BIT_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v"         , &m_trig_dimuon3);
    //tree->SetBranchAddress("HLT_BIT_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v"       , &m_trig_dimuon4);
    //tree->SetBranchAddress("HLT_BIT_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v"      , &m_trig_dimuon1);
    //tree->SetBranchAddress("HLT_BIT_HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v"    , &m_trig_dimuon2);
    //tree->SetBranchAddress("HLT_BIT_HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v", &m_trig_dielec1);
  // Set up trigger vector to be mapped to trigger variables specified in AnalysisCfg.
    for(int i=0; i<m_muon_trig.size(); i++) tree->SetBranchAddress(anCfg.muonTriggers[i].c_str(), &m_muon_trig[i]);
    for(int i=0; i<m_elec_trig.size(); i++) tree->SetBranchAddress(anCfg.elecTriggers[i].c_str(), &m_elec_trig[i]);
    return true;
}


void EventHandler::evalCriteria()
{ // Evaluates the class' list of event selection criteria

  // cout << "   EventHandler::evalCriteria(): BEGIN." << endl;
  // cout << "    EventHandler::evalCriteria(): TEST: EVENT NUMBER = " << m_event << endl;

    resetSelectionVariables();

  ////////////////////////////////////////////////
  // 2016-03-15 - SPECIAL TEMP FITTING CHECK
    if(usingTopHalfDY    && m_event <= 30000000) return;
    if(usingBottomHalfDY && m_event >  30000000) return;
    if(usingEvenEventDY  && m_event%2 == 1     ) return;
    if(usingOddEventDY   && m_event%2 == 0     ) return;
  ////////////////////////////////////////////////

  // Apply genWeight sign (for amc#NLO)
    if(usingSim)
        if(m_genWeight < 0) evtWeight*=-1;

  // Apply puWeight on Sim
    if(usingSim)
        evtWeight*=m_puWeight;

  // Check JSON if working with a data event.
    if(usingSim) inJSON = false;       // If using simulation, automatically set the JSON variable to false.
    else if(anCfg.jsonSelect)          // if selecting from JSON...
    {   if(anCfg.lumiJSON.isValid())   //   If JSON provided in Analysis Config...
            inJSON = anCfg.lumiJSON.isInJSON(m_run, m_lumi);  // Check to see if the run and lumi are in the "good" JSON sections.
        else inJSON = m_json==1;       // Use the ntuple value
    }
    else inJSON = true;                // Set to true if not selecting from JSON

  // Check if event has the required triggers. Kick if not triggered.
    isElTriggered = isMuTriggered = false;
    for( const int& trig : m_muon_trig ) if(trig != 0) {isMuTriggered = true; break;}
    for( const int& trig : m_elec_trig ) if(trig != 0) {isElTriggered = true; break;}

  // If there are no triggers specified, take all triggers.
    if(m_muon_trig.size() == 0) isMuTriggered = true;
    if(m_elec_trig.size() == 0) isElTriggered = true;

    //for( int i=0; i<m_muon_trig.size(); i++) cout << "  " << m_muon_trig[i] << "  " << anCfg.muonTriggers[i] << endl;
    //for( int i=0; i<m_elec_trig.size(); i++) cout << "  " << m_elec_trig[i] << "  " << anCfg.elecTriggers[i] << endl;

    if(usingSim && noTrigOnMC) isMuTriggered = isElTriggered = true;
    if(!isElTriggered && !isMuTriggered) return;

  // Check for the proper number or leptons. Kick if neither.
    if( m_nLeps<2 ) return;

  // Lepton selection
    // **************PROBLEM AREA*********************
    // Not perfect... takes leading leptons. Makes the isolation and kinematic checks. Constructs Z.
    // Doesn't take into account that a lower-pt lepton might be a Z-daughter.
    // Need to go back and rework how this information is saved. May be too general.
    // May want to add some control plot information to the Ntupler/PATTupler.
    // For now, just finds leptons without intermediate steps.
    // Also need to implement trigger matching (or see if implemented in previous steps).

  // Perform selection on leptons and store indexes sorted by pt.
    if(m_Vtype==0)
    {
        for(Index i=0; i<m_nLeps; i++)
        {
            //cout << "    EventHandler::evalCriteria(): Muon Props (muon #, pt, eta, iso): = (" << i << ", " << m_muon_pt[i] << ", " << m_muon_eta[i] << ", " << m_muon_iso[i] << ")" << endl;
          // Perform selection on this muon. Skip to next if it doesn't meet criteria.
            if(         m_lep_pt [i] <anCfg.muonPtMin
                || fabs(m_lep_eta[i])>anCfg.muonEtaMax
                ||      m_lep_iso[i] >anCfg.muonIsoMax
              ) continue;
          // Insert muon in list based on pt.
            Index lowPtIndex = i;
            for(int j=0; j<validLeptons.size(); j++) if(m_lep_pt[validLeptons[j]]<m_lep_pt[lowPtIndex]) swap(validLeptons[j], lowPtIndex);
            validLeptons.push_back(lowPtIndex);
        }
        //if(validMuons.size()>=2) cout << "    --> TWO VALID MUONS with vType == " << m_Vtype << ", charges of " << m_muon_charge[0] << "," << m_muon_charge[1] << endl;
    }
    else if(m_Vtype==1)
    {
        for(Index i=0; i<m_nLeps; i++)
        {
    //cout << "    EventHandler::evalCriteria(): ELEC Props (elec #, pt, eta, iso): = (" << i << ", " << m_lep_pt[i] << ", " << m_lep_eta[i] << ", " << m_lep_iso[i] << ")" << endl;
          // Perform selection on this electron.
            if(    m_lep_pt [i] < anCfg.elecPtMin
                || (fabs(m_lep_eta[i])>anCfg.elecEtaInnerMax && (fabs(m_lep_eta[i])<anCfg.elecEtaOuterMin || fabs(m_lep_eta[i])>anCfg.elecEtaOuterMax))
                //|| m_lep_iso[i] >anCfg.elecIsoMax
              ) continue;
          // Insert electron in list based on pt.
            Index lowPtIndex = i;
            for(int j=0; j<validLeptons.size(); j++) if(m_lep_pt[validLeptons[j]]<m_lep_pt[lowPtIndex]) swap(validLeptons[j], lowPtIndex);
            validLeptons.push_back(lowPtIndex);
        }
    }


  // Check lepton id, isolation (TO IMPLEMENT. ALREADY DONE IN NTUPLER)
  //

  // Check for two valid muons, electrons. (Assume 0, 1 are leading and subleading.)
  //  hasValidMuons =    validMuons.size() >=2   //  && m_Vtype==0
                  //  && (m_muon_charge[validMuons[0]]*m_muon_charge[validMuons[1]]==-1 || !anCfg.dilepMuonReqOppSign )
                  //  && isMuTriggered
                  //  && m_Vtype==0
  //  ;
  //  hasValidElectrons =    validElectrons.size()>=2 //&& m_Vtype==1
                      //  && (m_elec_charge[validElectrons[0]]*m_elec_charge[validElectrons[1]]==-1 || !anCfg.dilepElecReqOppSign )
                      //  && isElTriggered
  //  ;
    hasValidMuons =    m_Vtype==0
                    && isMuTriggered
                    && validLeptons.size() >=2
                    && (m_lep_charge[validLeptons[0]]*m_lep_charge[validLeptons[1]]==-1 || !anCfg.dilepMuonReqOppSign )
    ;
    hasValidElectrons =    m_Vtype==1
                        && isElTriggered
                        && validLeptons.size() >=2
                        && (m_lep_charge[validLeptons[0]]*m_lep_charge[validLeptons[1]]==-1 || !anCfg.dilepElecReqOppSign )
    ;

  // Calculate Z_DelR based on valid muons/electrons.
    Z_DelR = Z_DelPhi = Z_DelEta = -1;
    if(hasValidMuons || hasValidElectrons)
    {
        Z_DelEta = fabs(m_lep_eta[validLeptons[0]]-m_lep_eta[validLeptons[1]]);
        Z_DelPhi = fabs(m_lep_phi[validLeptons[0]]-m_lep_phi[validLeptons[1]]);
        if(Z_DelPhi > TMath::Pi()) Z_DelPhi = 2.0*TMath::Pi() - Z_DelPhi;
        Z_DelR   = sqrt(Z_DelEta*Z_DelEta+Z_DelPhi*Z_DelPhi);
    }

  // Apply lepton SFs
    if(usingSim)
    {
        string lType="";
        if( hasValidElectrons) lType="elec";
        if( hasValidMuons    ) lType="muon";
        if( lType!="" )
        {
            evtWeight *= anCfg.lepSFs[lType+"_sf_trk" ].getSF(m_lep_pt[validLeptons[0]],m_lep_eta[validLeptons[0]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_trk" ].getSF(m_lep_pt[validLeptons[1]],m_lep_eta[validLeptons[1]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_id"  ].getSF(m_lep_pt[validLeptons[0]],m_lep_eta[validLeptons[0]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_id"  ].getSF(m_lep_pt[validLeptons[1]],m_lep_eta[validLeptons[1]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_iso" ].getSF(m_lep_pt[validLeptons[0]],m_lep_eta[validLeptons[0]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_iso" ].getSF(m_lep_pt[validLeptons[1]],m_lep_eta[validLeptons[1]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_trig"].getSF(m_lep_pt[validLeptons[0]],m_lep_eta[validLeptons[0]]).first;
            evtWeight *= anCfg.lepSFs[lType+"_sf_trig"].getSF(m_lep_pt[validLeptons[1]],m_lep_eta[validLeptons[1]]).first;
            //cout << "     SF TEST!! " << lType << ", " << evtWeight << endl;
        }

    }

  // ADDED 2016-11-10 - MEANT FOR COMBINATION OF DY AND DY1J EVENTS.
    //if(usingSim && usingDY && m_lheNj == 1) evtWeight *= 0.028391905220026;

    hasValidZBosonMass = m_Z_mass>=anCfg.dilepInvMassMin && m_Z_mass<=anCfg.dilepInvMassMax;
    hasValidMET        = m_MET_et<=anCfg.metMax;

  // Run through list of jets to find valid jets, then check those jets for various features.
    validJets.clear();
    for(Index i=0; i<m_nJets; i++)
    {
      // Perform selection on this Jet. Skip to next if it doesn't meet criteria.
        if(m_jet_pt[i]<anCfg.jetPtMin || fabs(m_jet_eta[i])>anCfg.jetEtaMax) continue;
      // Insert jet in list based on pt.
        Index lowPtIndex = i;
        for(Index j=0; j<validJets.size(); j++)
        {
            //cout << "    evalCriteria(): Checking jet at index " << lowPtIndex << " against " << validJets[j] << endl;
            if(m_jet_pt[validJets[j]]<m_jet_pt[lowPtIndex]) swap(validJets[j], lowPtIndex);
        }
        validJets.push_back(lowPtIndex);
    }

    for(auto& svType : SVType)
        for(auto& hfTag : HFTags)
            {   HFJets   [hfTag][svType] = vector<bool>(validJets.size(), false);
                hasHFJets[hfTag][svType] = false;
                leadHFJet[hfTag][svType] = -1;
            }

  // Check HF Tagging info for all valid jets and perform on-the-fly calculations
    //if(validJets.size() > 0) cout << "  Testing " << validJets.size() << " jets..." << endl;
    for(Index vJet_i=0, evt_i=0; vJet_i<validJets.size(); vJet_i++)
    {
      // For each jet, calculate corrected secondary vertex mass (calculate for valid jets only)
        jet_msv_quickCorr[evt_i] = calculateJetMSVQuickCorrection(evt_i);
        // if(jet_msv_quickCorr[evt_i] != 0) cout << "    TEST: Jet msv: " << m_jet_msv[evt_i] << "   Jet MSVQCorr: " << jet_msv_quickCorr[evt_i] << endl;
        // if(jet_msv_quickCorr[evt_i] != 0 && m_jet_msv[evt_i] == 0) cout << "    TEST: Jet msv: " << m_jet_msv[evt_i] << "   Jet MSVQCorr: " << jet_msv_quickCorr[evt_i] << endl;

      // Jet is HF if it passes the CSV operating point and has a reconstructed secondary vertex.
        evt_i = validJets[vJet_i];  // Set the evt_i to the validJet's index within the EventHandler.
        for(auto& svType : SVType)
            for(auto& hfTag : HFTags)
                if(HFTagDiscrimVar[hfTag][evt_i] >= HFTagDiscrimOP[hfTag] && SVVariable[svType][evt_i] >= SVMinimumVal[svType])
                { // Unfortunate hardcoded checking: check the vertex category of the corrected secondary vertices, if the svType is appropriate.
                  // cISVf = full SV reco'd (==0)
                  // cISVp = psuedo vertex (==1)
                    if(svType == "cISVf" && m_jet_vtxCat_IVF[evt_i] != 0) continue;
                    if(svType == "cISVp" && m_jet_vtxCat_IVF[evt_i] != 1) continue;
                  // Set HFSVtag Variables.
                    HFJets[hfTag][svType][vJet_i] = true;
                    if(!hasHFJets[hfTag][svType])
                    {   leadHFJet[hfTag][svType] = vJet_i;
                        hasHFJets[hfTag][svType] = true;
                    }
                }
    }

  // Combine a few of the checks into a couple of comprehensive variables.
    isZeeEvent = (usingSim || inJSON) && hasValidElectrons && hasValidZBosonMass;
    isZuuEvent = (usingSim || inJSON) && hasValidMuons     && hasValidZBosonMass;
    isZllEvent = isZeeEvent || isZuuEvent;
    isZpJEvent = isZllEvent && validJets.size()>0;

  // Kick function if not using Sim. Otherwise, check jets for flavor properties
    if(!usingSim) return;

    bMCJets = cMCJets = lMCJets = vector<bool>(validJets.size(), false);
    hasBJet = hasCJet = false;
    for(Index vJet_i=0, evt_i=0; vJet_i<validJets.size(); vJet_i++)
    {
        evt_i = validJets[vJet_i];  // Set the evt_i to the validJet's index within the EventHandler.
        if(fabs(m_jet_hadflv[evt_i])==5) { bMCJets[vJet_i]=true; if(!hasBJet) leadBJet=vJet_i; hasBJet = true; }
        if(fabs(m_jet_hadflv[evt_i])==4) { cMCJets[vJet_i]=true; if(!hasCJet) leadCJet=vJet_i; hasCJet = true; }
        lMCJets[vJet_i] = fabs(m_jet_hadflv[evt_i])!=5 && fabs(m_jet_hadflv[evt_i])!=4;
    }

  // For each flavor tag, calculate an event weight based on jet tagging efficiency and tagging data/mc scalefactors.
  // See link for method used: https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
    for(auto& hfTag : HFTags) for(auto& svType : SVType)
        if(hasHFJets[hfTag][svType]) jetTagEvtWeight[hfTag][svType] = calculateJetTagEvtWeight(hfTag.Data(), svType.Data());

  // Kick function if not using DY. Otherwise, check for origin from Z->tautau
    if(!usingDY) return;
    zBosonFromTaus = (m_zdecayMode==3);

    // if(isZpJEvent) printJets();

}

// Returns whether or not this event has any of the listed triggers.
bool EventHandler::triggered(vector<int> &triggersToTest)
{
    for(int i : triggersToTest) if(m_triggers[i]) return true;
    return false;
}


void EventHandler::resetSelectionVariables()
{
    inJSON = isElTriggered = isMuTriggered = hasValidElectrons = hasValidMuons
      = hasValidZBosonMass = zBosonFromTaus = hasValidMET
      = isZpJEvent = isZHFEvent = hasBJet = hasCJet = false;
    leadBJet = leadCJet = 200;
  //  validMuons.clear();
  //  validElectrons.clear();
    validLeptons.clear();
    validJets.clear();
    evtWeight = 1.0;
    for(int i=0; i<maxNumJets; i++) jet_msv_quickCorr[i] = -10.0;
    for( auto& hftag : HFTags) for( auto& svtype : SVType) jetTagEvtWeight[hftag][svtype] = 1.0;
}


void EventHandler::printJets()
{
    if(!hasBJet && !hasCJet) return;
    cout << "------------------------------\n"
            "    Printing Jets..." << endl;
    for( auto& i : validJets ) cout << "   " << setw(4) << i << setprecision(4) << setw(8) << m_jet_pt[i] << setw(4) << m_jet_hadflv[i]
                                    << (m_jet_hadflv[i] == 4 || m_jet_hadflv[i] == -4 || m_jet_hadflv[i] == 5 || m_jet_hadflv[i] == -5 ? "    <------" : "") << "\n";
    if(usingDY)
    {
        if(hasBJet) cout << "    Leading BJet = " << validJets[leadBJet] << endl;
        if(hasCJet) cout << "    Leading CJet = " << validJets[leadCJet] << endl;
    }
    cout << endl;
}


float EventHandler::calculatePUReweight(int i)  // Taking an input of the number of primary vertices (PV) in an event, calculated the reweighting factor for that event.
{
    if(i<0 || i>51) return 1;
    return data2[i]/mc2[i];
    // Use modded up/down arrays for up/down error calculation.
}

float EventHandler::calculateJetMSVQuickCorrection(int jet_i)
{
  // Set up vector variables
    TVector3 priVtxPos(m_pv_x[0], m_pv_y[0], m_pv_z[0]);
    TVector3 secVtxPos(m_jet_vtx_x[jet_i] , m_jet_vtx_y[jet_i] , m_jet_vtx_z[jet_i] );
    TVector3 secVtxMom(m_jet_vtx_px[jet_i], m_jet_vtx_py[jet_i], m_jet_vtx_pz[jet_i]);
    TVector3 secVtxRelPos(secVtxPos-priVtxPos);
  // Secondary vertex momentum that is transverse to the vertex position vector relative to the PV: = rsv x psv / |rsv|
    float secVtxPtSq = (secVtxRelPos.Mag2() > 0 ? secVtxRelPos.Cross(secVtxMom).Mag2()/secVtxRelPos.Mag2() : 0);

    //ROOT.TMath.Sqrt(sv_mass*sv_mass + sv_pt2) + ROOT.TMath.Sqrt(sv_pt2)
    return sqrt(m_jet_msv[jet_i]*m_jet_msv[jet_i] + secVtxPtSq) + sqrt(secVtxPtSq);
}


float EventHandler::calculateJetTagEvtWeight(string hfOpPt, string svOpPt, bool debug)
{ // For given flavor tag, calculate an event weight based on jet tagging efficiency and tagging data/mc scalefactors.
  // See link for method used: https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
  // MODIFIED 2017-03-03 - Currently doesn't take into account differing SV operating points. Just uses standard msv OP.
    float probData = 1.0;
    float probMC   = 1.0;
  // For each valid jet...
    if(debug) cout << "      For " << validJets.size() << " jets: ";
    if(debug) cout << "\n      f vi(ei){eff,sf}tag pt eta hftagVal hfTagOP svVal svOP -->PD PM";
    for(Index vJet_i=0, jet_i=0; vJet_i<validJets.size(); vJet_i++)
    {
        jet_i = validJets[vJet_i]; // Get jet_i, the valid jet's actual index within the raw array.

      // Get the jet flavor.
        char flv = 'l';
        switch(abs(m_jet_hadflv[jet_i]))
        {  case 5 : flv = 'b'; break;
           case 4 : flv = 'c'; break;
           default: flv = 'l';
        }
      // Get the jet tagging efficiency and SF
        float jetEff = anCfg.jetTagWeight.getJetEff(flv, hfOpPt, svOpPt, m_jet_pt[jet_i], m_jet_eta[jet_i]);
        float jetSF  = anCfg.jetTagWeight.getJetSF (flv, hfOpPt, m_jet_pt[jet_i], m_jet_eta[jet_i]);

      // Get whether this jet was tagged or not.
        bool tagged = HFJets[hfOpPt][svOpPt][vJet_i];

      // Factor into probability values.
        probData *= (tagged ? jetEff*jetSF : 1.0-jetEff*jetSF );
        probMC   *= (tagged ? jetEff       : 1.0-jetEff       );
        if(debug) cout << "\n      " << flv << " " << vJet_i << "(" << jet_i << "){"<<jetEff<<","<<jetSF<<"}"<<(tagged?'t':'n') << " "
        << setprecision(2) << m_jet_pt[jet_i] << " " << m_jet_eta[jet_i] << " "
        << HFTagDiscrimVar[hfOpPt][jet_i] << " " << HFTagDiscrimOP[hfOpPt] << " "
        << SVVariable[svOpPt][jet_i] << " " << SVMinimumVal[svOpPt] << " "
        << " --> " << probData << " / " << probMC;
    }
    float wt = probData/probMC;
    if(debug) cout << "\n      jetTagEvtWeight calculated for " << hfOpPt << ": " << svOpPt << ": " << wt << std::fixed << endl;

/////////////// TO FIX: 2017-03-03 TEMPORARY SOLUTION: Set wt to zero until new jet eff. histograms w/ sv selection created.
    if(TMath::IsNaN(wt) && svOpPt != "noSV") wt = 1.0;
///////////////

    return wt;
}
