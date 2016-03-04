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

const vector<float> EffPlotExtractor::sf_b_ptmin = { 20, 30, 40, 50, 60, 70,  80, 100, 120, 160, 210, 260, 320, 400, 500, 600 };
const vector<float> EffPlotExtractor::sf_b_ptmax = { 30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600, 800 };

EffPlotExtractor::EffPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    EffPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;
    errorType   = 1;

  // Assign numbers to SF error arrays.
  // Taken directly from https://twiki.cern.ch/twiki/pub/CMS/BtagRecommendation53X/SFb-pt_payload_Moriond13.txt
  // Cited from page https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53X
  // Future - find better way to define and store these in program.
   //pt bin ranges     // {        20,        30,        40,        50,        60,        70,        80,       100,       120,       160,       210,       260,       320,       400,       500,       600 };
                       // {        30,        40,        50,        60,        70,        80,       100,       120,       160,       210,       260,       320,       400,       500,       600,       800 };
    SFb_errors["NoHF"]  = { 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0       };
    SFb_errors["SVT" ]  = { 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0      , 1.0       };
    SFb_errors["CSVL"]  = { 0.0484285, 0.0126178, 0.0120027, 0.0141137, 0.0145441, 0.0131145, 0.0168479, 0.0160836, 0.0126209, 0.0136017, 0.019182 , 0.0198805, 0.0386531, 0.0392831, 0.0481008, 0.0474291 };
    SFb_errors["CSVM"]  = { 0.0554504, 0.0209663, 0.0207019, 0.0230073, 0.0208719, 0.0200453, 0.0264232, 0.0240102, 0.0229375, 0.0184615, 0.0216242, 0.0248119, 0.0465748, 0.0474666, 0.0718173, 0.0717567 };
    SFb_errors["CSVT"]  = { 0.0567059, 0.0266907, 0.0263491, 0.0342831, 0.0303327, 0.024608 , 0.0333786, 0.0317642, 0.031102 , 0.0295603, 0.0474663, 0.0503182, 0.0580424, 0.0575776, 0.0769779, 0.0898199 };
    SFb_errors["CSVS"]  = { 0.0567059, 0.0266907, 0.0263491, 0.0342831, 0.0303327, 0.024608 , 0.0333786, 0.0317642, 0.031102 , 0.0295603, 0.0474663, 0.0503182, 0.0580424, 0.0575776, 0.0769779, 0.0898199 };



  // Initialize cut flow counter
    nEvents["Candidates from Ntupler"    ] = 0;
    nEvents["Analyzed in PAT"            ] = evt.patEventsAnalyzed;       // Taken from "Counts" in ntuple file
    nEvents["Entries Analyzed"           ] = 0;
//    nEvents["Triggered"                  ] = 0;
//    nEvents["Two valid leptons"          ] = 0;
//    nEvents["Valid Z mass"               ] = 0;
    nEvents["Valid Z+j Event"            ] = 0;
    nEvents["Valid Z+j Event w/ MET cut" ] = 0;
    nJets  ["Valid Jets"                 ] = 0;     // For per-jet efficiency studies
    for(const TString& hfLabel : EffPlotExtractor::HFTags)
    {   nEvents[TString("Valid Z+HF Event("            )+hfLabel+")"] = 0;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"] = 0;
        nJets  [TString("Valid HF Jets ("              )+hfLabel+")"] = 0;
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
    TH1* tempEvtHisto = new TH1F( "n_evts", "Event Tagging Eff. vs. Jet p_{T}" ";Jet p_{T} (GeV)" ";Tagging Eff.", 40,  0, 200); tempEvtHisto->Sumw2();
    TH1* tempJetHisto = new TH1F( "n_jets"  , "Jet Tagging Eff. vs. Jet p_{T}" ";Jet p_{T} (GeV)" ";Tagging Eff.", 40,  0, 200); tempJetHisto->Sumw2();

  // Clone the template into necessary histograms.
    TString taggedTitle  = "nt";    // = "Number Tagged"
    TString normTitle    = "n";     // = "Number"
    TString evtFlavTitle = "" ;
    TString jetFlavTitle = "" ;
    if(usingDY)
    {
        if(selectingZb) { evtFlavTitle = "ZbEvents"; jetFlavTitle = "bJets";}
        if(selectingZc) { evtFlavTitle = "ZcEvents"; jetFlavTitle = "cJets";}
        if(selectingZl) { evtFlavTitle = "ZlEvents"; jetFlavTitle = "lJets";}
        if(evtFlavTitle!="")
        {
            TString fullEvtTitle = normTitle+"_"+evtFlavTitle;
            TString fullJetTitle = normTitle+"_"+jetFlavTitle;
            h[fullEvtTitle] = (TH1F*)tempEvtHisto->Clone(fullEvtTitle); // Make a histogram to contain results from Z+f events.
            h[fullJetTitle] = (TH1F*)tempJetHisto->Clone(fullJetTitle); // Make a histogram to contain results from f jets.
        }
    }
    for(const TString& hfLabel : EffPlotExtractor::HFTags)  // For each HFTag...
    {
        TString genEvtTitle = taggedTitle+"Evt_"+hfLabel;
        TString genJetTitle = taggedTitle+"Jet_"+hfLabel;
        h[genEvtTitle] = (TH1F*)tempEvtHisto->Clone(genEvtTitle);   // Make a histogram for all tagged events.
        h[genJetTitle] = (TH1F*)tempJetHisto->Clone(genJetTitle);   // Make a histogram for all jets.
        if(usingDY && (selectingZb || selectingZc || selectingZl))
        {   // If using DY and working with a non-TauTau event...
            TString fullEvtTagTitle  = taggedTitle+"_"+evtFlavTitle+"_"+hfLabel;    // Make a histogram that contains the results from tagged Z+f events.
            TString fullJetTagTitle  = taggedTitle+"_"+jetFlavTitle+"_"+hfLabel;    // Make a histogram that contains the results from tagged f jets.
            h[fullEvtTagTitle        ] = (TH1F*)tempEvtHisto->Clone(fullEvtTagTitle         );
            h[fullEvtTagTitle+"_pErr"] = (TH1F*)tempEvtHisto->Clone(fullEvtTagTitle+"_pErr" );
            h[fullEvtTagTitle+"_mErr"] = (TH1F*)tempEvtHisto->Clone(fullEvtTagTitle+"_mErr" );
            h[fullJetTagTitle        ] = (TH1F*)tempJetHisto->Clone(fullJetTagTitle         );
            h[fullJetTagTitle+"_pErr"] = (TH1F*)tempJetHisto->Clone(fullJetTagTitle+"_pErr" );
            h[fullJetTagTitle+"_mErr"] = (TH1F*)tempJetHisto->Clone(fullJetTagTitle+"_mErr" );
        }
    }
    delete tempEvtHisto;
    delete tempJetHisto;

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
    nJets[  "Valid Jets"                ]++;
    if(usingDY)
    {
        if(selectingZb)
        {   h["n_ZbEvents"]->Fill(evt.m_jet_pt[evt.validJets[evt.leadBJet]]);           // Fill for by-evt eff.
            for(Index i=0; i<evt.validJets.size(); i++)                                   // Cycle over valid jets
                if(evt.bMCJets[i]) h["n_bJets"]->Fill(evt.m_jet_pt[evt.validJets[i]]);  //   Store jet pt value if jet is valid b jet
        }
        if(selectingZc)
        {   h["n_ZcEvents"]->Fill(evt.m_jet_pt[evt.validJets[evt.leadCJet]]);
            for(Index i=0; i<evt.validJets.size(); i++)
                if(evt.cMCJets[i]) h["n_cJets"]->Fill(evt.m_jet_pt[evt.validJets[i]]);
        }
        if(selectingZl)
        {   h["n_ZlEvents"]->Fill(evt.m_jet_pt[evt.validJets[0]]);
            for(Index i=0; i<evt.validJets.size(); i++)
                if(evt.lMCJets[i]) h["n_lJets"]->Fill(evt.m_jet_pt[evt.validJets[i]]);
        }
    }

    for(const TString& hfLabel : EffPlotExtractor::HFTags)
    {
      // Select for Z events with at least one HF jet and with the MET cut.
        if(!evt.hasHFJets[hfLabel]) continue;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"]++;
        h[TString("ntEvt_")+hfLabel]->Fill(evt.m_jet_pt[evt.validJets[evt.leadHFJet[hfLabel]]]);
        if(usingDY)
        {
            float jet_pt = 0;
            float sfb_wt = 0;
            float sfb_er = 0;       // 2016-02-18 - jets added to histograms, weighted by HFTag SF from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53X
            if(selectingZb)
            {
                jet_pt = evt.m_jet_pt[evt.validJets[evt.leadBJet]];
                sfb_wt = sf_b_eq(   hfLabel, jet_pt);
                sfb_er = sf_b_error(hfLabel, jet_pt);
                h[TString("nt_ZbEvents_")+hfLabel        ]->Fill(jet_pt, sfb_wt        );
                h[TString("nt_ZbEvents_")+hfLabel+"_pErr"]->Fill(jet_pt, sfb_wt+sfb_er );
                h[TString("nt_ZbEvents_")+hfLabel+"_mErr"]->Fill(jet_pt, sfb_wt-sfb_er );
                for(Index i = 0; i<evt.validJets.size(); i++)       // Cycle over all jets and fill when appropriate.
                {
                    if(!evt.bMCJets[i]) continue;
                    nJets[TString("Valid HF Jets (")+hfLabel+")"]++;
                    jet_pt = evt.m_jet_pt[evt.validJets[i]];
                    sfb_wt = sf_b_eq(   hfLabel, jet_pt);
                    sfb_er = sf_b_error(hfLabel, jet_pt);
                    h[TString("nt_bJets_")+hfLabel        ]->Fill(jet_pt, sfb_wt        );
                    h[TString("nt_bJets_")+hfLabel+"_pErr"]->Fill(jet_pt, sfb_wt+sfb_er );
                    h[TString("nt_bJets_")+hfLabel+"_mErr"]->Fill(jet_pt, sfb_wt-sfb_er );
                }
            }
            if(selectingZc)
            {
                jet_pt = evt.m_jet_pt[evt.validJets[evt.leadCJet]];
                sfb_wt = sf_b_eq(   hfLabel, jet_pt);
                sfb_er = sf_b_error(hfLabel, jet_pt);
                h[TString("nt_ZcEvents_")+hfLabel        ]->Fill(jet_pt, sfb_wt        );
                h[TString("nt_ZcEvents_")+hfLabel+"_pErr"]->Fill(jet_pt, sfb_wt+sfb_er );
                h[TString("nt_ZcEvents_")+hfLabel+"_mErr"]->Fill(jet_pt, sfb_wt-sfb_er );
                for(Index i = 0; i<evt.validJets.size(); i++)       // Cycle over all jets and fill when appropriate.
                {
                    if(!evt.cMCJets[i]) continue;
                    nJets[TString("Valid HF Jets (")+hfLabel+")"]++;
                    jet_pt = evt.m_jet_pt[evt.validJets[i]];
                    sfb_wt = sf_b_eq(   hfLabel, jet_pt);
                    sfb_er = sf_b_error(hfLabel, jet_pt);
                    h[TString("nt_cJets_")+hfLabel        ]->Fill(jet_pt, sfb_wt        );
                    h[TString("nt_cJets_")+hfLabel+"_pErr"]->Fill(jet_pt, sfb_wt+sfb_er );
                    h[TString("nt_cJets_")+hfLabel+"_mErr"]->Fill(jet_pt, sfb_wt-sfb_er );
                }
            }
            if(selectingZl)
            {
                h[TString("nt_ZlEvents_")+hfLabel]->Fill(evt.m_jet_pt[evt.validJets[0]] );
                for(Index i = 0; i<evt.validJets.size(); i++)       // Cycle over all jets and fill when appropriate.
                {
                    if(!evt.lMCJets[i]) continue;
                    nJets[TString("Valid HF Jets (")+hfLabel+")"]++;
                    jet_pt = evt.m_jet_pt[evt.validJets[i]];
                    h[TString("nt_lJets_")+hfLabel]->Fill(jet_pt);
                }
            }
        }
    }
}

void EffPlotExtractor::saveToFile()
{

// Last minute counter.
//    nEvents["Candidates from Ntupler"    ] += evt.entriesInNtuple;
    nEvents["Candidates from Ntupler"    ] = 0;

  // Move to this plotter's directory, then save each file to the directory or overwrite.
    cout << "   EffPlotExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->cd();
    for(const auto& hist : h) hist.second->Write("", TObject::kOverwrite);
    cout << "EffPlotExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    for( auto& n : nJets   ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;

}


float EffPlotExtractor::sf_b_eq(const TString& hfLabel, const float& x)
{ // Equation taken directly from https://twiki.cern.ch/twiki/pub/CMS/BtagRecommendation53X/SFb-pt_payload_Moriond13.txt
  // Cited from page https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53X
  // x = jet_pt

    if(hfLabel == "CSVT" || hfLabel == "CSVS")
        return 0.869965*((1.+( 0.0335062  *x))/(1.+( 0.0304598  *x)));
    if(hfLabel == "CSVM")
        return 0.726981*((1.+( 0.253238   *x))/(1.+( 0.188389   *x)));
    if(hfLabel == "CSVL")
        return 0.981149*((1.+(-0.000713295*x))/(1.+(-0.000703264*x)));
    return 1.0;

}

float EffPlotExtractor::sf_b_error(const TString& hfLabel, const float& pt)
{
  // If not worrying about errors, return 0.
    if(errorType == 0) return 0;

    if(errorType == 1)  // Bin-by-bin error.
    {
      // Find the correct pt bin. If not in range, return 0;
        if( pt < sf_b_ptmin[0] ) return 0;
        unsigned int i=0;
        for( ; i<sf_b_ptmin.size(); i++ )
            if(pt < sf_b_ptmax[i] && pt >= sf_b_ptmin[i]) break;
        if(i == sf_b_ptmin.size()) return 0;

      // Return the error from the appropriate HF tag and pt bin.
        return SFb_errors[hfLabel][i];
    }

  // CRAP SHOOT VALUES
    if(errorType == 2)  // Largest bin error.
    {
        if(hfLabel == "CSVL") return 0.0484285;
        if(hfLabel == "CSVM") return 0.0718173;
        if(hfLabel == "CSVT") return 0.0898199;
        if(hfLabel == "CSVS") return 0.0898199;
        return 0;
    }

return 0;
}
