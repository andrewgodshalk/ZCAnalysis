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
/*
fn_ntuple["dy_01of20"   ] = pt.get<string>("FILE LOCATIONS.dy_01of20"   );
fn_ntuple["dy_02of20"   ] = pt.get<string>("FILE LOCATIONS.dy_02of20"   );
fn_ntuple["dy_03of20"   ] = pt.get<string>("FILE LOCATIONS.dy_03of20"   );
fn_ntuple["dy_04of20"   ] = pt.get<string>("FILE LOCATIONS.dy_04of20"   );
fn_ntuple["dy_05of20"   ] = pt.get<string>("FILE LOCATIONS.dy_05of20"   );
fn_ntuple["dy_06of20"   ] = pt.get<string>("FILE LOCATIONS.dy_06of20"   );
fn_ntuple["dy_07of20"   ] = pt.get<string>("FILE LOCATIONS.dy_07of20"   );
fn_ntuple["dy_08of20"   ] = pt.get<string>("FILE LOCATIONS.dy_08of20"   );
fn_ntuple["dy_09of20"   ] = pt.get<string>("FILE LOCATIONS.dy_09of20"   );
fn_ntuple["dy_10of20"   ] = pt.get<string>("FILE LOCATIONS.dy_10of20"   );
fn_ntuple["dy_11of20"   ] = pt.get<string>("FILE LOCATIONS.dy_11of20"   );
fn_ntuple["dy_12of20"   ] = pt.get<string>("FILE LOCATIONS.dy_12of20"   );
fn_ntuple["dy_13of20"   ] = pt.get<string>("FILE LOCATIONS.dy_13of20"   );
fn_ntuple["dy_14of20"   ] = pt.get<string>("FILE LOCATIONS.dy_14of20"   );
fn_ntuple["dy_15of20"   ] = pt.get<string>("FILE LOCATIONS.dy_15of20"   );
fn_ntuple["dy_16of20"   ] = pt.get<string>("FILE LOCATIONS.dy_16of20"   );
fn_ntuple["dy_17of20"   ] = pt.get<string>("FILE LOCATIONS.dy_17of20"   );
fn_ntuple["dy_18of20"   ] = pt.get<string>("FILE LOCATIONS.dy_18of20"   );
fn_ntuple["dy_19of20"   ] = pt.get<string>("FILE LOCATIONS.dy_19of20"   );
fn_ntuple["dy_20of20"   ] = pt.get<string>("FILE LOCATIONS.dy_20of20"   );
fn_ntuple["elecBv1"     ] = pt.get<string>("FILE LOCATIONS.elecBv1"     );
fn_ntuple["elecBv2_1of5"] = pt.get<string>("FILE LOCATIONS.elecBv2_1of5");
fn_ntuple["elecBv2_2of5"] = pt.get<string>("FILE LOCATIONS.elecBv2_2of5");
fn_ntuple["elecBv2_3of5"] = pt.get<string>("FILE LOCATIONS.elecBv2_3of5");
fn_ntuple["elecBv2_4of5"] = pt.get<string>("FILE LOCATIONS.elecBv2_4of5");
fn_ntuple["elecBv2_5of5"] = pt.get<string>("FILE LOCATIONS.elecBv2_5of5");
fn_ntuple["elecC_1of2"  ] = pt.get<string>("FILE LOCATIONS.elecC_1of2"  );
fn_ntuple["elecC_2of2"  ] = pt.get<string>("FILE LOCATIONS.elecC_2of2"  );
fn_ntuple["elecD_1of2"  ] = pt.get<string>("FILE LOCATIONS.elecD_1of2"  );
fn_ntuple["elecD_2of2"  ] = pt.get<string>("FILE LOCATIONS.elecD_2of2"  );
fn_ntuple["elecE_1of2"  ] = pt.get<string>("FILE LOCATIONS.elecE_1of2"  );
fn_ntuple["elecE_2of2"  ] = pt.get<string>("FILE LOCATIONS.elecE_2of2"  );
fn_ntuple["elecF"       ] = pt.get<string>("FILE LOCATIONS.elecF"       );
fn_ntuple["elecG_1of3"  ] = pt.get<string>("FILE LOCATIONS.elecG_1of3"  );
fn_ntuple["elecG_2of3"  ] = pt.get<string>("FILE LOCATIONS.elecG_2of3"  );
fn_ntuple["elecG_3of3"  ] = pt.get<string>("FILE LOCATIONS.elecG_3of3"  );
fn_ntuple["muonBv1"     ] = pt.get<string>("FILE LOCATIONS.muonBv1"     );
fn_ntuple["muonBv2_1of5"] = pt.get<string>("FILE LOCATIONS.muonBv2_1of5");
fn_ntuple["muonBv2_2of5"] = pt.get<string>("FILE LOCATIONS.muonBv2_2of5");
fn_ntuple["muonBv2_3of5"] = pt.get<string>("FILE LOCATIONS.muonBv2_3of5");
fn_ntuple["muonBv2_4of5"] = pt.get<string>("FILE LOCATIONS.muonBv2_4of5");
fn_ntuple["muonBv2_5of5"] = pt.get<string>("FILE LOCATIONS.muonBv2_5of5");
fn_ntuple["muonC_1of2"  ] = pt.get<string>("FILE LOCATIONS.muonC_1of2"  );
fn_ntuple["muonC_2of2"  ] = pt.get<string>("FILE LOCATIONS.muonC_2of2"  );
fn_ntuple["muonD_1of3"  ] = pt.get<string>("FILE LOCATIONS.muonD_1of3"  );
fn_ntuple["muonD_2of3"  ] = pt.get<string>("FILE LOCATIONS.muonD_2of3"  );
fn_ntuple["muonD_3of3"  ] = pt.get<string>("FILE LOCATIONS.muonD_3of3"  );
fn_ntuple["muonE_1of3"  ] = pt.get<string>("FILE LOCATIONS.muonE_1of3"  );
fn_ntuple["muonE_2of3"  ] = pt.get<string>("FILE LOCATIONS.muonE_2of3"  );
fn_ntuple["muonE_3of3"  ] = pt.get<string>("FILE LOCATIONS.muonE_3of3"  );
fn_ntuple["muonF_1of2"  ] = pt.get<string>("FILE LOCATIONS.muonF_1of2"  );
fn_ntuple["muonF_2of2"  ] = pt.get<string>("FILE LOCATIONS.muonF_2of2"  );
fn_ntuple["muonG_1of4"  ] = pt.get<string>("FILE LOCATIONS.muonG_1of4"  );
fn_ntuple["muonG_2of4"  ] = pt.get<string>("FILE LOCATIONS.muonG_2of4"  );
fn_ntuple["muonG_3of4"  ] = pt.get<string>("FILE LOCATIONS.muonG_3of4"  );
fn_ntuple["muonG_4of4"  ] = pt.get<string>("FILE LOCATIONS.muonG_4of4"  );
fn_ntuple["tt"          ] = pt.get<string>("FILE LOCATIONS.tt"          );
fn_ntuple["ww"          ] = pt.get<string>("FILE LOCATIONS.ww"          );
fn_ntuple["wz"          ] = pt.get<string>("FILE LOCATIONS.wz"          );
fn_ntuple["zz"          ] = pt.get<string>("FILE LOCATIONS.zz"          );
*/
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
fn_ntuple["muonC"         ] = pt.get<string>("FILE LOCATIONS.muonC"         );
fn_ntuple["muonD"         ] = pt.get<string>("FILE LOCATIONS.muonD"         );
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
