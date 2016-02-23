#ifndef GUARD_EffPlotExtractor_h
#define GUARD_EffPlotExtractor_h

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

class EffPlotExtractor : public HistogramExtractor {
public:
    EffPlotExtractor(EventHandler&, TDirectory*, TString o = "");
    virtual ~EffPlotExtractor(){}

    void  fillHistos();       // Fills the class' contained histograms from the passed Entry.
    float sf_b_eq   (const TString&, const float&);
    float sf_b_error(const TString&, const float&);

private:
    void saveToFile(); // Saves the histograms and the log to file.

  // Labels for split histograms
    static const std::vector<TString> HFTags;
    //static const vector<TString> numLabel = {"_1st", "_2nd", "_3rd", "_4th", "_5th", "_6th", "_7th", "_8th", "_9th"};
    std::map<TString, std::vector<float> > SFb_errors;
    static const std::vector<float> sf_b_ptmin;
    static const std::vector<float> sf_b_ptmax;

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

    float evtWt;
    float errorFactor;  // Error factor, for syst error calculations.
                        //   For none, 0. For error up(down), use error +1(-1).
    float errorType;    // Specifies way to calculate sf systematic error.
                        //   Set to 1 for bin-by-bin up/down. Set to 2 for "conservative" up/down (highest sf_error bin value)
};

#endif
