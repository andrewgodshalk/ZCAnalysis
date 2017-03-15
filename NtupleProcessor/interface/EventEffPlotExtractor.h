#ifndef GUARD_EventEffPlotExtractor_h
#define GUARD_EventEffPlotExtractor_h

/*------------------------------------------------------------------------------
   EffPloExtractor Class

 Created : 2016-02-17  godshalk
 Modified: 2016-02-17  godshalk

 Class inheritted from HistogramExtractor, designed to extract efficiency
 histograms from Step2 Ntuples.


------------------------------------------------------------------------------*/

#include <map>
#include <TString.h>
#include "HistogramExtractor.h"
#include "EventHandler.h"

typedef unsigned long count;

class EventEffPlotExtractor : public HistogramExtractor {
public:
    EventEffPlotExtractor(EventHandler&, TDirectory*, TString o = "");
    virtual ~EventEffPlotExtractor(){}
    void  fillHistos();       // Fills the class' contained histograms from the passed Entry.

private:
    void saveToFile(); // Saves the histograms and the log to file.

  // Labels for split histograms
    static const std::vector<TString> HFTags;
    static const std::vector<TString> SVTypes;

    TString csvOpPt       ;
    TString diffVariable  ;

  // Options
    bool usingDY         ;
    bool usingSim        ;
    bool selectingZee    ;
    bool selectingZuu    ;
    bool selectingZl     ;
    bool selectingZc     ;
    bool selectingZb     ;

  // Counter for cut flow table.
    std::map<TString, count> nEvents;
    float evtWeight;
};

#endif
