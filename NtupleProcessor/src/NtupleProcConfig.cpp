/*------------------------------------------------------------------------------
NtupleProcConfig.cpp

 Created : 2015-09-15  godshalk
 Modified: 2015-11-09  godshalk

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
    fn_ntuple["muon"   ] = pt.get<string>("FILE LOCATIONS.muon"   );
    fn_ntuple["elec"   ] = pt.get<string>("FILE LOCATIONS.elec"   );
    fn_ntuple["dy"     ] = pt.get<string>("FILE LOCATIONS.dy"     );
    fn_ntuple["dy1j"   ] = pt.get<string>("FILE LOCATIONS.dy1j"   );
    fn_ntuple["ww"     ] = pt.get<string>("FILE LOCATIONS.ww"     );
    fn_ntuple["wz"     ] = pt.get<string>("FILE LOCATIONS.wz"     );
    fn_ntuple["zz"     ] = pt.get<string>("FILE LOCATIONS.zz"     );
    fn_ntuple["ttlep"  ] = pt.get<string>("FILE LOCATIONS.ttlep"  );
    fn_ntuple["ttsemi" ] = pt.get<string>("FILE LOCATIONS.ttsemi" );
    fn_ntuple["tthad"  ] = pt.get<string>("FILE LOCATIONS.tthad"  );
    fn_ntuple["t_s"    ] = pt.get<string>("FILE LOCATIONS.t_s"    );
    fn_ntuple["t_t"    ] = pt.get<string>("FILE LOCATIONS.t_t"    );
    fn_ntuple["t_tw"   ] = pt.get<string>("FILE LOCATIONS.t_tw"   );
    fn_ntuple["tbar_s" ] = pt.get<string>("FILE LOCATIONS.tbar_s" );
    fn_ntuple["tbar_t" ] = pt.get<string>("FILE LOCATIONS.tbar_t" );
    fn_ntuple["tbar_tw"] = pt.get<string>("FILE LOCATIONS.tbar_tw");
    fn_output = pt.get<string>("FILE LOCATIONS.output");

    maxNumJets  = pt.get<int>("RUNNING VARIABLES.max_ntuple_jets" );
    maxNumMuons = pt.get<int>("RUNNING VARIABLES.max_ntuple_muons");
    maxNumElecs = pt.get<int>("RUNNING VARIABLES.max_ntuple_elecs");

  // Extract HistogramExtractors
    boost::property_tree::ptree extList = pt.get_child("HISTOGRAM EXTRACTORS");
    for( const auto& kv : extList) hExtractorStrs.push_back(kv.second.data());

}
