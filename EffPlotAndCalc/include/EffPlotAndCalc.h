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

    std::map<char, TH2F*>  h_nJets;   // h_nJets[Flavor]
    std::map<char, TH2F*> hr_nJets;   // h_nJets[Flavor]
    std::map<char, std::map<TString,TH2F*> >  h_nTaggedJets;  // h_nTaggedJets[Flavor][Tag]
    std::map<char, std::map<TString,TH2F*> > hr_nTaggedJets;  // h_nTaggedJets[Flavor][Tag]
    std::map<char, std::map<TString,TH2F*> >  h_JetTagEff  ;  // h_JetTagEff[Flavor][Tag]
    TFile* f_input_;
    TFile* f_output_;
    std::string fn_input_;
    std::string fn_output_;
    std::string options_;
    TH2F* h_temp_;    // Template for 2D efficiency histograms

};
