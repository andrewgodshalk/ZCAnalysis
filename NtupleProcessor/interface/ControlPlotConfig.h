#ifndef GUARD_ControlPlotConfig_h
#define GUARD_ControlPlotConfig_h

/*------------------------------------------------------------------------------
   ControlPlotConfig Class

 Created : 2015-11-09  godshalk
 Modified: 2015-11-18  godshalk

 Child class of ConfigReader.h. Takes a ini-like configuration file with the
 information needed to create control plot histograms.

 Has a set of strings for each physic object needed (jet, muon, electron, MET,
 and dilepton). Each set of string should have information on histogram
 binning and labeling.
 
 Strings are very general. ControlPlotExtractor creates a set of plots for each
 physics object under a set of desired selection conditions (e.g.
 ControlPlotExtractor would want a full set of muon plots before and after
 selection is performed).

------------------------------------------------------------------------------*/

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <TString.h>
#include "../../ZCLibrary/ConfigReader.h"


class ControlPlotConfig : public ConfigReader
{
  public:
    ControlPlotConfig( TString fnc = "NtupleProcessor/etc/zcControlPlotConfig_default.ini");
   ~ControlPlotConfig(){}


    std::map<TString, std::vector<std::vector<std::string> > > h_histoStrings;

  private:
    void getHistoStringsFromFileHeading(TString);
};



#endif
