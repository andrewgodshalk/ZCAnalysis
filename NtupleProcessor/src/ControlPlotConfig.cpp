/*------------------------------------------------------------------------------
ControlPlotConfig.cpp

 Created : 2015-09-15  godshalk
 Modified: 2015-11-06  godshalk

------------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <TString.h>
#include <TRegexp.h>
#include "../interface/ControlPlotConfig.h"

using std::cout;     using std::endl;   using std::stringstream;
using std::string;   using std::pair;   using std::vector;

ControlPlotConfig::ControlPlotConfig(TString fnc)
: ConfigReader(fnc)
{
    cout << "    ControlPlotConfig: Created.\n"
            "      Config Input File: " << fn_config << "\n"
         << endl;

  // Read in histogram strings from config file.
    boost::property_tree::ptree extList = pt.get_child("CONTROL PLOTS");
    for( const auto& kv : extList)
    {
        string hName(kv.first);
        string hData(kv.second.data());
        h_strings[hName] = vector<std::string>();
        getListFromString(hData, h_strings[hName]);
    }

}
