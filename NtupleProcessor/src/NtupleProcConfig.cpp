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
    fn_ntuple["dy1j_1of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_1of7"        );
    fn_ntuple["dy1j_2of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_2of7"        );
    fn_ntuple["dy1j_3of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_3of7"        );
    fn_ntuple["dy1j_4of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_4of7"        );
    fn_ntuple["dy1j_5of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_5of7"        );
    fn_ntuple["dy1j_6of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_6of7"        );
    fn_ntuple["dy1j_7of7"        ] = pt.get<string>("FILE LOCATIONS.dy1j_7of7"        );
    fn_ntuple["dy"               ] = pt.get<string>("FILE LOCATIONS.dy"               );
    fn_ntuple["st_s"             ] = pt.get<string>("FILE LOCATIONS.st_s"             );
    fn_ntuple["st_t_4f_lep"      ] = pt.get<string>("FILE LOCATIONS.st_t_4f_lep"      );
    fn_ntuple["st_t_5f_lep"      ] = pt.get<string>("FILE LOCATIONS.st_t_5f_lep"      );
    fn_ntuple["stbar_t_4f_lep"   ] = pt.get<string>("FILE LOCATIONS.stbar_t_4f_lep"   );
    fn_ntuple["stbar_t_5f_lep"   ] = pt.get<string>("FILE LOCATIONS.stbar_t_5f_lep"   );
    fn_ntuple["stbar_tw_5f_nohad"] = pt.get<string>("FILE LOCATIONS.stbar_tw_5f_nohad");
    fn_ntuple["stbar_tw_5f"      ] = pt.get<string>("FILE LOCATIONS.stbar_tw_5f"      );
    fn_ntuple["st_tw_5f_nohad"   ] = pt.get<string>("FILE LOCATIONS.st_tw_5f_nohad"   );
    fn_ntuple["st_tw_5f"         ] = pt.get<string>("FILE LOCATIONS.st_tw_5f"         );
    fn_ntuple["elec_c"           ] = pt.get<string>("FILE LOCATIONS.elec_c"           );
    fn_ntuple["elec_d_1of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_1of8"      );
    fn_ntuple["elec_d_2of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_2of8"      );
    fn_ntuple["elec_d_3of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_3of8"      );
    fn_ntuple["elec_d_4of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_4of8"      );
    fn_ntuple["elec_d_5of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_5of8"      );
    fn_ntuple["elec_d_6of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_6of8"      );
    fn_ntuple["elec_d_7of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_7of8"      );
    fn_ntuple["elec_d_8of8"      ] = pt.get<string>("FILE LOCATIONS.elec_d_8of8"      );
    fn_ntuple["muon_c"           ] = pt.get<string>("FILE LOCATIONS.muon_c"           );
    fn_ntuple["muon_d_1of6"      ] = pt.get<string>("FILE LOCATIONS.muon_d_1of6"      );
    fn_ntuple["muon_d_2of6"      ] = pt.get<string>("FILE LOCATIONS.muon_d_2of6"      );
    fn_ntuple["muon_d_3of6"      ] = pt.get<string>("FILE LOCATIONS.muon_d_3of6"      );
    fn_ntuple["muon_d_4of6"      ] = pt.get<string>("FILE LOCATIONS.muon_d_4of6"      );
    fn_ntuple["muon_d_5of6"      ] = pt.get<string>("FILE LOCATIONS.muon_d_5of6"      );
    fn_ntuple["muon_d_6of6"      ] = pt.get<string>("FILE LOCATIONS.muon_d_6of6"      );
    fn_ntuple["tt_lep_1of2"      ] = pt.get<string>("FILE LOCATIONS.tt_lep_1of2"      );
    fn_ntuple["tt_lep_2of2"      ] = pt.get<string>("FILE LOCATIONS.tt_lep_2of2"      );
    fn_ntuple["tt_1of5"          ] = pt.get<string>("FILE LOCATIONS.tt_1of5"          );
    fn_ntuple["tt_2of5"          ] = pt.get<string>("FILE LOCATIONS.tt_2of5"          );
    fn_ntuple["tt_3of5"          ] = pt.get<string>("FILE LOCATIONS.tt_3of5"          );
    fn_ntuple["tt_4of5"          ] = pt.get<string>("FILE LOCATIONS.tt_4of5"          );
    fn_ntuple["tt_5of5"          ] = pt.get<string>("FILE LOCATIONS.tt_5of5"          );
    fn_ntuple["ww"               ] = pt.get<string>("FILE LOCATIONS.ww"               );
    fn_ntuple["wzz"              ] = pt.get<string>("FILE LOCATIONS.wzz"              );
    fn_ntuple["wz"               ] = pt.get<string>("FILE LOCATIONS.wz"               );
    fn_ntuple["zz"               ] = pt.get<string>("FILE LOCATIONS.zz"               );
    fn_output = pt.get<string>("FILE LOCATIONS.output");

    maxNumJets  = pt.get<int>("RUNNING VARIABLES.max_ntuple_jets" );
    maxNumMuons = pt.get<int>("RUNNING VARIABLES.max_ntuple_muons");
    maxNumElecs = pt.get<int>("RUNNING VARIABLES.max_ntuple_elecs");

  // Extract HistogramExtractors
    boost::property_tree::ptree extList = pt.get_child("HISTOGRAM EXTRACTORS");
    for( const auto& kv : extList) hExtractorStrs.push_back(kv.second.data());

}
