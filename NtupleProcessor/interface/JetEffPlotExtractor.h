#ifndef GUARD_JetEffPlotExtractor_h
#define GUARD_JetEffPlotExtractor_h

/*------------------------------------------------------------------------------
   JetEffPloExtractor Class

 Created : 2016-02-17  godshalk
 Modified: 2017-03-15  godshalk

 Class inheritted from HistogramExtractor, designed to extract efficiency
 histograms from Step2 Ntuples.

 2017 - Refactored into separate classes for Jet and Event eff.


------------------------------------------------------------------------------*/

#include <map>
#include <TString.h>
#include "HistogramExtractor.h"
#include "EventHandler.h"

typedef unsigned long count;

class JetEffPlotExtractor : public HistogramExtractor {
public:
    JetEffPlotExtractor(EventHandler&, TDirectory*, TString o = "");
    virtual ~JetEffPlotExtractor(){}
    void  fillHistos();       // Fills the class' contained histograms from the passed Entry.

private:
    void saveToFile(); // Saves the histograms and the log to file.

  // Labels for split histograms
    static const std::vector<TString> HFTags;
    static const std::vector<TString> SVTypes;
    static const std::vector<TString> selectionLabels;
    std::vector<char> flavorLabels;
    TString csvOpPt       ;
    TString diffVariable  ;
    float evtWt;

  // Options
    bool usingSim        ;
    bool selectingZee    ;
    bool selectingZuu    ;

  // Counter for cut flow table.
    std::map<TString, count> nEvents;
    std::map<TString, count> nJets;

};

#endif
