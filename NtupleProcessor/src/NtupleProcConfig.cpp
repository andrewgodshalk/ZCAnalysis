/*------------------------------------------------------------------------------
NtupleProcConfig.cpp

 Created : 2015-09-15  godshalk
 Modified: 2016-09-08  godshalk

 2016-09-09 - Added LeptonSF classes, which input Lepton SFs from json files.
------------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <TString.h>
#include <TRegexp.h>
#include "../interface/NtupleProcConfig.h"

using std::cout;     using std::endl;   using std::stringstream;
using std::string;   using std::pair;   using std::vector;

NtupleProcConfig::NtupleProcConfig(TString fnc)
: ConfigReader(fnc)
{
  // TEST output
    cout << "    NtupleProcConfig: Created.\n"
            "      Config Input File: " << fn_config << "\n"
         << endl;

  // Open and read in config file
    fn_output = pt.get<string>("FILE LOCATIONS.output");

    maxNumJets  = pt.get<int>("RUNNING VARIABLES.max_ntuple_jets" );
    maxNumMuons = pt.get<int>("RUNNING VARIABLES.max_ntuple_muons");
    maxNumElecs = pt.get<int>("RUNNING VARIABLES.max_ntuple_elecs");

  // Extract HistogramExtractors
    boost::property_tree::ptree extList = pt.get_child("HISTOGRAM EXTRACTORS");
    for( const auto& kv : extList) hExtractorStrs.push_back(kv.second.data());

}
