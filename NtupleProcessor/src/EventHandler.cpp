/*
 * EventHandler.cpp
 *
 *  Created on: May 14, 2015
 *      Author: godshalk
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include <TBranch.h>
#include <TLeaf.h>
#include <TMath.h>
#include "../interface/EventHandler.h"

using std::cout;   using std::endl;   using std::vector;   using std::swap;
using std::setw;   using std::setprecision;

EventHandler::EventHandler(TString fnac, TString o) : anCfg(fnac), options(o)
{
  // Check the option to see if we're working with Simulation or Data
    usingSim           = (options.Contains("Sim"        , TString::kIgnoreCase) ? true : false);
    usingDY            = (options.Contains("DY"         , TString::kIgnoreCase) ? true : false);
    usingTopHalfDY     = (options.Contains("TOPHALF"    , TString::kIgnoreCase) ? true : false);
    usingBottomHalfDY  = (options.Contains("BOTTOMHALF" , TString::kIgnoreCase) ? true : false);
    usingEvenEventDY   = (options.Contains("EVEN"       , TString::kIgnoreCase) ? true : false);
    usingOddEventDY    = (options.Contains("ODD"        , TString::kIgnoreCase) ? true : false);

    patEventsAnalyzed = 0;
    entriesInNtuple   = 0;
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
//        "Vtype"      , "nallMuons"         , "nallElectrons"     , "nallJets"          ,
//        "V*"         , "allMuon_pt"        , "allElectron_pt"    , "allJet_pt"         ,
//        "zdecayMode" , "allMuon_eta"       , "allElectron_eta"   , "allJet_eta"        ,
//        "EVENT*"     , "allMuon_phi"       , "allElectron_phi"   , "allJet_phi"        ,
//         "MET*"      , "allMuon_charge"    , "allElectron_charge", "allJet_csv"        ,
//                       "allMuon_pfCorrIso" ,                       "allJet_vtxMass"    ,
//        "triggerFlags",                                            "allJet_flavour"    ,
//        "weightTrig2012DiEle",
//        "weightTrig2012DiMuon"
"Vtype" , "nvLeptons"          , "nJet"       , "met_pt"   ,
"V_mass", "vLeptons_pt"        , "Jet_pt"     , "met_phi"  ,
"V_pt"  , "vLeptons_eta"       , "Jet_eta"    , "met_sumEt",
"V_eta" , "vLeptons_phi"       , "Jet_phi"    ,
"V_phi" , "vLeptons_charge"    , "Jet_btagCSV",
"json"  , "vLeptons_pfRelIso04", "Jet_vtxMass",
"evt"   ,                        "Jet_id",
"htJet30"    ,
"mhtJet30"   ,
"mhtPhiJet30",
"Jet_

    };
    for(TString br : branches_to_reactivate) tree->SetBranchStatus(br.Data(), 1);

  // Z variables
    m_zdecayMode = 0;
    if(tree->GetListOfBranches()->FindObject("zdecayMode"))
        tree->SetBranchAddress("zdecayMode",      &m_zdecayMode  );
    tree->SetBranchAddress("Vtype"     ,          &m_Vtype       );
//    temp_branch = tree->GetBranch("V");
//    temp_branch->GetLeaf( "mass" )->SetAddress(   &m_Z_mass      );
//    temp_branch->GetLeaf( "pt"   )->SetAddress(   &m_Z_pt        );
//    temp_branch->GetLeaf( "eta"  )->SetAddress(   &m_Z_eta       );
//    temp_branch->GetLeaf( "phi"  )->SetAddress(   &m_Z_phi       );
    tree->SetBranchAddress( "V_mass", &m_Z_mass      );
    tree->SetBranchAddress( "V_pt"  , &m_Z_pt        );
    tree->SetBranchAddress( "V_eta" , &m_Z_eta       );
    tree->SetBranchAddress( "V_phi" , &m_Z_phi       );

  // JSON
//    temp_branch = tree->GetBranch("EVENT");
//    temp_branch->GetLeaf( "json"  )->SetAddress(   &m_json        );
//    temp_branch->GetLeaf( "event" )->SetAddress(   &m_event       );
    tree->SetBranchAddress( "json", &m_json        );
    tree->SetBranchAddress( "evt" , &m_event       );

  // Muon variables
//    tree->SetBranchAddress( "nallMuons"         , &m_nMuons      );
//    tree->SetBranchAddress( "allMuon_pt"        ,  m_muon_pt     );
//    tree->SetBranchAddress( "allMuon_eta"       ,  m_muon_eta    );
//    tree->SetBranchAddress( "allMuon_phi"       ,  m_muon_phi    );
//    tree->SetBranchAddress( "allMuon_charge"    ,  m_muon_charge );
//    tree->SetBranchAddress( "allMuon_pfCorrIso" ,  m_muon_iso    );
    tree->SetBranchAddress( "nvLeptons"           , &m_nMuons      );
    tree->SetBranchAddress( "vLeptons_pt"         ,  m_muon_pt     );
    tree->SetBranchAddress( "vLeptons_eta"        ,  m_muon_eta    );
    tree->SetBranchAddress( "vLeptons_phi"        ,  m_muon_phi    );
    tree->SetBranchAddress( "vLeptons_charge"     ,  m_muon_charge );
    tree->SetBranchAddress( "vLeptons_pfRelIso04" ,  m_muon_iso    );

  // Muon variables
//    tree->SetBranchAddress( "nallElectrons"        , &m_nElecs      );
//    tree->SetBranchAddress( "allElectron_pt"       ,  m_elec_pt     );
//    tree->SetBranchAddress( "allElectron_eta"      ,  m_elec_eta    );
//    tree->SetBranchAddress( "allElectron_phi"      ,  m_elec_phi    );
//    tree->SetBranchAddress( "allElectron_charge"   ,  m_elec_charge );
//    tree->SetBranchAddress( "allElectron_pfCorrIso",  m_elec_iso    );

  // Jet variables
//    tree->SetBranchAddress( "nallJets"          , &m_nJets       );
//    tree->SetBranchAddress( "allJet_pt"         ,  m_jet_pt      );
//    tree->SetBranchAddress( "allJet_eta"        ,  m_jet_eta     );
//    tree->SetBranchAddress( "allJet_phi"        ,  m_jet_phi     );
//    tree->SetBranchAddress( "allJet_csv"        ,  m_jet_csv     );
//    tree->SetBranchAddress( "allJet_vtxMass"    ,  m_jet_msv     );
//    tree->SetBranchAddress( "allJet_flavour"    ,  m_jet_flv     );
    tree->SetBranchAddress( "nJet"           , &m_nJets       );
    tree->SetBranchAddress( "Jet_pt"         ,  m_jet_pt      );
    tree->SetBranchAddress( "Jet_eta"        ,  m_jet_eta     );
    tree->SetBranchAddress( "Jet_phi"        ,  m_jet_phi     );
    tree->SetBranchAddress( "Jet_btagCSV"    ,  m_jet_csv     );
    tree->SetBranchAddress( "Jet_vtxMass"    ,  m_jet_msv     );
    tree->SetBranchAddress( "Jet_id"         ,  m_jet_flv     );

  // MET variables
//    temp_branch = tree->GetBranch("MET");
//    temp_branch->GetLeaf( "et"   )->SetAddress( &m_MET_et    );
//    temp_branch->GetLeaf( "phi"  )->SetAddress( &m_MET_phi   );
//    temp_branch->GetLeaf( "sumet")->SetAddress( &m_MET_sumet );
//    temp_branch->GetLeaf( "sig"  )->SetAddress( &m_MET_sig   );
//    temp_branch = tree->GetBranch("MET");
    tree->SetBranchAddress( "met_pt"     , &m_MET_et    );
    tree->SetBranchAddress( "met_phi"    , &m_MET_phi   );
    tree->SetBranchAddress( "met_sumEt"  , &m_MET_sumet );
    tree->SetBranchAddress( "htJet30"    , &m_ht        );
    tree->SetBranchAddress( "mhtJet30"   , &m_mht_phi   );
    tree->SetBranchAddress( "mhtPhiJet30", &m_mht       );

//    temp_branch->GetLeaf( "met_rawPt"  )->SetAddress( &m_MET_sig   );

  // Trigger variables
//    tree->SetBranchAddress( "triggerFlags",         m_triggers   );
//    tree->SetBranchAddress( "weightTrig2012DiEle" , &m_wt_diEle  );
//    tree->SetBranchAddress( "weightTrig2012DiMuon", &m_wt_diMuon );

    return true;
}


void EventHandler::evalCriteria()
{ // Evaluates the class' list of event selection criteria

//cout << "   EventHandler::evalCriteria(): BEGIN." << endl;
//cout << "    EventHandler::evalCriteria(): TEST: EVENT NUMBER = " << m_event << endl;

    resetSelectionVariables();

////////////////////////////////////////////////
// 2016-03-15 - SPECIAL TEMP FITTING CHECK
    if(usingTopHalfDY    && m_event <= 30000000) return;
    if(usingBottomHalfDY && m_event >  30000000) return;
    if(usingEvenEventDY  && m_event%2 == 1     ) return;
    if(usingOddEventDY   && m_event%2 == 0     ) return;
////////////////////////////////////////////////

  // Check JSON if working with a data event.
    if(usingSim) inJSON = false;       // If using simulation, automatically set the JSON variable to false.
    else         inJSON = m_json==1 || !anCfg.jsonSelect;   //  Otherwise, go with what value is given by the ntuple or set to true if not checking.
    cout << m_json << " ";

  // Check if event has the required triggers. Kick if not triggered.
//    isElTriggered = triggered(anCfg.elecTriggers);
//    isMuTriggered = triggered(anCfg.muonTriggers);
    isMuTriggered = true;
    //if(!isElTriggered && !isMuTriggered) return;

  // Check for the proper number or leptons. Kick if neither.
//    if( m_nElecs<2 && m_nMuons<2 ) return;
    if( m_nMuons<2 ) return;
//cout << "    EventHandler::evalCriteria(): TEST: SELECTION CHECK: # Muons = " << m_nMuons;
//    if( m_nMuons<2 ) { cout << endl; return;}
//    cout << " --> SELECTED!!" << endl;

  // Lepton selection
    // **************PROBLEM AREA*********************
    // Not perfect... takes leading leptons. Makes the isolation and kinematic checks. Constructs Z.
    // Doesn't take into account that a lower-pt lepton might be a Z-daughter.
    // Need to go back and rework how this information is saved. May be too general.
    // May want to add some control plot information to the Ntupler/PATTupler.
    // For now, just finds leptons without intermediate steps.
    // Also need to implement trigger matching (or see if implemented in previous steps).

  // Perform selection on leptons and store indexes sorted by pt.
    for(Index i=0; i<m_nMuons; i++)
    {
//cout << "    EventHandler::evalCriteria(): Muon Props (muon #, pt, eta, iso): = (" << i << ", " << m_muon_pt[i] << ", " << m_muon_eta[i] << ", " << m_muon_iso[i] << ")" << endl;
      // Perform selection on this muon. Skip to next if it doesn't meet criteria.
        if(         m_muon_pt [i] <anCfg.muonPtMin
            || fabs(m_muon_eta[i])>anCfg.muonEtaMax
            ||      m_muon_iso[i] >anCfg.muonIsoMax
          ) continue;
      // Insert muon in list based on pt.
        Index lowPtIndex = i;
        for(int j=0; j<validMuons.size(); j++) if(m_muon_pt[validMuons[j]]<m_muon_pt[lowPtIndex]) swap(validMuons[j], lowPtIndex);
        validMuons.push_back(lowPtIndex);
    }
//if(validMuons.size()>=2) cout << "    --> TWO VALID MUONS with vType == " << m_Vtype << ", charges of " << m_muon_charge[0] << "," << m_muon_charge[1] << endl;
//    for(Index i=0; i<m_nElecs; i++)
//    {
//      // Perform selection on this electron.
//        if(    m_elec_pt [i] <anCfg.elecPtMin
//            || (fabs(m_elec_eta[i])>anCfg.elecEtaInnerMax && (fabs(m_elec_eta[i])<anCfg.elecEtaOuterMin || fabs(m_elec_eta[i])>anCfg.elecEtaOuterMax))
//            //|| m_elec_iso[i] >anCfg.elecIsoMax
//          ) continue;
//      // Insert electron in list based on pt.
//        Index lowPtIndex = i;
//        for(int j=0; j<validElectrons.size(); j++) if(m_elec_pt[validElectrons[j]]<m_elec_pt[lowPtIndex]) swap(validElectrons[j], lowPtIndex);
//        validElectrons.push_back(lowPtIndex);
//    }

  // Check lepton id, isolation (TO IMPLEMENT. ALREADY DONE IN NTUPLER)
  //

  // Check for two valid muons, electrons. (Assume 0, 1 are leading and subleading.)
    hasValidMuons =    validMuons.size() >=2   //  && m_Vtype==0
                    && (m_muon_charge[validMuons[0]]*m_muon_charge[validMuons[1]]==-1 || !anCfg.dilepMuonReqOppSign )
                    //&& isMuTriggered
                    && m_Vtype==0
    ;
//    hasValidElectrons =    validElectrons.size()>=2 //&& m_Vtype==1
//                        && (m_elec_charge[validElectrons[0]]*m_elec_charge[validElectrons[1]]==-1 || !anCfg.dilepElecReqOppSign )
//                        && isElTriggered
//    ;

  // Calculate Z_DelR based on valid muons/electrons.
    Z_DelR = Z_DelPhi = Z_DelEta = -1;
    if(hasValidMuons)
    {
        Z_DelEta = fabs(m_muon_eta[validMuons[0]]-m_muon_eta[validMuons[1]]);
        Z_DelPhi = fabs(m_muon_phi[validMuons[0]]-m_muon_phi[validMuons[1]]);
        if(Z_DelPhi > TMath::Pi()) Z_DelPhi = 2.0*TMath::Pi() - Z_DelPhi;
        Z_DelR   = sqrt(Z_DelEta*Z_DelEta+Z_DelPhi*Z_DelPhi);
    }
//    else if(hasValidElectrons)
//    {
//        Z_DelEta = fabs(m_elec_eta[validElectrons[0]]-m_elec_eta[validElectrons[1]]);
//        Z_DelPhi = fabs(m_elec_phi[validElectrons[0]]-m_elec_phi[validElectrons[1]]);
//        if(Z_DelPhi > TMath::Pi()) Z_DelPhi -= TMath::Pi();
//        Z_DelR   = sqrt(Z_DelEta*Z_DelEta+Z_DelPhi*Z_DelPhi);
//    }

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

  // If there are valid jets, make some vectors to contain their heavy flavor tagging properties.
    if(validJets.size()>0)
    {
        HFJets["CSVS"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVS"] = false;
        HFJets["CSVT"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVT"] = false;
        HFJets["CSVM"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVM"] = false;
        HFJets["CSVL"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVL"] = false;
        HFJets["SVT" ] = vector<bool>(validJets.size(), false);   hasHFJets["SVT" ] = false;
        HFJets["NoHF"] = vector<bool>(validJets.size(), false);   hasHFJets["NoHF"] = false;
    }

  // Check HF Tagging info for all valid jets
    for(Index vJet_i=0, evt_i=0; vJet_i<validJets.size(); vJet_i++)
    { // Jet is HF if it passes the CSV operating point and has a reconstructed secondary vertex.
        evt_i = validJets[vJet_i];  // Set the evt_i to the validJet's index within the EventHandler.
        if(m_jet_csv[evt_i] < anCfg.stdCSVOpPts["NoHF"]) cout << "   csv sub-NoHF: "  << m_jet_csv[evt_i] << " < " << anCfg.stdCSVOpPts["NoHF"] << endl;
        if(m_jet_msv[evt_i] < anCfg.noSVT)               cout << "   csv sub-noSVT: " << m_jet_msv[evt_i] << " < " << anCfg.minSVT              << endl;

        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["NoHF"] && m_jet_msv[evt_i] >=anCfg. noSVT) { HFJets["NoHF"][vJet_i]=true; if(!hasHFJets["NoHF"]) leadHFJet["NoHF"]=vJet_i; hasHFJets["NoHF"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["SVT" ] && m_jet_msv[evt_i] > anCfg.minSVT) { HFJets["SVT" ][vJet_i]=true; if(!hasHFJets["SVT" ]) leadHFJet["SVT" ]=vJet_i; hasHFJets["SVT" ] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVL"] && m_jet_msv[evt_i] > anCfg.minSVT) { HFJets["CSVL"][vJet_i]=true; if(!hasHFJets["CSVL"]) leadHFJet["CSVL"]=vJet_i; hasHFJets["CSVL"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVM"] && m_jet_msv[evt_i] > anCfg.minSVT) { HFJets["CSVM"][vJet_i]=true; if(!hasHFJets["CSVM"]) leadHFJet["CSVM"]=vJet_i; hasHFJets["CSVM"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVT"] && m_jet_msv[evt_i] > anCfg.minSVT) { HFJets["CSVT"][vJet_i]=true; if(!hasHFJets["CSVT"]) leadHFJet["CSVT"]=vJet_i; hasHFJets["CSVT"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVS"] && m_jet_msv[evt_i] > anCfg.minSVT) { HFJets["CSVS"][vJet_i]=true; if(!hasHFJets["CSVS"]) leadHFJet["CSVS"]=vJet_i; hasHFJets["CSVS"] = true; } else continue;
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
        if(fabs(m_jet_flv[evt_i])==5) { bMCJets[vJet_i]=true; if(!hasBJet) leadBJet=vJet_i; hasBJet = true; }
        if(fabs(m_jet_flv[evt_i])==4) { cMCJets[vJet_i]=true; if(!hasCJet) leadCJet=vJet_i; hasCJet = true; }
        lMCJets[vJet_i] = fabs(m_jet_flv[evt_i])!=5 && fabs(m_jet_flv[evt_i])!=4;
    }

  // Kick function if not using DY. Otherwise, check for origin from Z->tautau
    if(!usingDY) return;
    zBosonFromTaus = (m_zdecayMode==3);

//    if(isZpJEvent) printJets();

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
    validMuons.clear();
    validElectrons.clear();
    validJets.clear();
}


void EventHandler::printJets()
{
    if(!hasBJet && !hasCJet) return;
    cout << "------------------------------\n"
            "    Printing Jets..." << endl;
    for( auto& i : validJets ) cout << "   " << setw(4) << i << setprecision(4) << setw(8) << m_jet_pt[i] << setw(4) << m_jet_flv[i]
                                    << (m_jet_flv[i] == 4 || m_jet_flv[i] == -4 || m_jet_flv[i] == 5 || m_jet_flv[i] == -5 ? "    <------" : "") << "\n";
    if(usingDY)
    {
        if(hasBJet) cout << "    Leading BJet = " << validJets[leadBJet] << endl;
        if(hasCJet) cout << "    Leading CJet = " << validJets[leadCJet] << endl;
    }
    cout << endl;
}
