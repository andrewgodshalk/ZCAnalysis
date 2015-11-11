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

using std::cout;   using std::endl;   using std::vector;

EventHandler::EventHandler(TString fnac, TString o) : analCfg(fnac), options(o)
{
  // Check the option to see if we're working with Simulation or Data, and whether you're looking for Zee or Zuu events
    usingSim = (options.Contains("Sim", TString::kIgnoreCase) ? true : false);
    usingZee = (options.Contains("Zee", TString::kIgnoreCase) ? true : false);
    usingZuu = (options.Contains("Zuu", TString::kIgnoreCase) ? true : false);
}

bool EventHandler::mapTree(TTree* tree)
{
  // Maps TTree to class' variables.
  // TO DO: Implement check for correct mapping, return result?
  //    - Set up exception handling for negative result.
/*
  TBranch * temp_branch;  // Temporary branch to get at members of struct-branches.

  // Z variables
    tree->SetBranchAddress("Vtype", &m_Vtype);
    temp_branch = tree->GetBranch("V");
    temp_branch->GetLeaf( "mass" )->SetAddress(     &m_Z_mass      );
    temp_branch->GetLeaf( "pt"   )->SetAddress(     &m_Z_pt        );
    temp_branch->GetLeaf( "eta"  )->SetAddress(     &m_Z_eta       );
    temp_branch->GetLeaf( "phi"  )->SetAddress(     &m_Z_phi       );

  // JSON
    temp_branch = tree->GetBranch("EVENT");
    temp_branch->GetLeaf( "json" )->SetAddress(     &m_json        );

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
    temp_branch->GetLeaf( "et"  )->SetAddress(       &m_MET_et      );
    temp_branch->GetLeaf( "phi" )->SetAddress(       &m_MET_phi     );

  // Trigger variables
    tree->SetBranchAddress("triggerFlags", m_triggers);
*/
    return true;
}


void EventHandler::evalCriteria()
{ // Evaluates the class' list of event selection criteria
/*
  // Check JSON if working with a data event.
    if(usingSim) eventInJSON = false;       // If using simulation, automatically set the JSON variable to false.
    else         eventInJSON = m_json==1;   //  Otherwise, go with what value is given by the ntuple.

  // Check for valid muons, electrons, Z-boson, and MET.
    validMuons     = m_nMuons>=2   && m_Vtype==0        && m_muon_charge[0]*m_muon_charge[1]==-1        // Check number and charge of muons, and that this is a Zuu event
                  && triggered(muonTriggers)
                  && m_muon_pt[0]>=stdCuts["muonPtMin"] && fabs(m_muon_eta[0])<=stdCuts["muonEtaMax"]   // Check muon pT and eta.
                  && m_muon_pt[1]>=stdCuts["muonPtMin"] && fabs(m_muon_eta[1])<=stdCuts["muonEtaMax"];
    validElectrons = m_nElecs>=2 && m_Vtype==1          // Check number of electrons, type of Z event, then if within pT and eta range.
                  && triggered(elecTriggers)
                  && m_elec_pt[0]>=stdCuts["elecPtMin"] && (fabs(m_elec_eta[0])<=stdCuts["elecEtaInnerMax"] || (fabs(m_elec_eta[0])>=stdCuts["elecEtaOuterMin"] && fabs(m_elec_eta[0])<=stdCuts["elecEtaOuterMax"]))
                  && m_elec_pt[1]>=stdCuts["elecPtMin"] && (fabs(m_elec_eta[1])<=stdCuts["elecEtaInnerMax"] || (fabs(m_elec_eta[1])>=stdCuts["elecEtaOuterMin"] && fabs(m_elec_eta[1])<=stdCuts["elecEtaOuterMax"]));

    validZBoson    = m_Z_mass>=stdCuts["dilepInvMassMin"] && m_Z_mass<=stdCuts["dilepInvMassMax"];
    validMET       = m_MET_et<=stdCuts["metMax"];

  // Run through list of jets to find valid jets, then check those jets for various features.
    validJets = vector<Index>(0);
    for(int i=0; i<m_nJets; i++)
        if(m_jet_pt[i]>=stdCuts["jetPtMin"] && fabs(m_jet_eta[i])<=stdCuts["jetEtaMax"])
            validJets.push_back(i);

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
        if(m_jet_csv[evt_i] < stdCSVOpPts["NoHF"]) cout << "   csv sub-NoHF: "  << m_jet_csv[evt_i] << " < " << stdCSVOpPts["NoHF"] << endl;
        if(m_jet_msv[evt_i] < noSVT)               cout << "   csv sub-noSVT: " << m_jet_msv[evt_i] << " < " << minSVT              << endl;

        if(m_jet_csv[evt_i]>=stdCSVOpPts["NoHF"] && m_jet_msv[evt_i]>= noSVT) { HFJets["NoHF"][vJet_i]=true; hasHFJets["NoHF"] = true; } else continue;
        if(m_jet_csv[evt_i]>=stdCSVOpPts["CSVL"] && m_jet_msv[evt_i]>=minSVT) { HFJets["CSVL"][vJet_i]=true; hasHFJets["CSVL"] = true; } else continue;
        if(m_jet_csv[evt_i]>=stdCSVOpPts["CSVM"] && m_jet_msv[evt_i]>=minSVT) { HFJets["CSVM"][vJet_i]=true; hasHFJets["CSVM"] = true; } else continue;
        if(m_jet_csv[evt_i]>=stdCSVOpPts["CSVT"] && m_jet_msv[evt_i]>=minSVT) { HFJets["CSVT"][vJet_i]=true; hasHFJets["CSVT"] = true; } else continue;
        if(m_jet_csv[evt_i]>=stdCSVOpPts["CSVS"] && m_jet_msv[evt_i]>=minSVT) { HFJets["CSVS"][vJet_i]=true; hasHFJets["CSVS"] = true; } else continue;
    }

  // Combine a few of the checks into a couple of comprehensive variables.
    validZeeEvent = (usingSim || eventInJSON) && usingZee && validElectrons && validZBoson && validMET;
    validZuuEvent = (usingSim || eventInJSON) && usingZuu && validMuons     && validZBoson && validMET;
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
*/
}

// Returns whether or not this event has any of the listed triggers.
bool EventHandler::triggered(vector<int> &triggersToTest)
{
/*
    //for(int i : triggersToTest) cout << "    Value at trigger listing " << i << ": " << m_triggers[i] << " --> Returning " << (m_triggers[i] ? "TRUE" : "FALSE") << endl;
    for(int i : triggersToTest) if(m_triggers[i]) return true;
*/
    return false;

}
