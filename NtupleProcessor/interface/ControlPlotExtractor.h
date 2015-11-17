#ifndef GUARD_ControlPlotExtractor_h
#define GUARD_ControlPlotExtractor_h

/*------------------------------------------------------------------------------
   ControlPlotExtractor Class

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

class ControlPlotExtractor : public HistogramExtractor {
public:
    ControlPlotExtractor(EventHandler&, TDirectory*, TString o = "");
    virtual ~ControlPlotExtractor(){}

    void fillHistos();       // Fills the class' contained histograms from the passed Entry.
    void saveToFile(); // Saves the histograms and the log to file.
    void fillLeptonHistograms(TString);
    void fillMETHistograms   (TString);
    void fillJetHistograms   (TString);

    static ControlPlotConfig cfg;

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
    bool selectingZtautau;

  // Counter for cut flow table.
    std::map<TString, count> nEvents;
};

#endif
