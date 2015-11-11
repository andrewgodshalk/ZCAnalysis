#ifndef GUARD_ControlPlotConfig_h
#define GUARD_ControlPlotConfig_h

/*------------------------------------------------------------------------------
   ControlPlotConfig Class

 Created : 2015-11-09  godshalk
 Modified: 2015-11-09  godshalk

 Class designed to contain and provide standard variables for Z+c Analysis code.
 
 Child class of ConfigReader.h. Takes a ini-like configuration file with the
 desired variables to be entered.

------------------------------------------------------------------------------*/

#include <map>
#include <utility>
#include <string>
#include <vector>
#include <TString.h>
#include "../../ZCLibrary/ConfigReader.h"


class ControlPlotConfig : public ConfigReader
{
  public:
    ControlPlotConfig( TString fnc = "NtupleProcessor/etc/zcControlPlotConfig_default.ini");
   ~ControlPlotConfig(){}

    std::map<TString, std::vector<std::string> > h_strings;

};



#endif
