/*
  JetEffPlotExtractor.cpp

   Created on: May 19, 2015
   Modified: March 15th, 2017
       Author: godshalk

   2017-03-15 - Refactored into Jet, Event classes.
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TH2F.h>
#include "../interface/JetEffPlotExtractor.h"

using std::string;  using std::cout;       using std::vector;
using std::endl;    using std::ofstream;   using std::setw;
using std::min;     using std::map;
typedef unsigned int Index;

const vector<TString> JetEffPlotExtractor::HFTags = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS"};
const vector<TString> JetEffPlotExtractor::SVTypes= {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
const vector<TString> JetEffPlotExtractor::selectionLabels = {"loose", "zpjmet"};   // Set up histograms for differing selections:
  // loose: Only with triggering and json selections.
  // zpjmet : Full ZpJ selection.


JetEffPlotExtractor::JetEffPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    JetEffPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;

  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);

  // Set up flavor labels appropriately. If using sim, separate into all flavors. If not, just use collect for all jets ('a').
    if(usingSim) flavorLabels = {'b','c','l','a'};
    else         flavorLabels = {'a'};
    TString numPfx = "n";  // = "Number"

  // Set up a template histogram based on what type of file is specified in options.
    hDir->cd();     // Move to this extractor's output directory to initialize the histograms.
    TH1* tempJetHisto   = new TH1F( "n_jets"     , "Jet Tagging Eff. vs. Jet p_{T}"         ";Jet p_{T} (GeV)" ";Tagging Eff.", 600, 0, 600                );
    TH1* tempJet2DHisto = new TH2F( "n_jets_2D"  , "Jet Tagging Eff. vs. Jet p_{T} vs. Eta" ";Jet p_{T} (GeV)" ";Eta"         , 600, 0, 600, 60,  -3.0, 3.0);
    tempJetHisto  ->Sumw2();   tempJet2DHisto->Sumw2();   tempJet2DHisto->SetOption("colz");

  // Clone the template into necessary histograms. Cycle over all conditions
    for( const TString& selLabel : selectionLabels)
    { // Create a directory for this selection.
        TDirectory* selDir = hDir->mkdir(selLabel);   selDir->cd();
      // Now cycle through tag/sv conditions and create histograms.
        for(        const char&    flavor  : flavorLabels                )
            for(    const TString& hfLabel : JetEffPlotExtractor::HFTags )
                for(const TString& svLabel : JetEffPlotExtractor::SVTypes)
        { // Make a histogram that contains the results from tagged jets.
            TString jetPlotTitle = numPfx+'_'+hfLabel+svLabel+'_'+flavor+"Jets";
            h[selLabel+'_'+jetPlotTitle      ] = (TH1F*)tempJetHisto  ->Clone(jetPlotTitle      );
            h[selLabel+'_'+jetPlotTitle+"_2D"] = (TH2F*)tempJet2DHisto->Clone(jetPlotTitle+"_2D");
        }
    }
  // Delete template histograms
    hDir->cd();
    delete tempJetHisto;   delete tempJet2DHisto;
}


// fillHistos() -> Standard function called for each event. Evaluates selection criteria, then fills appropriate histograms.
void JetEffPlotExtractor::fillHistos()
{   nEvents["Entries Analyzed"]++;

  // Set up sign for negatively weighted events (for AMC@NLO sets)
    double evtWeight = (evt.evtWeight < 0 ? -1.0 : 1.0);

  // If event isn't from simulation and isn't in JSON, kick.
    if( !usingSim && !evt.inJSON) return;
  // Also, if the event isn't triggered for the proper decay channel, kick.
    if((selectingZee && !evt.isElTriggered) || (selectingZuu && !evt.isMuTriggered)) return;
    nEvents["Triggered"]++;

  // Select for Z events with at least one jet.
    bool zpjmetSelect = evt.isZpJEvent && evt.hasValidMET;

  // Fill some counters.
    nJets["Valid Jets"] += evt.validJets.size();
    if(zpjmetSelect)
    {   nEvents["Valid Z+j Event w/ MET cut"]++;
        nJets  ["Valid Jets from ZpJ"] += evt.validJets.size();
    }

    // Fill histograms
    for(const TString& selLabel : selectionLabels)
    { // Check if this event meets the desired selection. If not, move to the next.
        if(selLabel=="zpjmet" && !zpjmetSelect) continue;

      // Cycle through the hf/sv combinations.
        for(    const TString& hfLabel : JetEffPlotExtractor::HFTags )
            for(const TString& svLabel : JetEffPlotExtractor::SVTypes)
        { // Check that the event has the proper hfsv-tagged jet.
            if(!evt.hasHFJets[hfLabel][svLabel]) continue;
            nEvents[selLabel+" ("+hfLabel+","+svLabel+")"]++;

          // Cycle over valid jets and fill "all jet" histos for each that meet tagging requirement.
            for(Index i=0; i<evt.validJets.size(); i++)
            {   if(!evt.HFJets[hfLabel][svLabel][i]) continue;
                nJets[TString("Jets from ")+selLabel+" ("+hfLabel+","+svLabel+")"]++;
                float jet_pt  = evt.m_jet_pt[ evt.validJets[i]];
                float jet_eta = evt.m_jet_eta[evt.validJets[i]];
                ((TH1*) h[ selLabel+"_n_"+hfLabel+svLabel+"_aJets"   ]) ->Fill(jet_pt,          evtWeight);
                ((TH2*) h[ selLabel+"_n_"+hfLabel+svLabel+"_aJets_2D"]) ->Fill(jet_pt, jet_eta, evtWeight);
              // Fill flavor-specific histograms if using simulation.
                if(!usingSim) continue;
                char flv = (evt.bMCJets[i] ? 'b' : (evt.cMCJets[i] ? 'c' : 'l'));
                ((TH1*) h[ selLabel+"_n_"+hfLabel+svLabel+'_'+flv+"Jets"   ]) ->Fill(jet_pt,          evtWeight);
                ((TH2*) h[ selLabel+"_n_"+hfLabel+svLabel+'_'+flv+"Jets_2D"]) ->Fill(jet_pt, jet_eta, evtWeight);
                nJets[TString("Jets(")+'c'+") from "+selLabel+" ("+hfLabel+","+svLabel+")"]++;
            }
        }
    }
}


void JetEffPlotExtractor::saveToFile()
{
    hDir->cd();   hDir->Write();

  // Print cut tables.
    cout << "JetEffPlotExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    for( auto& n : nJets   ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;

}
