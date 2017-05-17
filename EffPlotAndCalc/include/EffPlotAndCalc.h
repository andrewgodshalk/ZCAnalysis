#ifndef GUARD_EffPlotAndCalc_h
#define GUARD_EffPlotAndCalc_h

/*------------------------------------------------------------------------------

EffPlotAndCalc

Class that takes an input of the EfficiencyPLotExtractor output (in the
NtupleProcessor Library) and uses the extracted plots to calculate a few various
efficiency measurements.

2017-02-02 - Currently working on calculating the differential efficiency of
tagging individual jets with respect to input pT and eta.

Current input: NP output file with raw_eff_plots added (created with code from
after 2017-02.)


------------------------------------------------------------------------------*/

#include <map>
#include <string>
#include "TFile.h"
#include "TH1.h"
#include "TH2F.h"
#include "TString.h"

class EffPlotAndCalc
{
public:
    EffPlotAndCalc(int argc=0, char* argv[]={});
    ~EffPlotAndCalc();

private:
    bool  processCommandLineInput(int argc=0, char* argv[]={});
    TH1*  getPlotFromFile(const TFile*, const char*);
    void  transferContents(TH2F*, TH2F*);
    TH2F* makeEffPlots(TString, TH2F*, TH2F*);
    void  printPlotValues(TH2F*);

    int    nPtBins_;
    int    nEtaBins_;
    double ptBinBounds_[];
    double etaBinBounds_[];

    TString numeratorPlotNameFormat    ;
    TString denominatorPlotNameFormat  ;
    TString numeratorPlotSourceFormat  ;
    TString denominatorPlotSourceFormat;
    TString efficiencyPlotNameFormat   ;
    std::map<TString, TH2F*> hr_nJets;   // Storage for plot from root file, with original binning.
    std::map<TString, TH2F*>  h_nJets;   // Storage for new # of jet plots, with new binning.
    std::map<TString, TH2F*>  h_nTaggedJets;  // Storage for plot from root file, with original binning.
    std::map<TString, TH2F*> hr_nTaggedJets;  // Storage for new # of jet plots, with new binning.
    std::map<TString, TH2F*>  h_JetTagEff  ;  // Final efficiency plot
    std::map<TString, TH1D*>  h_JetTagEff1D;  // Final efficiency plot
    TFile* f_input_;
    TFile* f_output_;
    std::string fn_input_;
    std::string fn_output_;
    std::string options_;
    TH2F* h_temp_;    // Template for 2D efficiency histograms

};


#endif
