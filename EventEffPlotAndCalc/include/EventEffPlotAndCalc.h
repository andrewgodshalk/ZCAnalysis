#ifndef GUARD_EventEffPlotAndCalc_h
#define GUARD_EventEffPlotAndCalc_h

/*------------------------------------------------------------------------------

EventEffPlotAndCalc

Class that takes an input of the EfficiencyPlotExtractor output (in the
NtupleProcessor Library) and uses the extracted plots to calculate the per-event
selection efficiency of Z+f events (where f = b,c,l)

Current input: NP output file with raw_eff_plots added (created with code from
after 2017-03-06.)


------------------------------------------------------------------------------*/

#include <map>
#include <string>
#include <utility>
#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"

class EventEffPlotAndCalc
{
public:
    EventEffPlotAndCalc(int argc=0, char* argv[]={});
    ~EventEffPlotAndCalc();

private:
    bool     processCommandLineInput(int argc=0, char* argv[]={});
    TH1*     getPlotFromFile(const TFile*, const char*);
    void     transferContents(TH1F*, TH1F*);
    TH1F*    makeEffPlots(TString, TH1F*, TH1F*);
    void     printPlotValues(TH1F*);
    TCanvas* makeCombinedEffPlots(TString, TString, TH1F*, TH1F*, TH1F*);
    std::pair<double, double> calculateAvgEff(TH1F*, TH1F*);

    int    nPtBins_;
    double ptBinBounds_[41];
    // int    nEtaBins_;
    // double etaBinBounds_[];

    std::map<char, TH1F*> hr_nEvts;       // hr_nEvts[Flavor] Storage for plot from root file, with original binning.
    std::map<char, TH1F*>  h_nEvts;       // h_nEvts [Flavor] Storage for new # of jet plots, with new binning.
    std::map<char, std::map<TString, std::map< TString, TH1F*> > >  h_nTaggedEvts;  // h_nTaggedEvts[Flavor][Tag][SVType]
    std::map<char, std::map<TString, std::map< TString, TH1F*> > > hr_nTaggedEvts;  // h_nTaggedEvts[Flavor][Tag][SVType]
    std::map<char, std::map<TString, std::map< TString, TH1F*> > > h_EvtTagEff   ;  // h_EvtTagEff  [Flavor][Tag][SVType]
    std::map<TString, std::map< TString, TH1F*   > >  h_nTaggedEvts_Data;
    std::map<TString, std::map< TString, TH1F*   > > hr_nTaggedEvts_Data;
    std::map<TString, std::map< TString, TCanvas*> > c_combinedEffPlot;   // Final, combined, beutified efficiency plots.
    std::map<char, std::map<TString, std::map< TString, std::pair<double, double> > > > avgEff;
    TFile* f_input_;
    TFile* f_output_;
    std::string fn_input_;
    std::string fn_output_;
    TH1F* h_temp_;    // Template for 2D efficiency histograms
};


#endif
