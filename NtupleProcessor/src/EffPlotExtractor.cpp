/*
  EffPlotExtractor.cpp
 
   Created on: May 19, 2015
       Author: godshalk
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
#include "../interface/EffPlotExtractor.h"

using std::stringstream;   using std::string;     using std::cout;   using std::vector;
using std::endl;           using std::ofstream;   using std::setw;   using std::min;
using std::map;
typedef unsigned int Index;

const vector<TString> EffPlotExtractor::HFTags = {"NoHF", "SVT", "CSVL", "CSVM", "CSVT", "CSVS"};

const vector<float> EffPlotExtractor::sf_b_ptmin = {        20,        30,        40,        50,        60,        70,        80,       100,       120,       160,       210,       260,       320,       400,       500,       600 };
const vector<float> EffPlotExtractor::sf_b_ptmax = {        30,        40,        50,        60,        70,        80,       100,       120,       160,       210,       260,       320,       400,       500,       600,       800 };

EffPlotExtractor::EffPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    EffPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;
    errorFactor = 1;
    errorType   = 1;

  // Assign numbers to SF error arrays.
  // Taken directly from https://twiki.cern.ch/twiki/pub/CMS/BtagRecommendation53X/SFb-pt_payload_Moriond13.txt
  // Cited from page https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53X
  // Future - find better way to define and store these in program.
   //pt bin ranges     // {        20,        30,        40,        50,        60,        70,        80,       100,       120,       160,       210,       260,       320,       400,       500,       600 };
                       // {        30,        40,        50,        60,        70,        80,       100,       120,       160,       210,       260,       320,       400,       500,       600,       800 };
    SFb_errors["NoHF"]  = { 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0       };
    SFb_errors["CSVL"]  = { 0.0484285, 0.0126178, 0.0120027, 0.0141137, 0.0145441, 0.0131145, 0.0168479, 0.0160836, 0.0126209, 0.0136017, 0.019182 , 0.0198805, 0.0386531, 0.0392831, 0.0481008, 0.0474291 };
    SFb_errors["CSVM"]  = { 0.0554504, 0.0209663, 0.0207019, 0.0230073, 0.0208719, 0.0200453, 0.0264232, 0.0240102, 0.0229375, 0.0184615, 0.0216242, 0.0248119, 0.0465748, 0.0474666, 0.0718173, 0.0717567 };
    SFb_errors["CSVT"]  = { 0.0567059, 0.0266907, 0.0263491, 0.0342831, 0.0303327, 0.024608 , 0.0333786, 0.0317642, 0.031102 , 0.0295603, 0.0474663, 0.0503182, 0.0580424, 0.0575776, 0.0769779, 0.0898199 };
    SFb_errors["CSVS"]  = { 0.0567059, 0.0266907, 0.0263491, 0.0342831, 0.0303327, 0.024608 , 0.0333786, 0.0317642, 0.031102 , 0.0295603, 0.0474663, 0.0503182, 0.0580424, 0.0575776, 0.0769779, 0.0898199 };



  // Initialize cut flow counter
    nEvents["Analyzed in PAT"            ] = evt.patEventsAnalyzed;       // Taken from "Counts" in ntuple file
    nEvents["Entries Analyzed"           ] = 0;
//    nEvents["Triggered"                  ] = 0;
//    nEvents["Two valid leptons"          ] = 0;
//    nEvents["Valid Z mass"               ] = 0;
    nEvents["Valid Z+j Event"            ] = 0;
    nEvents["Valid Z+j Event w/ MET cut" ] = 0;
    for(const TString& hfLabel : EffPlotExtractor::HFTags)
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

cout << " EffPlotExtractor: CREATED!!" << endl;

  // Set up a template histogram based on what type of file is specified in options.
    hDir->cd();     // Move to this extractor's output directory to initialize the histograms.
    TH1* tempHisto = new TH1F( "n_tagged", "Tagging Eff. vs. Jet p_{T}" ";Tagged Jet p_{T} (GeV)"";Tagging Eff.", 40,  0, 200); tempHisto->Sumw2();

  // Clone the template into necessary histograms.
    TString taggedTitle = "n_tagged";
    TString normTitle = "n";
    TString flavorTitle = "";
    if(usingDY)
    {
        if(selectingZb) flavorTitle = "ZbEvents";
        if(selectingZc) flavorTitle = "ZcEvents";
        if(selectingZl) flavorTitle = "ZlEvents";
        if(flavorTitle!="")
        {
            TString fullFlavTitle = normTitle+"_"+flavorTitle;
            h[fullFlavTitle] = (TH1F*)tempHisto->Clone(fullFlavTitle); // Make a histogram to contain results from Z+f events.
//            h[fullFlavTitle]->Sumw2();
        }
    }
    for(const TString& hfLabel : EffPlotExtractor::HFTags)  // For each HFTag...
    {
        TString genTitle = taggedTitle+"_"+hfLabel;
        h[genTitle] = (TH1F*)tempHisto->Clone(genTitle);   // Make a histogram for all tagged events.
//        h[genTitle]->Sumw2();
        if(usingDY && (selectingZb || selectingZc || selectingZl))
        {   // If using DY and working with a non-TauTau event...
            TString fullTagTitle  = taggedTitle+"_"+flavorTitle+"_"+hfLabel;    // Make a histogram that contains the results from tagged Z+f events.
            h[fullTagTitle ] = (TH1F*)tempHisto->Clone(fullTagTitle );
//            h[fullTagTitle ]->Sumw2();
        }
    }
    delete tempHisto;

}

void EffPlotExtractor::fillHistos()
{

//cout << "   EffPlotExtractor::fillHistos(): BEGIN (Event = " << nEvents["Entries Analyzed"] << ", w/ options " << options << ")";
    nEvents["Entries Analyzed"]++;

  // If event isn't in JSON and isn't a simulation event, move on to the next event.
    if(!evt.inJSON && !usingSim) return;

  // If using a DY, check starting conditions to see if a certain type of event is desired.
    if(usingDY)
    {   if( selectingZtautau && !evt.zBosonFromTaus        ) return;
        else if( selectingZb && !evt.hasBJet               ) return;
        else if( selectingZc && (evt.hasBJet||!evt.hasCJet)) return;
        else if( selectingZl && (evt.hasBJet|| evt.hasCJet)) return;
    }

  // Check if the event has triggered for the desired decay chain. If not, kill.
    if( (selectingZee && evt.isElTriggered) || (selectingZuu && evt.isMuTriggered)) nEvents["Triggered"]++;
    else return;

  // Select for Z events with at least one jet.
    if(!evt.isZpJEvent || !evt.hasValidMET) return;
    nEvents["Valid Z+j Event w/ MET cut"]++;
    if(usingDY)
    {
         if(selectingZb) h["n_ZbEvents"]->Fill(evt.m_jet_pt[evt.validJets[evt.leadBJet]]);
         if(selectingZc) h["n_ZcEvents"]->Fill(evt.m_jet_pt[evt.validJets[evt.leadCJet]]);
         if(selectingZl) h["n_ZlEvents"]->Fill(evt.m_jet_pt[evt.validJets[0           ]]);
    }

    for(const TString& hfLabel : EffPlotExtractor::HFTags)
    {
      // Select for Z events with at least one HF jet and with the MET cut.
        if(!evt.hasHFJets[hfLabel]) continue;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"]++;
        h[TString("n_tagged")+"_"+hfLabel]->Fill(evt.m_jet_pt[evt.validJets[0]]);
        if(usingDY)
        {
            float jet_pt = 0;
            float sfb_wt = 0;       // 2016-02-18 - jets added to histograms, weighted by HFTag SF from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53X
            if(selectingZb)
            {
                jet_pt = evt.m_jet_pt[evt.validJets[evt.leadBJet]];
                sfb_wt = sf_b_eq(hfLabel, jet_pt);
                h[TString("n_tagged_ZbEvents_")+hfLabel]->Fill(jet_pt, sfb_wt);
            }
            if(selectingZc)
            {
                jet_pt = evt.m_jet_pt[evt.validJets[evt.leadCJet]];
                sfb_wt = sf_b_eq(hfLabel, jet_pt);
                h[TString("n_tagged_ZcEvents_")+hfLabel]->Fill(jet_pt, sfb_wt);
            }
            if(selectingZl)
            {
                h[TString("n_tagged_ZlEvents_")+hfLabel]->Fill(evt.m_jet_pt[evt.validJets[0]] );
            }
        }
    }
}

void EffPlotExtractor::saveToFile()
{

// Last minute counter.
    nEvents["Candidates from Ntupler"    ] = evt.entriesInNtuple;

  // Move to this plotter's directory, then save each file to the directory or overwrite.
    cout << "   EffPlotExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->cd();
    for(const auto& hist : h) hist.second->Write("", TObject::kOverwrite);
    cout << "EffPlotExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;

}


float EffPlotExtractor::sf_b_eq(const TString& hfLabel, const float& x)
{ // Equation taken directly from https://twiki.cern.ch/twiki/pub/CMS/BtagRecommendation53X/SFb-pt_payload_Moriond13.txt
  // Cited from page https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53X
  // x = jet_pt

    if(hfLabel == "CSVT" || hfLabel == "CSVS")
        return 0.869965*((1.+( 0.0335062  *x))/(1.+( 0.0304598  *x))) + sf_b_error(hfLabel, x);
    if(hfLabel == "CSVM")
        return 0.726981*((1.+( 0.253238   *x))/(1.+( 0.188389   *x))) + sf_b_error(hfLabel, x);
    if(hfLabel == "CSVL")
        return 0.981149*((1.+(-0.000713295*x))/(1.+(-0.000703264*x))) + sf_b_error(hfLabel, x);
    return 1.0;

}

float EffPlotExtractor::sf_b_error(const TString& hfLabel, const float& pt)
{
  // If not worrying about errors, return 0.
    if(errorFactor == 0) return 0;

  // CRAP SHOOT VALUES
    if(errorType == 2)  // Largest bin error.
    {
        if(hfLabel == "CSVL") return errorFactor*0.0484285;
        if(hfLabel == "CSVM") return errorFactor*0.0718173;
        if(hfLabel == "CSVT") return errorFactor*0.0898199;
        if(hfLabel == "CSVS") return errorFactor*0.0898199;
        return 0;
    }

    if(errorType == 1)  // Bin-by-bin error.
    {
      // Find the correct pt bin. If not in range, return 0;
        if( pt < sf_b_ptmin[0] ) return 0;
        unsigned int i=0;
        for( ; i<sf_b_ptmin.size(); i++ )
            if(pt < sf_b_ptmax[i] && pt >= sf_b_ptmin[i]) break;
        if(i == sf_b_ptmin.size()) return 0;

      // Return the error from the appropriate HF tag and pt bin.
        return errorFactor*SFb_errors[hfLabel][i];
    }

return 0;
}
