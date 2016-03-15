#ifndef GUARD_TemplateExtractor_h
#define GUARD_TemplateExtractor_h

/*------------------------------------------------------------------------------
   TemplateExtractor Class

 Created : 2015-11-09  godshalk
 Modified: 2015-11-09  godshalk

 Class designed to extract control plots from ntuples for Z+c analysis.


------------------------------------------------------------------------------*/

#include <map>
#include <TString.h>
#include "HistogramExtractor.h"
#include "EventHandler.h"
#include "ControlPlotConfig.h"

typedef unsigned long count;

class TemplateExtractor : public HistogramExtractor {
public:
    TemplateExtractor(EventHandler&, TDirectory*, TString o = "");
    virtual ~TemplateExtractor(){}

    void fillHistos();       // Fills the class' contained histograms from the passed Entry.

private:
    void saveToFile(); // Saves the histograms and the log to file.

  // Labels for split histograms
    static const std::vector<TString> HFTags;

    TString csvOpPt       ;
    TString diffVariable  ;
    TString tempPrefix    ;

  // Options
    bool usingDY         ;
    bool usingSim        ;
    bool selectingZee    ;
    bool selectingZuu    ;
    bool selectingZl     ;
    bool selectingZc     ;
    bool selectingZb     ;
    bool selectingZtautau;

  // Counter for cut flow table.
    std::map<TString, count> nEvents;

    float evtWt;

};



#endif
