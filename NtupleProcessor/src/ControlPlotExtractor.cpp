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
using std::endl;           using std::ofstream;   using std::setw;   using std::min;
typedef unsigned int Index;

// Initialize class statics
    ControlPlotConfig ControlPlotExtractor::cfg       = ControlPlotConfig();

const vector<TString> ControlPlotExtractor::multName  = {"", "_ld"  , "_sl"      , "_ex"   };
const vector<TString> ControlPlotExtractor::multTitle = {"", "Lead ", "Sub-lead ", "Extra "};

const vector<TString> ControlPlotExtractor::HFTags    = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS"};

ControlPlotExtractor::ControlPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    ControlPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;

  // Initialize cut flow counter
    nEvents["Analyzed in PAT"            ] = evt.patEventsAnalyzed;       // Taken from "Counts" in ntuple file
    nEvents["Entries Analyzed"           ] = 0;
    nEvents["Triggered"                  ] = 0;
    nEvents["Two valid leptons"          ] = 0;
    nEvents["Valid Z mass"               ] = 0;
    nEvents["Valid Z+j Event"            ] = 0;
    nEvents["Valid Z+j Event w/ MET cut" ] = 0;
    for(const TString& hfLabel : ControlPlotExtractor::HFTags)
    {   nEvents[TString("Valid Z+HF Event("            )+hfLabel+")"] = 0;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"] = 0;
    }

  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    usingDY          = (options.Contains("DY"        , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);
    selectingZl      = (options.Contains("Zl"        , TString::kIgnoreCase) ? true : false);
    selectingZc      = (options.Contains("Zc"        , TString::kIgnoreCase) ? true : false);
    selectingZb      = (options.Contains("Zb"        , TString::kIgnoreCase) ? true : false);
    selectingZtautau = (options.Contains("Ztautau"   , TString::kIgnoreCase) ? true : false);

    makePhysicsObjectHistograms("muon"    , "raw"    , "(No Selection)"                   );
    makePhysicsObjectHistograms("electron", "raw"    , "(No Selection)"                   );
    makePhysicsObjectHistograms("muon"    , "vldll"  , "(Valid Leptons)"   , selectingZuu );
    makePhysicsObjectHistograms("electron", "vldll"  , "(Valid Leptons)"   , selectingZee );
    makePhysicsObjectHistograms("dilepton", "vldll"  , "(Valid Leptons)"                  );
    makePhysicsObjectHistograms(            "vldZ"   , "(Valid Z-boson)"                  );
    makePhysicsObjectHistograms(            "zpj"    , "(Z+jet Event)"                    );
    makePhysicsObjectHistograms(            "zpjmet" , "(Z+jet w/ MET cut)"               );

    for(const TString& hfLabel : ControlPlotExtractor::HFTags)
    {
        makePhysicsObjectHistograms(            TString("zhf_"   )+hfLabel , TString("(Z+HF) "           )+"("+hfLabel+")"                );
        makePhysicsObjectHistograms("hfjet"   , TString("zhf_"   )+hfLabel , TString("(Z+HF) "           )+"("+hfLabel+")" , true         );
        makePhysicsObjectHistograms(            TString("zhfmet_")+hfLabel , TString("(Z+HF w/ MET cut) ")+"("+hfLabel+")"                );
        makePhysicsObjectHistograms("hfjet"   , TString("zhfmet_")+hfLabel , TString("(Z+HF w/ MET cut) ")+"("+hfLabel+")" , true         );
    }
}

void ControlPlotExtractor::fillHistos()
{

//cout << "   ControlPlotExtractor::fillHistos(): BEGIN (Event = " << nEvents["Entries Analyzed"] << ", w/ options " << options << ")";

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
    fillMuonHistograms(    "raw", false);
    fillElectronHistograms("raw", false);

  // Check if two leptons are present. If so, continue and log id information.
    //if(    (selectingZee && evt.m_nMuons<2)
    //    || (selectingZuu && evt.m_nElecs<2)
    //  ) return;
  // Fill Histos based on lepton id information (=== TO DO ===)
    //

  // If you don't have two valid leptons of the desired flavor, kick.
    if(selectingZuu && !evt.hasValidMuons    ) return;
    if(selectingZee && !evt.hasValidElectrons) return;
    nEvents["Two valid leptons"]++;
    //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found event w/ valid leptons." << endl;
    //if(selectingZee) fillMuonHistograms(    "vl");
    //if(selectingZuu) fillElectronHistograms("vl");
    fillMuonHistograms(    "vldll");
    fillElectronHistograms("vldll");
    fillDileptonHistograms("vldll");

  // Select event if it has a valid Z-mass. Increment counter and fill appropriate histograms.
    if(!evt.hasValidZBosonMass) return;
    nEvents["Valid Z mass"]++;
    fillAllObjectHistograms("vldZ");

  // Select for Z events with at least one jet.
    if(evt.validJets.size()==0) return;
    //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found Z+j Event."
            //" HF Check: evt.hasHFJets[\"CSVT\"]=" << (evt.hasHFJets["CSVT"]?"true":"false") << endl;
    nEvents["Valid Z+j Event"]++;
    fillAllObjectHistograms("zpj");

  // Select for Z events with at least one jet and with the MET cut.
    if(evt.hasValidMET)
    {   //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found Z+j Event w/ min MET." << endl;
        nEvents["Valid Z+j Event w/ MET cut"]++;
        fillAllObjectHistograms("zpjmet");
    }

    for(const TString& hfLabel : ControlPlotExtractor::HFTags)
    {
      // Select for Z events with at least one HF jet.
        if(!evt.hasHFJets[hfLabel]) continue;
        //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found Z+HF Event." << endl;
        nEvents[TString("Valid Z+HF Event(")+hfLabel+")"]++;
        fillAllObjectHistograms(TString("zhf_")+hfLabel);

      // Select for Z events with at least one HF jet and with the MET cut.
        if(!evt.hasValidMET) continue;
        //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found Z+HF Event w/ min MET." << endl;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"]++;
        fillAllObjectHistograms(TString("zhfmet_")+hfLabel);
    }
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

void ControlPlotExtractor::makePhysicsObjectHistograms(TString prefix, TString titleAddition)
{
    makePhysicsObjectHistograms("muon"    , prefix, titleAddition, selectingZuu );
    makePhysicsObjectHistograms("electron", prefix, titleAddition, selectingZee );
    makePhysicsObjectHistograms("dilepton", prefix, titleAddition               );
    makePhysicsObjectHistograms("met"     , prefix, titleAddition               );
    makePhysicsObjectHistograms("jet"     , prefix, titleAddition, true         );
}

void ControlPlotExtractor::makePhysicsObjectHistograms(TString poLabel, TString prefix, TString titleAddition, bool split)
{
  // Unset splitting if working with objects that should be singular.
    if(poLabel == "met" || poLabel == "dilepton") split = false;

    hDir->cd();     // Move to this extractor's output directory to initialize the histograms.
    TString histoName, histoTitle;
    int numBins;   float binMin, binMax;
  // Set up key to retrieve histogram information from config file map.
    TString poKey = poLabel;
    if(poLabel=="hfjet") poKey = "JET";   // If using heavy flavor jets, use the same key as regular jets.
    else poKey.ToUpper();
    vector<vector<string> > const & h_strings = cfg.h_histoStrings[poKey];

  // Go through information for this physics object's histograms and set up the necessary histograms.
    for(vector<string> const & info : h_strings)
    {
        vector<string>::const_iterator info_itr = ++(info.begin());
      // Extract histogram information from config file strings.
        numBins    = atoi((*(info_itr++)).data());
        binMin     = atof((*(info_itr++)).data());
        binMax     = atof((*(info_itr++)).data());

      // Build appropriate title string. A bit more complicated, as you need to set specially for HF jets, add title addition, and modify based on splitting.
        histoTitle = (poLabel=="hfjet" ? "HF-Tagged " : "");
        do { histoTitle+= *(info_itr++) + " "; } while(*info_itr != ";");   // Add the standard title information
        histoTitle += titleAddition + " ";  // Take a short break from loop to add some additional text to the histogram title.
        do { histoTitle+= *(info_itr++) + " "; } while(info_itr != info.end()); // Add the remaining information, which should be the axis titles.

      // If splitting is set and not setting up a multiplicity histogram, create lead, sublead, and extra versions of this histogram.
        int i = 0;   bool splitThis = split && info[0]!="_mult";
        do {
            histoName  = prefix + "_" + poLabel + multName[i] + info[0];
            cout << "    ControlPlotExtractor::ControlPlotExtractor(): Creating histogram " << histoName << " w/ title: " << (multTitle[i]+histoTitle) << endl;
            h[histoName] = new TH1F(histoName, multTitle[i]+histoTitle, numBins, binMin, binMax);
            hDir->WriteTObject(h[histoName], 0, "Overwrite");
        } while( splitThis && (i++)<3);
    }
}

void ControlPlotExtractor::fillMuonHistograms(TString prefix, bool split)
{
    //cout << "  fillMuonHistograms(" << prefix << ")" << endl;
    //cout << "    filling : " << prefix+"_muon_mult" << endl;
    h[prefix+"_muon_mult"]->Fill(evt.validMuons.size());
    int objsEntered = 0;
    for(Index i : evt.validMuons)
    { // Fill general histograms
        //cout << "    filling : " << prefix+"_muon_pt" << endl;
        h[prefix+"_muon_pt" ]->Fill(evt.m_muon_pt [i]);
        h[prefix+"_muon_eta"]->Fill(evt.m_muon_eta[i]);
        h[prefix+"_muon_phi"]->Fill(evt.m_muon_phi[i]);
        h[prefix+"_muon_iso"]->Fill(evt.m_muon_iso[i]);
        if(!selectingZuu || !split) continue;
      // Select for lead/sublead/extra histograms
        int j = min(++objsEntered, 3);
        //cout << "    filling : " << prefix+"_muon"+multName[j]+"_pt" << endl;
        h[prefix+"_muon"+multName[j]+"_pt" ]->Fill(evt.m_muon_pt [i]);
        h[prefix+"_muon"+multName[j]+"_eta"]->Fill(evt.m_muon_eta[i]);
        h[prefix+"_muon"+multName[j]+"_phi"]->Fill(evt.m_muon_phi[i]);
        h[prefix+"_muon"+multName[j]+"_iso"]->Fill(evt.m_muon_iso[i]);
        //cout << "    entered " << j << " muons..." << endl;
    }
}

void ControlPlotExtractor::fillElectronHistograms(TString prefix, bool split)
{
    h[prefix+"_electron_mult"]->Fill(evt.validElectrons.size());
    int objsEntered = 0;      // Keeps track of how many objects you've entered for labeling purposes
    for(Index i : evt.validElectrons)
    { // Fill general histograms
        h[prefix+"_electron_pt" ]->Fill(evt.m_elec_pt [i]);
        h[prefix+"_electron_eta"]->Fill(evt.m_elec_eta[i]);
        h[prefix+"_electron_phi"]->Fill(evt.m_elec_phi[i]);
        h[prefix+"_electron_iso"]->Fill(evt.m_elec_iso[i]);
        if(!selectingZee || !split) continue;
      // Select for lead/sublead/extra histograms
        int j = min(++objsEntered, 3);
        h[prefix+"_electron"+multName[j]+"_pt" ]->Fill(evt.m_elec_pt [i]);
        h[prefix+"_electron"+multName[j]+"_eta"]->Fill(evt.m_elec_eta[i]);
        h[prefix+"_electron"+multName[j]+"_phi"]->Fill(evt.m_elec_phi[i]);
        h[prefix+"_electron"+multName[j]+"_iso"]->Fill(evt.m_elec_iso[i]);
    }
}

void ControlPlotExtractor::fillDileptonHistograms(TString prefix)
{
    h[prefix+"_dilepton_mass"  ]->Fill(evt.m_Z_mass);
    h[prefix+"_dilepton_pt"    ]->Fill(evt.m_Z_pt  );
    h[prefix+"_dilepton_eta"   ]->Fill(evt.m_Z_eta );
    h[prefix+"_dilepton_phi"   ]->Fill(evt.m_Z_phi );
    h[prefix+"_dilepton_DelR"  ]->Fill(evt.Z_DelR  );
    h[prefix+"_dilepton_DelEta"]->Fill(evt.Z_DelEta);
    h[prefix+"_dilepton_DelPhi"]->Fill(evt.Z_DelPhi);
}

void ControlPlotExtractor::fillMETHistograms(TString prefix)
{
    h[prefix+"_met_et"   ]->Fill(evt.m_MET_et   );
    h[prefix+"_met_phi"  ]->Fill(evt.m_MET_phi  );
    h[prefix+"_met_sumet"]->Fill(evt.m_MET_sumet);
    h[prefix+"_met_sig"  ]->Fill(evt.m_MET_sig  );
}

void ControlPlotExtractor::fillJetHistograms(TString prefix)
{
    h[prefix+"_jet_mult"]->Fill(evt.validJets.size());
    int objsEntered = 0;      // Keeps track of how many objects you've entered for labeling purposes
    for(Index i : evt.validJets)
    { // Fill general histograms
        h[prefix+"_jet_pt" ]->Fill(evt.m_jet_pt [i]);
        h[prefix+"_jet_eta"]->Fill(evt.m_jet_eta[i]);
        h[prefix+"_jet_phi"]->Fill(evt.m_jet_phi[i]);
        h[prefix+"_jet_csv"]->Fill(evt.m_jet_csv[i]);
        h[prefix+"_jet_msv"]->Fill(evt.m_jet_msv[i]);
      // Select for lead/sublead/extra histograms
        int j = min(++objsEntered, 3);
        h[prefix+"_jet"+multName[j]+"_pt" ]->Fill(evt.m_jet_pt [i]);
        h[prefix+"_jet"+multName[j]+"_eta"]->Fill(evt.m_jet_eta[i]);
        h[prefix+"_jet"+multName[j]+"_phi"]->Fill(evt.m_jet_phi[i]);
        h[prefix+"_jet"+multName[j]+"_csv"]->Fill(evt.m_jet_csv[i]);
        h[prefix+"_jet"+multName[j]+"_msv"]->Fill(evt.m_jet_msv[i]);
    }
    if(!prefix.Contains("hf")) return;
    objsEntered = 0;      // Keeps track of how many objects you've entered for labeling purposes
    TString hfTag = prefix(prefix.Length()-4, 4);   // Retrieve HF tag from prefix. Prefix should have form "pfx_CSVT"
    cout << hfTag << endl;
    for(int i=0; i<evt.validJets.size(); i++)
    { // Cycle through valid jet listings.
        if(!evt.HFJets[hfTag][i]) continue;     // If this jet doesn't have the appropriate tag, continue.
        Index k = evt.validJets[i];
        h[prefix+"_hfjet_pt" ]->Fill(evt.m_jet_pt [k]);
        h[prefix+"_hfjet_eta"]->Fill(evt.m_jet_eta[k]);
        h[prefix+"_hfjet_phi"]->Fill(evt.m_jet_phi[k]);
        h[prefix+"_hfjet_csv"]->Fill(evt.m_jet_csv[k]);
        h[prefix+"_hfjet_msv"]->Fill(evt.m_jet_msv[k]);
      // Select for lead/sublead/extra histograms
        int j = min(++objsEntered, 3);
        h[prefix+"_hfjet"+multName[j]+"_pt" ]->Fill(evt.m_jet_pt [k]);
        h[prefix+"_hfjet"+multName[j]+"_eta"]->Fill(evt.m_jet_eta[k]);
        h[prefix+"_hfjet"+multName[j]+"_phi"]->Fill(evt.m_jet_phi[k]);
        h[prefix+"_hfjet"+multName[j]+"_csv"]->Fill(evt.m_jet_csv[k]);
        h[prefix+"_hfjet"+multName[j]+"_msv"]->Fill(evt.m_jet_msv[k]);
    }
    h[prefix+"_hfjet_mult"]->Fill(objsEntered);     // Fill hf mult with the number of hf jets entered.
}

void ControlPlotExtractor::fillAllObjectHistograms(TString prefix)
{
    fillMuonHistograms(    prefix);
    fillElectronHistograms(prefix);
    fillDileptonHistograms(prefix);
    fillJetHistograms(     prefix);
    fillMETHistograms(     prefix);
}

