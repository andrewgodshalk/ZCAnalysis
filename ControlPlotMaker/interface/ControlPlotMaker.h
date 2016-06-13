#ifndef GUARD_ControlPlotMaker_h
#define GUARD_ControlPlotMaker_h

/*------------------------------------------------------------------------------
   ControlPlotMaker

 Created : 2015-11-23  godshalk
 Modified: 2015-11-23  godshalk

 Main class of ControlPlotMaker program.

------------------------------------------------------------------------------*/
#include <TH1.h>
#include <TH1F.h>
#include <THStack.h>
#include <TString.h>
#include <vector>
#include "../../ZCLibrary/AnalysisConfig.h"

class ControlPlotMaker
{
  public:
    ControlPlotMaker(  // Primary constructor.
            TString fnac ="NtupleProcessor/etc/zcAnalysisConfig_default.ini",    // Analysis config file. Used mostly weights, colors, etc.
            TString fni  ="zc_analysis.root"       ,    // root file where individual dataset histograms are stored.
            TString fno  ="zc_analysis_cpplot.root",    // root file where 
            TString o    = ""                      ,    // Extra options input with program
            bool log     = true                    ,    // Use log scale on plots?
            bool flv     = true                    ,    // Split DY by flavor?
            bool tau     = true                    ,    // Split DY decaying to tau from other decays?
            bool eps     = true                         // Also output to histograms as eps files
    );
   ~ControlPlotMaker(){}

  private:

    AnalysisConfig anCfg;

    TString fnInput             ;
    TString fnOutput            ;
    TString options             ;
    bool    usingLogScale       ;
    bool    splitDYByFlavor     ;
    bool    splitTauDecayFromDY ;
    bool    outputAsEPS         ;
    bool    usingLegStats       ;

    TFile* inputFile;
    TFile* outputFile;
    std::map<std::string, TDirectory*>  dsDirectory;    // Pointer to directory for dataset.
    std::map<std::string, TH1*>         dsHist;         // Pointers to clones of histograms that we want to stack.
    std::map<std::string, Color_t>      dsColor;
    std::map<std::string, Style_t>      dsStyle;
    std::vector<std::string> bkgdDatasets;
    std::vector<std::string> sigDatasets;        // Lists of sets that are split by category.
    std::vector<std::string> dataDatasets;
    std::vector<std::string> allDatasets;

    void setDatasetDir(std::string, std::string);      // Sets up sources and histograms styles;
    TH1* createStackHisto(std::string&, TString&);   // Sets up a histogram from file given the proper dataset and histogram name.
    TH1F* createRatioPlot(TH1F*, TH1F*);
    TH1F* createSimSumPlot(THStack*);

};

#endif
