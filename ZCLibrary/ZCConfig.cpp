/*------------------------------------------------------------------------------
ZCConfig.cpp

 Created : 2015-09-15  godshalk
 Modified: 2015-09-15  godshalk

------------------------------------------------------------------------------*/

#include <iostream>
#include <TString.h>
#include "ZCConfig.h"

using std::cout;   using std::endl;

ZCConfig::ZCConfig(TString fnc)
: fn_config(fnc)
{
  // TEST output
    cout << "    ZCConfig: Created.\n"
            "      Config Input File: " << fn_config << "\n"
         << endl;

  // Open config file.
  // Process input
  // Close config file.
  // Complete processing on config file variables.


}