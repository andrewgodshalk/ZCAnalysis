/*
  ControlPlotExtractor.cpp
 
   Created on: May 19, 2015
       Author: godshalk
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TH2F.h>
#include "../interface/ControlPlotExtractor.h"

using std::stringstream;   using std::string;     using std::cout;   using std::vector;
using std::endl;           using std::ofstream;   using std::setw;
typedef unsigned int Index;

ControlPlotConfig ControlPlotExtractor::cfg = ControlPlotConfig();

ControlPlotExtractor::ControlPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    ControlPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;

  // Initialize cut flow counter
    nEvents["Analyzed in PAT"  ] = evt.patEventsAnalyzed;       // Taken from "Counts" in ntuple file
    nEvents["Entries Analyzed" ] = 0;
    nEvents["Triggered"        ] = 0;
    nEvents["Two valid leptons"] = 0;
    nEvents["Valid Z mass"     ] = 0;


  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    usingDY          = (options.Contains("DY"        , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);
    selectingZl      = (options.Contains("Zl"        , TString::kIgnoreCase) ? true : false);
    selectingZc      = (options.Contains("Zc"        , TString::kIgnoreCase) ? true : false);
    selectingZb      = (options.Contains("Zb"        , TString::kIgnoreCase) ? true : false);
    selectingZtautau = (options.Contains("Ztautau"   , TString::kIgnoreCase) ? true : false);

  // Initialize histograms from config file.
    hDir->cd();     // Move to this extractor's output directory to initialize the histograms.
    TString histoName, binStr, minStr, maxStr, histoTitle;
    for(auto& kv : cfg.h_strings)
    {
      // Extract histogram information from config file strings.
        histoName  = kv.first;
        binStr     = kv.second[0];
        minStr     = kv.second[1];
        maxStr     = kv.second[2];
        histoTitle = kv.second[3];
        for(int i=4; i<kv.second.size(); i++) histoTitle += " " + kv.second[i];

      // Check if histo already exists in the directory. If so, overwrite.
        // TO DO: Overwrite handling.
        //cout << "    ControlPlotExtractor::ControlPlotExtractor(): Creating histogram " << histoName << endl;
        h[histoName] = new TH1F(histoName, histoTitle, atoi(binStr), atof(minStr), atof(maxStr));
        hDir->WriteTObject(h[histoName], 0, "Overwrite");
    }

}


void ControlPlotExtractor::fillHistos()
{

cout << "   ControlPlotExtractor::fillHistos(): BEGIN (w/ options " << options << ")" << endl;

    nEvents["Entries Analyzed"]++;

  // If event isn't in JSON and isn't a simulation event, move on to the next event.
    if(!evt.inJSON && !usingSim) return;

  // If using a DY, check starting conditions to see if a certain type of event is desired.
    if(usingDY)
    {   if(selectingZtautau  && !evt.zBosonFromTaus        ) return;
        else if( selectingZb && !evt.hasBJet               ) return;
        else if( selectingZc && !evt.hasCJet               ) return;
        else if( selectingZl && (evt.hasBJet||evt.hasCJet) ) return;
    }


// Check if the event has triggered for the desired decay chain. If not, kill.
    if( (selectingZee && evt.isElTriggered) || (selectingZuu && evt.isMuTriggered)) nEvents["Triggered"]++;
    else return;

  // Fill raw lepton histograms.

    h["raw_mu_mult"]->Fill(evt.m_nMuons);
    for( int i=0; i<evt.m_nMuons; i++)
    {
        h["raw_mu_pt" ]->Fill(evt.m_muon_pt [i]);
        h["raw_mu_eta"]->Fill(evt.m_muon_eta[i]);
        h["raw_mu_phi"]->Fill(evt.m_muon_phi[i]);
        h["raw_mu_iso"]->Fill(evt.m_muon_iso[i]);
    }
    h["raw_el_mult"]->Fill(evt.m_nElecs);
    for( int i=0; i<evt.m_nElecs; i++)
    {
        h["raw_el_pt" ]->Fill(evt.m_elec_pt [i]);
        h["raw_el_eta"]->Fill(evt.m_elec_eta[i]);
        h["raw_el_phi"]->Fill(evt.m_elec_phi[i]);
        h["raw_el_iso"]->Fill(evt.m_elec_iso[i]);
    }

//  // Check if two leptons are present. If so, continue and log id information.
//    if(    (selectingZee && evt.m_nMuons<2)
//        || (selectingZuu && evt.m_nElecs<2)
//      ) return;

  // Fill Histos based on lepton id information (=== TO DO ===)
  //

  // If you don't have two valid leptons of the desired flavor, kick.
    if(selectingZuu && !evt.hasValidMuons    ) return;
    if(selectingZee && !evt.hasValidElectrons) return;
    nEvents["Two valid leptons"]++;
    //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found event w/ valid leptons." << endl;
    if(selectingZee) fillLeptonHistograms("vldEl");
    if(selectingZuu) fillLeptonHistograms("vldMu");

  // Fill raw histograms.
    h["vldLep_dilep_mass"  ]->Fill(evt.m_Z_mass);
    h["vldLep_dilep_pt"    ]->Fill(evt.m_Z_pt  );
    h["vldLep_dilep_eta"   ]->Fill(evt.m_Z_eta );
    h["vldLep_dilep_phi"   ]->Fill(evt.m_Z_phi );
    h["vldLep_dilep_DelR"  ]->Fill(evt.Z_DelR  );
    h["vldLep_dilep_DelEta"]->Fill(evt.Z_DelEta);
    h["vldLep_dilep_DelPhi"]->Fill(evt.Z_DelPhi);

  // Select event if it has a valid Z-mass. Increment counter and fill appropriate histograms.
    if(!evt.hasValidZBosonMass) return;
    nEvents["Valid Z mass"]++;

    h["vldZ_dilep_mass"  ]->Fill(evt.m_Z_mass);
    h["vldZ_dilep_pt"    ]->Fill(evt.m_Z_pt  );
    h["vldZ_dilep_eta"   ]->Fill(evt.m_Z_eta );
    h["vldZ_dilep_phi"   ]->Fill(evt.m_Z_phi );
    h["vldZ_dilep_DelR"  ]->Fill(evt.Z_DelR  );
    h["vldZ_dilep_DelEta"]->Fill(evt.Z_DelEta);
    h["vldZ_dilep_DelPhi"]->Fill(evt.Z_DelPhi);

    fillLeptonHistograms("vldZ");
    fillJetHistograms(   "vldZ");
    fillMETHistograms(   "vldZ");

}


void ControlPlotExtractor::saveToFile()
{
  // Last minute counter.
    nEvents["Candidates from Ntupler"    ] = evt.entriesInNtuple;

  // Move to this plotter's directory, then save each file to the directory or overwrite.
    cout << "   ControlPlotExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->cd();
    for(const auto& hist : h) hist.second->Write("", TObject::kOverwrite);
    cout << "ControlPlotExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;
}


void ControlPlotExtractor::fillLeptonHistograms(TString label)
{ // Labels for lead, sublead, and extra leptons, organized by index position.
    static vector<TString> posLabel = {"_ld", "_sl", "_ex", "_ex", "_ex", "_ex", "_ex", "_ex", "_ex", "_ex", "_ex"};
    h[label+"_mu_mult"]->Fill(evt.validMuons.size());
    for(Index i : evt.validMuons)
    { // Fill general histograms
        h[label+"_mu_pt" ]->Fill(evt.m_muon_pt [evt.validMuons[i]]);
        h[label+"_mu_eta"]->Fill(evt.m_muon_eta[evt.validMuons[i]]);
        h[label+"_mu_phi"]->Fill(evt.m_muon_phi[evt.validMuons[i]]);
        h[label+"_mu_iso"]->Fill(evt.m_muon_iso[evt.validMuons[i]]);
        if(selectingZuu)
        { // Select for lead/sublead/extra histograms
            h[label+posLabel[i]+"mu_pt" ]->Fill(evt.m_muon_pt [evt.validMuons[i]]);
            h[label+posLabel[i]+"mu_eta"]->Fill(evt.m_muon_eta[evt.validMuons[i]]);
            h[label+posLabel[i]+"mu_phi"]->Fill(evt.m_muon_phi[evt.validMuons[i]]);
            h[label+posLabel[i]+"mu_iso"]->Fill(evt.m_muon_iso[evt.validMuons[i]]);
        }
    }
    h[label+"_el_mult"]->Fill(evt.validElectrons.size());
    for(Index i : evt.validElectrons)
    { // Fill general histograms
        h[label+"_el_pt" ]->Fill(evt.m_elec_pt [evt.validElectrons[i]]);
        h[label+"_el_eta"]->Fill(evt.m_elec_eta[evt.validElectrons[i]]);
        h[label+"_el_phi"]->Fill(evt.m_elec_phi[evt.validElectrons[i]]);
        h[label+"_el_iso"]->Fill(evt.m_elec_iso[evt.validElectrons[i]]);
        if(selectingZee)
        { // Select for lead/sublead/extra histograms
            h[label+posLabel[i]+"el_pt" ]->Fill(evt.m_elec_pt [evt.validElectrons[i]]);
            h[label+posLabel[i]+"el_eta"]->Fill(evt.m_elec_eta[evt.validElectrons[i]]);
            h[label+posLabel[i]+"el_phi"]->Fill(evt.m_elec_phi[evt.validElectrons[i]]);
            h[label+posLabel[i]+"el_iso"]->Fill(evt.m_elec_iso[evt.validElectrons[i]]);
        }
    }
}


void ControlPlotExtractor::fillMETHistograms(TString label)
{
    h[label+"_MET_et"   ]->Fill(evt.m_MET_et   );
    h[label+"_MET_phi"  ]->Fill(evt.m_MET_phi  );
    h[label+"_MET_sumet"]->Fill(evt.m_MET_sumet);
    h[label+"_MET_sig"  ]->Fill(evt.m_MET_sig  );
}


void ControlPlotExtractor::fillJetHistograms(TString label)
{
    static vector<TString> jetLabel = {"_1st", "_2nd", "_3rd", "_4th", "_5th", "_6th", "_7th", "_8th", "_9th"};
}
