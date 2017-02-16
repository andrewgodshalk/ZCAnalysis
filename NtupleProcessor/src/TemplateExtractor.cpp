/*
  TemplateExtractor.cpp
 
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
#include "../interface/TemplateExtractor.h"

using std::stringstream;   using std::string;     using std::cout;   using std::vector;
using std::endl;           using std::ofstream;   using std::setw;   using std::min   ;
typedef unsigned int Index;

const std::vector<TString> TemplateExtractor::HFTags = {"NoHF", "SVT", "CSVL", "CSVM", "CSVT", "CSVS"};


TemplateExtractor::TemplateExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o), tempPrefix("template_")
{
    cout << "    TemplateExtractor: Created.\n"
            "      Options: " << options
         << endl;

    nEvents["Entries Analyzed"           ] = 0;
    nEvents["Valid Z+j Event w/ MET cut" ] = 0;
    for(const TString& hfLabel : TemplateExtractor::HFTags)
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"] = 0;

  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    usingDY          = (options.Contains("DY"        , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);
    selectingZl      = (options.Contains("Zl"        , TString::kIgnoreCase) ? true : false);
    selectingZc      = (options.Contains("Zc"        , TString::kIgnoreCase) ? true : false);
    selectingZb      = (options.Contains("Zb"        , TString::kIgnoreCase) ? true : false);
    selectingZtautau = (options.Contains("Ztautau"   , TString::kIgnoreCase) ? true : false);

  // Create template histograms (one for each operating point)
    hDir->cd();
    for(const TString& hfLabel : TemplateExtractor::HFTags)
    {
        TString histLabel_msv   = tempPrefix+"msv_"  +hfLabel;
        TString histLabel_msvqc = tempPrefix+"msvqc_"+hfLabel;
        h[histLabel_msv  ] = new TH1F(histLabel_msv  , "Jet M_{SV}; M_{SV} (GeV); Events/Bin", 200, 0, 10);
        h[histLabel_msvqc] = new TH1F(histLabel_msvqc, "Jet M_{SV,QCorr.}; M_{SV,QCorr.} (GeV); Events/Bin", 200, 0, 10);
        h[histLabel_msv  ]->Sumw2();
        h[histLabel_msvqc]->Sumw2();
        hDir->WriteTObject(h[histLabel_msv  ], 0, "Overwrite");
        hDir->WriteTObject(h[histLabel_msvqc], 0, "Overwrite");
    }
}


void TemplateExtractor::fillHistos()
{
//cout << "   TemplateExtractor::fillHistos(): BEGIN (Event = " << nEvents["Entries Analyzed"] << ", w/ options " << options << ")";

    nEvents["Entries Analyzed"]++;

  // Select for Z events with at least one jet and with the MET cut.
    if(!evt.inJSON && !usingSim) return;
    if( (selectingZee && evt.isElTriggered) || (selectingZuu && evt.isMuTriggered)) nEvents["Triggered"]++;
    else return;
    if(!evt.isZpJEvent || !evt.hasValidMET) return;

  // Set up sign for negatively weighted events.
    double evtWeight = (evt.evtWeight < 0 ? -1.0 : 1.0);

  // If using a DY, check starting conditions to see if a certain type of event is desired.
    if(usingDY)
    {   if(selectingZtautau  && !evt.zBosonFromTaus        ) return;
        else if( selectingZb && !evt.hasBJet               ) return;
        else if( selectingZc && (evt.hasBJet||!evt.hasCJet)) return;
        else if( selectingZl && (evt.hasBJet|| evt.hasCJet)) return;
    }
    nEvents["Valid Z+j Event w/ MET cut"]++;

    for(const TString& hfLabel : TemplateExtractor::HFTags)
    {
      // Select for Z events with at least one HF jet.
        if(!evt.hasHFJets[hfLabel]) continue;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+")"]++;
      // Fill templates!!
        h[tempPrefix+"msv_"  +hfLabel]->Fill(evt.m_jet_msv[evt.leadHFJet[hfLabel]]);
        h[tempPrefix+"msvqc_"+hfLabel]->Fill(evt.jet_msv_quickCorr[evt.leadHFJet[hfLabel]]);
    }
}

void TemplateExtractor::saveToFile()
{

  // Last minute counter.
    nEvents["Candidates from Ntupler"    ] = evt.entriesInNtuple;

  // Move to this plotter's directory, then save each file to the directory or overwrite.
    cout << "   TemplateExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->cd();
    for(const auto& hist : h) hist.second->Write("", TObject::kOverwrite);
    cout << "TemplateExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;
}

