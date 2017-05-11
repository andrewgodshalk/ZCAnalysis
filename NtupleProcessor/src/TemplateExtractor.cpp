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

// const vector<TString> TemplateExtractor::HFTags  = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS"};
// const vector<TString> TemplateExtractor::SVTypes = {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
const vector<TString> TemplateExtractor::HFTags  = {"CSVM", "CSVT"};
const vector<TString> TemplateExtractor::SVTypes = {"noSV", "pfSV", "pfISV", "cISV"};

TemplateExtractor::TemplateExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o), tempPrefix("template_")
{
    cout << "    TemplateExtractor: Created.\n"
            "      Options: " << options
         << endl;

    nEvents["Entries Analyzed"           ] = 0;
    nEvents["Valid Z+j Event w/ MET cut" ] = 0;
    for(    const TString& hfLabel : TemplateExtractor::HFTags )
        for(const TString& svLabel : TemplateExtractor::SVTypes)
            nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+","+svLabel+")"] = 0;
    // cout <<  "   HELLO! " << endl;

  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    usingDY          = (options.Contains("DY"        , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);
    selectingZl      = (options.Contains("Zl"        , TString::kIgnoreCase) ? true : false);
    selectingZc      = (options.Contains("Zc"        , TString::kIgnoreCase) ? true : false);
    selectingZb      = (options.Contains("Zb"        , TString::kIgnoreCase) ? true : false);
    selectingZtautau = (options.Contains("Ztautau"   , TString::kIgnoreCase) ? true : false);

  // Create template histograms (one for each HF operating point and SV type)
    hDir->cd();
    for(    const TString& svLabel : TemplateExtractor::SVTypes)
        for(const TString& hfLabel : TemplateExtractor::HFTags )
    {
        TString histLabel_msv = tempPrefix+svLabel+"_"+hfLabel;
        // cout << "    TemplateExtractor -> Creating histogram: " << histLabel_msv << endl;
        // Hard code in all the options, because bleh.
        if(svLabel == "noSV" ) h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 200, 0, 10);
        if(svLabel == "oldSV") h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 200, 0, 10);
        if(svLabel == "pfSV" ) h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 200, 0, 10);
        if(svLabel == "pfISV") h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 200, 0, 10);
        if(svLabel == "qcSV" ) h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 200, 0, 10);
        if(svLabel == "cISV" ) h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 400, 0, 20);
        if(svLabel == "cISVf") h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 400, 0, 20);
        if(svLabel == "cISVp") h[histLabel_msv] = new TH1F(histLabel_msv, "Jet M_{SV} ("+svLabel+","+hfLabel+"); M_{SV} (GeV); Events/Bin", 400, 0, 20);
        h[histLabel_msv]->Sumw2();
        // hDir->WriteTObject(h[histLabel_msv], 0, "Overwrite");
    }
    // cout << "    TemplateExtractor -> Setup Complete." << endl;
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

    for(    const TString& svLabel : TemplateExtractor::SVTypes)
        for(const TString& hfLabel : TemplateExtractor::HFTags )
    {
      // Select for Z events with at least one HF jet.
        if(!evt.hasHFJets[hfLabel][svLabel]) continue;
        nEvents[TString("Valid Z+HF Event w/ MET cut (")+hfLabel+","+svLabel+")"]++;
      // Fill templates!!
        // h[tempPrefix+"_"+svLabel+"_"+hfLabel]->Fill(evt.m_jet_msv[evt.leadHFJet[hfLabel]]);
        h[tempPrefix+svLabel+"_"+hfLabel]->Fill(
            evt.SVVariable[svLabel][evt.leadHFJet[hfLabel][svLabel]],        // Fill using the appropriately specified variable for this SV
            evtWeight * evt.jetTagEvtWeight[hfLabel.Data()][svLabel.Data()]["central"]  // Fill weighted by event and btag/sv SFs.
        );
    }
}

void TemplateExtractor::saveToFile()
{

  // Last minute counter.
    nEvents["Candidates from Ntupler"] = evt.entriesInNtuple;

  // Move to this plotter's directory, then save each file to the directory or overwrite.
    cout << "   TemplateExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->cd();
    hDir->Write();
    // for(const auto& hist : h) hist.second->Write("", TObject::kOverwrite);
    cout << "TemplateExtractor Cut Table\n"
            "------------------------------\n";
    for( auto& n : nEvents ) cout << "   " << setw(40) << n.first << "  " << n.second << "\n";
    cout << endl;
}
