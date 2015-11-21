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
            "      Config Input File: " << fn_config
         << endl;

    getHistoStringsFromFileHeading("MUON"    );
    getHistoStringsFromFileHeading("ELECTRON");
    getHistoStringsFromFileHeading("DILEPTON");
    getHistoStringsFromFileHeading("MET"     );
    getHistoStringsFromFileHeading("JET"     );

}


void ControlPlotConfig::getHistoStringsFromFileHeading(TString poHeading)
{ // Read in histogram strings from config file from given heading.
  // Set up space for the set of split histogram strings within the map, under the given heading, and associate it with a pointer.
    vector<vector<string> >* h_strings = &(h_histoStrings[poHeading] = vector<vector<string> >());
  // Find the set for this object within the config file input.
    boost::property_tree::ptree extList = pt.get_child(poHeading.Data());
  // Go through the set and map into vectors
    for( const auto& kv : extList)
    {
        h_strings->push_back(vector<string>(0));
        string hData = string(kv.first) + " " + kv.second.data();
        getListFromString(hData, h_strings->back());
    }
}
