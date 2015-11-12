/*
 * EventHandler.cpp
 *
 *  Created on: May 14, 2015
 *      Author: godshalk
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <TBranch.h>
#include <TLeaf.h>
#include "../interface/EventHandler.h"

using std::cout;   using std::endl;   using std::vector;   using std::swap;

EventHandler::EventHandler(TString fnac, TString o) : anCfg(fnac), options(o)
{
  // Check the option to see if we're working with Simulation or Data
    usingSim = (options.Contains("Sim", TString::kIgnoreCase) ? true : false);
    usingDY  = (options.Contains("DY" , TString::kIgnoreCase) ? true : false);

}

bool EventHandler::mapTree(TTree* tree)
{
  // Maps TTree to class' variables.
  // TO DO: Implement check for correct mapping, return result?
  //    - Set up exception handling for negative result.

  TBranch * temp_branch;  // Temporary branch to get at members of struct-branches.

  // Deactivate all branches, reactivate as necessary.
    tree->SetBranchStatus("*",0);
    vector<TString> branches_to_reactivate = {
        "Vtype"      , "nallMuons"         , "nallElectrons"     , "nallJets"          ,
        "V*"         , "allMuon_pt"        , "allElectron_pt"    , "allJet_pt"         ,
        "zdecayMode" , "allMuon_eta"       , "allElectron_eta"   , "allJet_eta"        ,
        "EVENT*" , "allMuon_phi"       , "allElectron_phi"   , "allJet_phi"        ,
         "MET*"      , "allMuon_charge"    , "allElectron_charge", "allJet_csv"        ,
                       "allMuon_pfCorrIso" ,                       "allJet_vtxMass"    ,
        "triggerFlags",                                            "allJet_flavour"
    };
    for(TString br : branches_to_reactivate) tree->SetBranchStatus(br.Data(), 1);

  // Z variables
    m_zdecayMode = 0;
    if(tree->GetListOfBranches()->FindObject("zdecayMode"))
        tree->SetBranchAddress("zdecayMode",      &m_zdecayMode  );
    tree->SetBranchAddress("Vtype"     ,          &m_Vtype       );
    temp_branch = tree->GetBranch("V");
    temp_branch->GetLeaf( "mass" )->SetAddress(   &m_Z_mass      );
    temp_branch->GetLeaf( "pt"   )->SetAddress(   &m_Z_pt        );
    temp_branch->GetLeaf( "eta"  )->SetAddress(   &m_Z_eta       );
    temp_branch->GetLeaf( "phi"  )->SetAddress(   &m_Z_phi       );

  // JSON
    temp_branch = tree->GetBranch("EVENT");
    temp_branch->GetLeaf( "json" )->SetAddress(   &m_json        );

  // Muon variables
    tree->SetBranchAddress( "nallMuons"         , &m_nMuons      );
    tree->SetBranchAddress( "allMuon_pt"        ,  m_muon_pt     );
    tree->SetBranchAddress( "allMuon_eta"       ,  m_muon_eta    );
    tree->SetBranchAddress( "allMuon_phi"       ,  m_muon_phi    );
    tree->SetBranchAddress( "allMuon_charge"    ,  m_muon_charge );
    tree->SetBranchAddress( "allMuon_pfCorrIso" ,  m_muon_iso    );

  // Muon variables
    tree->SetBranchAddress( "nallElectrons"     , &m_nElecs      );
    tree->SetBranchAddress( "allElectron_pt"    ,  m_elec_pt     );
    tree->SetBranchAddress( "allElectron_eta"   ,  m_elec_eta    );
    tree->SetBranchAddress( "allElectron_phi"   ,  m_elec_phi    );
    tree->SetBranchAddress( "allElectron_charge",  m_elec_charge );

  // Jet variables
    tree->SetBranchAddress( "nallJets"          , &m_nJets       );
    tree->SetBranchAddress( "allJet_pt"         ,  m_jet_pt      );
    tree->SetBranchAddress( "allJet_eta"        ,  m_jet_eta     );
    tree->SetBranchAddress( "allJet_phi"        ,  m_jet_phi     );
    tree->SetBranchAddress( "allJet_csv"        ,  m_jet_csv     );
    tree->SetBranchAddress( "allJet_vtxMass"    ,  m_jet_msv     );
    tree->SetBranchAddress( "allJet_flavour"    ,  m_jet_flv     );

  // MET variables
    temp_branch = tree->GetBranch("MET");
    temp_branch->GetLeaf(   "et"  )->SetAddress(  &m_MET_et      );
    temp_branch->GetLeaf(   "phi" )->SetAddress(  &m_MET_phi     );

  // Trigger variables
    tree->SetBranchAddress( "triggerFlags",         m_triggers    );

    return true;
}


void EventHandler::evalCriteria()
{ // Evaluates the class' list of event selection criteria

  // Check JSON if working with a data event.
    if(usingSim) inJSON = false;       // If using simulation, automatically set the JSON variable to false.
    else         inJSON = m_json==1;   //  Otherwise, go with what value is given by the ntuple.

  // Cycle through muons, electrons, find leading and sub-leading.
    ldMu = ldEl = 0;
    slMu = slEl = 1;
    if(m_nMuons>=2)
    {
        if(m_muon_pt[ldMu]<m_muon_pt[slMu]) { ldMu = 1; slMu = 0; }
        for(int i=2; i<m_nMuons; i++)
        {
            if(     m_muon_pt[i]>=m_muon_pt[ldMu]) {slMu = ldMu; ldMu = i;}
            else if(m_muon_pt[i]>=m_muon_pt[slMu])  slMu = i;
        }
    }
    if(m_nElecs>=2)
    {
        if(m_elec_pt[ldEl]<m_elec_pt[slEl]) { ldEl = 1; slEl = 0; }
        for(int i=2; i<m_nElecs; i++)
        {
            if(     m_elec_pt[i]>=m_elec_pt[ldEl]) {slEl = ldEl; ldEl = i;}
            else if(m_elec_pt[i]>=m_elec_pt[slEl])  slEl = i;
        }
    }

  // Check for valid muons, electrons, Z-boson, and MET.
    validMuons     = m_nMuons>=2   //  && m_Vtype==0
                  && (m_muon_charge[ldMu]*m_muon_charge[slMu]==-1 || !anCfg.dilepMuonReqOppSign )
                  && triggered(anCfg.muonTriggers)
                  && m_muon_pt [ldMu]>=anCfg.muonPtMin  && fabs(m_muon_eta[ldMu])<=anCfg.muonEtaMax   // Check muon pT and eta.
                  && m_muon_pt [slMu]>=anCfg.muonPtMin  && fabs(m_muon_eta[slMu])<=anCfg.muonEtaMax
                  && m_muon_iso[ldMu]<=anCfg.muonIsoMax && m_muon_iso[slMu]<=anCfg.muonIsoMax
    ;

    validElectrons = m_nElecs>=2 //&& m_Vtype==1          // Check number of electrons, type of Z event, then if within pT and eta range.
                  && (m_elec_charge[ldEl]*m_elec_charge[slEl]==-1 || !anCfg.dilepElecReqOppSign )
                  && triggered(anCfg.elecTriggers)
                  && m_elec_pt [ldEl]>=anCfg.elecPtMin  && (fabs(m_elec_eta[ldEl])<=anCfg.elecEtaInnerMax || (fabs(m_elec_eta[ldEl])>=anCfg.elecEtaOuterMin && fabs(m_elec_eta[ldEl])<=anCfg.elecEtaOuterMax))
                  && m_elec_pt [slEl]>=anCfg.elecPtMin  && (fabs(m_elec_eta[slEl])<=anCfg.elecEtaInnerMax || (fabs(m_elec_eta[slEl])>=anCfg.elecEtaOuterMin && fabs(m_elec_eta[slEl])<=anCfg.elecEtaOuterMax))
                  && m_muon_iso[ldEl]<=anCfg.muonIsoMax && m_muon_iso[slEl]<=anCfg.muonIsoMax;
    ;

  // Calculate Z_DelR based on valid muons/electrons.
    Z_DelR = Z_DelPhi = Z_DelEta = -1;
    if(validMuons)
    {
        Z_DelEta = fabs(m_muon_eta[0]-m_muon_eta[1]);
        Z_DelPhi = fabs(m_muon_phi[0]-m_muon_phi[1]);
        Z_DelR   = sqrt(Z_DelEta*Z_DelEta+Z_DelPhi*Z_DelPhi);
    }
    else if(validElectrons)
    {
        Z_DelEta = fabs(m_elec_eta[0]-m_elec_eta[1]);
        Z_DelPhi = fabs(m_elec_phi[0]-m_elec_phi[1]);
        Z_DelR   = sqrt(Z_DelEta*Z_DelEta+Z_DelPhi*Z_DelPhi);
    }

    validZBoson    = m_Z_mass>=anCfg.dilepInvMassMin && m_Z_mass<=anCfg.dilepInvMassMax;
    validMET       = m_MET_et<=anCfg.metMax;

  // Run through list of jets to find valid jets, then check those jets for various features.
    validJets = vector<Index>(0);
    for(int i=0; i<m_nJets; i++)
        if(m_jet_pt[i]>=anCfg.jetPtMin && fabs(m_jet_eta[i])<=anCfg.jetEtaMax)
        { // Insert jet in proper place in list.
            Index temp=i;
            for(int j=0; j<validJets.size(); j++)
                if(m_jet_pt[validJets[j]]<m_jet_pt[temp]) swap(temp, validJets[j]); //cout << "SWAPED!!" << endl;
            validJets.push_back(temp);
        }

  // If there are valid jets, make some vectors to contain their heavy flavor tagging properties.
    if(validJets.size()>0)
    {
        HFJets["CSVS"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVS"] = false;
        HFJets["CSVT"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVT"] = false;
        HFJets["CSVM"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVM"] = false;
        HFJets["CSVL"] = vector<bool>(validJets.size(), false);   hasHFJets["CSVL"] = false;
        HFJets["NoHF"] = vector<bool>(validJets.size(), false);   hasHFJets["NoHF"] = false;
    }

  // Check HF Tagging info for all valid jets
    for(Index vJet_i=0, evt_i=0; vJet_i<validJets.size(); vJet_i++)
    { // Jet is HF if it passes the CSV operating point and has a reconstructed secondary vertex.
        evt_i = validJets[vJet_i];  // Set the evt_i to the validJet's index within the EventHandler.
        if(m_jet_csv[evt_i] < anCfg.stdCSVOpPts["NoHF"]) cout << "   csv sub-NoHF: "  << m_jet_csv[evt_i] << " < " << anCfg.stdCSVOpPts["NoHF"] << endl;
        if(m_jet_msv[evt_i] < anCfg.noSVT)               cout << "   csv sub-noSVT: " << m_jet_msv[evt_i] << " < " << anCfg.minSVT              << endl;

        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["NoHF"] && m_jet_msv[evt_i]>=anCfg. noSVT) { HFJets["NoHF"][vJet_i]=true; hasHFJets["NoHF"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVL"] && m_jet_msv[evt_i]>=anCfg.minSVT) { HFJets["CSVL"][vJet_i]=true; hasHFJets["CSVL"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVM"] && m_jet_msv[evt_i]>=anCfg.minSVT) { HFJets["CSVM"][vJet_i]=true; hasHFJets["CSVM"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVT"] && m_jet_msv[evt_i]>=anCfg.minSVT) { HFJets["CSVT"][vJet_i]=true; hasHFJets["CSVT"] = true; } else continue;
        if(m_jet_csv[evt_i]>=anCfg.stdCSVOpPts["CSVS"] && m_jet_msv[evt_i]>=anCfg.minSVT) { HFJets["CSVS"][vJet_i]=true; hasHFJets["CSVS"] = true; } else continue;

    }

  // Combine a few of the checks into a couple of comprehensive variables.
    validZeeEvent = (usingSim || inJSON) && validElectrons && validZBoson && validMET;
    validZuuEvent = (usingSim || inJSON) && validMuons     && validZBoson && validMET;
    validStdEvent = validZeeEvent || validZuuEvent;
    validZPJEvent = validStdEvent && validJets.size()>0;

  // Kick function if not using Sim. Otherwise, check jets for flavor properties
    if(!usingSim) return;

    bMCJets = cMCJets = lMCJets = vector<bool>(validJets.size(), false);
    hasBJet = hasCJet = false;
    for(Index vJet_i=0, evt_i=0; vJet_i<validJets.size(); vJet_i++)
    {
        evt_i = validJets[vJet_i];  // Set the evt_i to the validJet's index within the EventHandler.
        if(fabs(m_jet_flv[evt_i])==5) { bMCJets[vJet_i]=true; hasBJet = true; }
        if(fabs(m_jet_flv[evt_i])==4) { cMCJets[vJet_i]=true; hasCJet = true; }
        lMCJets[vJet_i] = fabs(m_jet_flv[evt_i])!=5 && fabs(m_jet_flv[evt_i])!=4;
    }

  // Kick function if not using DY. Otherwise, check for origin from Z->tautau
    if(!usingDY) return;
    zBosonFromTaus = (m_zdecayMode==3);
}


// Returns whether or not this event has any of the listed triggers.
bool EventHandler::triggered(vector<int> &triggersToTest)
{
    for(int i : triggersToTest) if(m_triggers[i]) return true;
    return false;
}
