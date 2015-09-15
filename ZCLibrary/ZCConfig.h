#ifndef GUARD_ZCConfig_h
#define GUARD_ZCConfig_h

/*------------------------------------------------------------------------------
   ZCConfig

 Created : 2015-09-15  godshalk
 Modified: 2015-09-15  godshalk

 Class designed to contain and provide standard variables for other classes
 used for the Z+c analysis.
 
 Takes a ini-like configuration file with the desired variables to be entered.

------------------------------------------------------------------------------*/

#include <TString.h>

class ZCConfig
{
  public:
    ZCConfig( TString fnc = "zc_standard_config.ini");
   ~ZCConfig(){}

  // VARIABLES FOR CONSUMPTION
  // COPY FROM ZCSTANDARDVARIABLES

  private: 
    TString fn_config;  // Location of the input configuration file.

};

#endif