/*
  ControlPlotExtractor.cpp
 
   Created on: May 19, 2015
       Author: godshalk
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include "../interface/ControlPlotExtractor.h"
//#include "../../ZCLibrary/timestamp.h"

using std::stringstream;   using std::string;     using std::cout;
using std::endl;           using std::ofstream;
typedef unsigned int Index;

ControlPlotExtractor::ControlPlotExtractor(EventHandler& eh, TString o) : HistogramExtractor(eh, o){}


void ControlPlotExtractor::fillHistos()
{
/*

//cout << "    Processing DiffXSHistoMaker for output " << fn_output << "...";

  // Check if the event is a valid Z+j event, then if there are HF jets of the desired operating point. Kick if not true.
    nEventsProcessed++;

  // Kill if event isn't a Z+j event, or isn't of the intended type (Zee or Zuu)
    if(!e->validZPJEvent)             { cout << "complete. Killed because !validZPJEvent." << endl;   return ""; }
    if(usingZee && !e->validZeeEvent) { cout << "complete. Killed because !validZeeEvent." << endl;   return ""; }
    if(usingZuu && !e->validZuuEvent) { cout << "complete. Killed because !validZuuEvent." << endl;   return ""; }
    nZPJEvents++;
  // Kill if event isn't a HF of the desired operating point.
    if(!e->hasHFJets[csvOpPt]) { cout << "complete. No HF Jets." << endl;   return ""; }
    nZHFEvents++;

    stringstream tempLog("");

  // Set up a pointer to the variable we'd like to fill with.
    float* tempPt;
    if(diffVariable == "ZPt"   ) tempPt = &(e->m_Z_pt   );
    if(diffVariable == "JetPt" ) tempPt =   e->m_jet_pt  ;
    if(diffVariable == "JetEta") tempPt =   e->m_jet_eta ;
    const float *variablePt = tempPt;

  // Determine if the jet to use to fill histograms.
    // If we are working with DY, look for a jet that has both HF-truth (c or b) and a HF tag.
    // If none is found, fill the light jet template with the leading HF jet.
    // If not working with DY, we simply use the leading HF-tagged jet.


    TString histoToFill = "template";
    Index i = 0;    // Index of HF-tagged jet used to fill histograms.
    Index nValidJets = e->validJets.size();

    if(usingDY)
    { // Check if the event is a b, c, or light jet event.
        if(     e->hasBJet) histoToFill += "_bjets";
        else if(e->hasCJet) histoToFill += "_cjets";
        else                histoToFill += "_ljets";
    }
    for(i=0; i<nValidJets; i++)
        if(e->HFJets[csvOpPt][i]) // If the jet is heavy flavor...
            break;                // Call off the search. You've found your leading jet.

  // Modify the index to add to the differential variable based on if diff-variable is based on the leading jet.
    Index diffFillIndex = (diffVariable == "ZPt" ? 0 : i );
    if(usingDY) h["template"]->Fill(*(variablePt+diffFillIndex), e->m_jet_msv[i]);
    h[histoToFill]           ->Fill(*(variablePt+diffFillIndex), e->m_jet_msv[i]);

*/
/*
        for(i=(e->hasBJet||e->hasCJet ? 0:nValidJets); i<nValidJets; i++)  // If we already know there are no b or c jets, skip to the end.
        { // If the jet is heavy flavor...
            if(e->HFJets[csvOpPt][i])
            { // If the jet is either b or c, designate the proper histogram to fill and set the proper jet to use.
                if(e->bMCJets[i]) {histoToFill+="_bjets"; break;}
                if(e->cMCJets[i]) {histoToFill+="_cjets"; break;}
            }
        }
      // If no HF-truth jets were found, fill the ljet template with a jet found in the following loop.
        if(i==nValidJets) histoToFill+="_ljets";
    }

  // If not using DY or no true HF jets were found, fill with the leading HF-tagged jet.
    if(!usingDY || histoToFill=="template_ljets")
        for(i=0; i<nValidJets; i++)
            if(e->HFJets[csvOpPt][i]) // If the jet is heavy flavor...
                break;                // Call off the search. You've found your leading jet.

  // Modify the index to add to the differential variable based on if diff-variable is based on the leading jet.
    Index diffFillIndex = (diffVariable == "ZPt" ? 0 : i );
    h[histoToFill]->Fill(*(variablePt+diffFillIndex), e->m_jet_msv[i]);
    if(usingDY) h["template"]->Fill(*(variablePt+diffFillIndex), e->m_jet_msv[i]);

    //FOR TESTING PURPOSES: tempLog << "    Filling: " << histoToFill << " w/ Valid Jet# " << i << " of " << nValidJets << "\n";
*/
/*
  // Fill counters based on filled histograms
    //cout << "    Filling: " << histoToFill << endl;
    //if(histoToFill=="template"      )
    nValidEvents++;
    if(histoToFill=="template_bjets") nValidBEvents++;
    if(histoToFill=="template_cjets") nValidCEvents++;
    if(histoToFill=="template_ljets") nValidLEvents++;

    //cout << histoToFill << endl;
    //cout << "complete. HF jet logged." << endl;

    //log << tempLog.str();
    return tempLog.str();
*/
}


void ControlPlotExtractor::saveToFile()
{

}
