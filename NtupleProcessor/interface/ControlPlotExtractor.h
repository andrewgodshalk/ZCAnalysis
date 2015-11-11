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
    ControlPlotExtractor(EventHandler&, TString o = "CSVT,JetPt");
    virtual ~ControlPlotExtractor(){}

    void fillHistos();     // Fills the class' contained histograms from the passed Entry.
    void saveToFile();     // Saves the histograms and the log to file.

    ControlPlotConfig cfg;

    bool    usingDY;
    bool    usingZee;
    bool    usingZuu;
    TString csvOpPt;
    TString diffVariable;

    count nEventsProcessed;
    count nZPJEvents      ;
    count nZHFEvents      ;
    count nValidEvents    ;
    count nValidBEvents   ;
    count nValidCEvents   ;
    count nValidLEvents   ;

  // Counter for cut flow table.
    std::map<TString, count> nEvents;
};

#endif
