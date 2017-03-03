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

private:
    void saveToFile(); // Saves the histograms and the log to file.
    void makePhysicsObjectHistograms(TString, TString);                             // Makes a set of histograms for all objects using inputs: histogram name prefix, label modification
    void makePhysicsObjectHistograms(TString, TString, TString, bool split=false);  // Makes a set of histograms using inputs: object type, histogram name prefix, label modification, split by object option.
    void fillEventHistograms    (TString, float wt=1.0, bool split=true);
    void fillMuonHistograms     (TString, float wt=1.0, bool split=true);
    void fillElectronHistograms (TString, float wt=1.0, bool split=true);
    void fillDileptonHistograms (TString, float wt=1.0);  // Fills all histograms for the object that match the given prefix.
    void fillMETHistograms      (TString, float wt=1.0);
    void fillJetHistograms      (TString, float wt=1.0);
    void fillAllObjectHistograms(TString, float wt=1.0);

    static ControlPlotConfig cfg;

    std::map<TString, TDirectory*> subDirMap;   // Map of all subdirectories of hDir containing histograms.

  // Labels for split histograms
    static const std::vector<TString> multName ;
    static const std::vector<TString> multTitle;
    static const std::vector<TString> HFTags;
    static const std::vector<TString> SVType;
    //static const vector<TString> numLabel = {"_1st", "_2nd", "_3rd", "_4th", "_5th", "_6th", "_7th", "_8th", "_9th"};

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

};

#endif
