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
fn_ntuple["muonBv1"       ] = pt.get<string>("FILE LOCATIONS.muonBv1"       );
fn_ntuple["muonBv2_01of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_01of24");
fn_ntuple["muonBv2_02of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_02of24");
fn_ntuple["muonBv2_03of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_03of24");
fn_ntuple["muonBv2_04of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_04of24");
fn_ntuple["muonBv2_05of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_05of24");
fn_ntuple["muonBv2_06of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_06of24");
fn_ntuple["muonBv2_07of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_07of24");
fn_ntuple["muonBv2_08of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_08of24");
fn_ntuple["muonBv2_09of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_09of24");
fn_ntuple["muonBv2_10of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_10of24");
fn_ntuple["muonBv2_11of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_11of24");
fn_ntuple["muonBv2_12of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_12of24");
fn_ntuple["muonBv2_13of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_13of24");
fn_ntuple["muonBv2_14of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_14of24");
fn_ntuple["muonBv2_15of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_15of24");
fn_ntuple["muonBv2_16of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_16of24");
fn_ntuple["muonBv2_17of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_17of24");
fn_ntuple["muonBv2_18of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_18of24");
fn_ntuple["muonBv2_19of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_19of24");
fn_ntuple["muonBv2_20of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_20of24");
fn_ntuple["muonBv2_21of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_21of24");
fn_ntuple["muonBv2_22of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_22of24");
fn_ntuple["muonBv2_23of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_23of24");
fn_ntuple["muonBv2_24of24"] = pt.get<string>("FILE LOCATIONS.muonBv2_24of24");
fn_ntuple["muonC         "] = pt.get<string>("FILE LOCATIONS.muonC"         );
fn_ntuple["muonD         "] = pt.get<string>("FILE LOCATIONS.muonD"         );
fn_ntuple["dy_inc_amc"    ] = pt.get<string>("FILE LOCATIONS.dy_inc_amc"    );
fn_ntuple["dy_inc_mm"     ] = pt.get<string>("FILE LOCATIONS.dy_inc_mm"     );
fn_ntuple["tt"            ] = pt.get<string>("FILE LOCATIONS.tt"            );
fn_ntuple["ww"            ] = pt.get<string>("FILE LOCATIONS.ww"            );
fn_ntuple["wz"            ] = pt.get<string>("FILE LOCATIONS.wz"            );
fn_ntuple["zz"            ] = pt.get<string>("FILE LOCATIONS.zz"            );

    fn_output = pt.get<string>("FILE LOCATIONS.output");

    maxNumJets  = pt.get<int>("RUNNING VARIABLES.max_ntuple_jets" );
    maxNumMuons = pt.get<int>("RUNNING VARIABLES.max_ntuple_muons");
    maxNumElecs = pt.get<int>("RUNNING VARIABLES.max_ntuple_elecs");

  // Extract HistogramExtractors
    boost::property_tree::ptree extList = pt.get_child("HISTOGRAM EXTRACTORS");
    for( const auto& kv : extList) hExtractorStrs.push_back(kv.second.data());

}
