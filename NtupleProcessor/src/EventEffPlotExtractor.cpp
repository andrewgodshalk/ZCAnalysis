/*
  EventEffPlotExtractor.cpp

   Created on: May 19, 2015
       Author: godshalk

       2016-03-04 - TO DO: Add a proper SF on/off option.
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
#include "../interface/EventEffPlotExtractor.h"

using std::stringstream;   using std::string;     using std::cout;   using std::vector;
using std::endl;           using std::ofstream;   using std::setw;   using std::min;
using std::map;
typedef unsigned int Index;

const vector<TString> EventEffPlotExtractor::HFTags = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS"};
const vector<TString> EventEffPlotExtractor::SVTypes= {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};

EventEffPlotExtractor::EventEffPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    EventEffPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;

  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    usingDY          = (options.Contains("DY"        , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);
    selectingZl      = (options.Contains("Zl"        , TString::kIgnoreCase) ? true : false);
    selectingZc      = (options.Contains("Zc"        , TString::kIgnoreCase) ? true : false);
    selectingZb      = (options.Contains("Zb"        , TString::kIgnoreCase) ? true : false);

  // Set up a template histogram based on what type of file is specified in options.
    hDir->cd();     // Move to this extractor's output directory to initialize the histograms.
    TH1* tempEvtHisto   = new TH1F( "n_evts", "Event Tagging Eff. vs. Lead Taggd Jet p_{T}" ";Lead Tagged Jet p_{T} (GeV)" ";Tagging Eff.", 600, 0, 600); tempEvtHisto->Sumw2();
    TString evtFlavTitle = "nZfEvents" ;
    for(    const TString& hfLabel : EventEffPlotExtractor::HFTags )
        for(const TString& svLabel : EventEffPlotExtractor::SVTypes)
    { // Make a histogram that contains the results from tagged jets.
      TString fullEvtTitle   = evtFlavTitle+'_'+hfLabel+svLabel;
      h[fullEvtTitle] = (TH1F*)tempEvtHisto->Clone(fullEvtTitle); // Make a histogram to contain results from Z+f events.
    }
    // Delete template histograms
    delete tempEvtHisto;
}

void EventEffPlotExtractor::fillHistos()
{   nEvents["Entries Analyzed"]++;

  // Set up sign for negatively weighted events (for AMC@NLO sets)
    evtWeight = (evt.evtWeight < 0 ? -1.0 : 1.0);

  // If event isn't in JSON and isn't a simulation event, move on to the next event.
    if(!evt.inJSON && !usingSim) return;

  // If using SIM, check starting conditions to see if a certain type of event is desired.
    if(usingSim)
    {   if(      selectingZb && !evt.hasBJet               ) return;
        else if( selectingZc && (evt.hasBJet||!evt.hasCJet)) return;
        else if( selectingZl && (evt.hasBJet|| evt.hasCJet)) return;
    }
    // From this point, no flavor checks are needed. This is the desired type of event.
    // Filling is done w/ leading HF-tagged jet, no matter what.

  // Check if the event has triggered for the desired decay chain. If not, kill.
    if( (selectingZee && evt.isElTriggered) || (selectingZuu && evt.isMuTriggered)) nEvents["Triggered"]++;
    else return;

  // Select for Z events with at least one jet.
    if(!evt.isZpJEvent || !evt.hasValidMET) return;
    nEvents["Valid Z+j Event w/ MET cut"]++;

  // Cycle through HF-tags and SV types, filling histos if the Z+j event passes selection for them.
    for(    const TString& hfLabel : EventEffPlotExtractor::HFTags )
        for(const TString& svLabel : EventEffPlotExtractor::SVTypes)
    {
      // Select for Z events with at least one HF jet and with the MET cut.
        if(!evt.hasHFJets[hfLabel][svLabel]) continue;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+","+svLabel+")"]++;
        float evt_tag_wt = evt.jetTagEvtWeight[hfLabel][svLabel]; // Event weight based on previously calculated Jet Tag Efficiency and btag scale factors.
        h[TString("nZfEvents_")+hfLabel+svLabel]->Fill(
            evt.m_jet_pt[evt.validJets[evt.leadHFJet[hfLabel][svLabel]]],
            evtWeight*evt_tag_wt
        );
    }
}


void EventEffPlotExtractor::saveToFile()
{
  // Move to this plotter's directory, then save each file to the directory or overwrite.
    cout << "   EventEffPlotExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->Write();

  // Output count results.
    cout << "EventEffPlotExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;
}
