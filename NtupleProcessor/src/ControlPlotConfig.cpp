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

  // Open and read in config file
    cout << "\t\t" << pt.get<string>("FILE LOCATIONS.muon") << endl;
    cout << "\t\t" << pt.get<float> ("MUON.muon_pt_min"   ) << endl;
    cout << endl;

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

    setWeight["dy"     ] = pt.get<float>("DATASET WEIGHTS.dy"     );
    setWeight["ww"     ] = pt.get<float>("DATASET WEIGHTS.ww"     );
    setWeight["wz"     ] = pt.get<float>("DATASET WEIGHTS.wz"     );
    setWeight["zz"     ] = pt.get<float>("DATASET WEIGHTS.zz"     );
    setWeight["tthad"  ] = pt.get<float>("DATASET WEIGHTS.tthad"  );
    setWeight["ttlep"  ] = pt.get<float>("DATASET WEIGHTS.ttlep"  );
    setWeight["ttsemi" ] = pt.get<float>("DATASET WEIGHTS.ttsemi" );
    setWeight["t_s"    ] = pt.get<float>("DATASET WEIGHTS.t_s"    );
    setWeight["t_t"    ] = pt.get<float>("DATASET WEIGHTS.t_t"    );
    setWeight["t_tw"   ] = pt.get<float>("DATASET WEIGHTS.t_tw"   );
    setWeight["tbar_s" ] = pt.get<float>("DATASET WEIGHTS.tbar_s" );
    setWeight["tbar_t" ] = pt.get<float>("DATASET WEIGHTS.tbar_t" );
    setWeight["tbar_tw"] = pt.get<float>("DATASET WEIGHTS.tbar_tw");

    minSVT = pt.get<float>("SVT OPERATING POINTS.minSVT");
    noSVT  = pt.get<float>("SVT OPERATING POINTS.noSVT" );

    stdCSVOpPts["NoHF"] = pt.get<float>("HF TAGGING OPERATING POINTS.NoHF");
    stdCSVOpPts["CSVL"] = pt.get<float>("HF TAGGING OPERATING POINTS.CSVL");
    stdCSVOpPts["CSVM"] = pt.get<float>("HF TAGGING OPERATING POINTS.CSVM");
    stdCSVOpPts["CSVT"] = pt.get<float>("HF TAGGING OPERATING POINTS.CSVT");
    stdCSVOpPts["CSVS"] = pt.get<float>("HF TAGGING OPERATING POINTS.CSVS");

    flatHFTagSF["NoHF"      ] = pt.get<float>("CSV SCALE FACTORS.NoHF"      );
    flatHFTagSF["CSVL"      ] = pt.get<float>("CSV SCALE FACTORS.CSVL"      );
    flatHFTagSF["CSVM"      ] = pt.get<float>("CSV SCALE FACTORS.CSVM"      );
    flatHFTagSF["CSVT"      ] = pt.get<float>("CSV SCALE FACTORS.CSVT"      );
    flatHFTagSF["CSVS"      ] = pt.get<float>("CSV SCALE FACTORS.CSVS"      );
    flatHFTagSF["CSVS_ljmis"] = pt.get<float>("CSV SCALE FACTORS.CSVS_ljmis");

    maxNumJets  = pt.get<int>("RUNNING VARIABLES.max_ntuple_jets" );
    maxNumMuons = pt.get<int>("RUNNING VARIABLES.max_ntuple_muons");
    maxNumElecs = pt.get<int>("RUNNING VARIABLES.max_ntuple_elecs");

    muonPtMin  = pt.get<float>("MUON.muon_pt_min" );
    muonEtaMax = pt.get<float>("MUON.muon_eta_max");
    //muon_iso
    //muon_id

    elecPtMin       = pt.get<float>("ELECTRON.elec_pt_min"        );
    elecEtaInnerMax = pt.get<float>("ELECTRON.elec_eta_inner_max" );
    elecEtaOuterMin = pt.get<float>("ELECTRON.elec_eta_outer_min" );
    elecEtaOuterMax = pt.get<float>("ELECTRON.elec_eta_outer_max" );
    //elec_iso
    //elec_id

    dilepInvMassMin = pt.get<float> ("DILEPTON.dimuon_invmass_min");
    dilepInvMassMax = pt.get<float> ("DILEPTON.dimuon_invmass_max");
    dilepMuonCharge = pt.get<string>("DILEPTON.dimuon_charge");
    dilepElecCharge = pt.get<string>("DILEPTON.dielec_charge");

    metMax          = pt.get<float>("MET.met_max");

    nJetsAnalyzed   = pt.get<float>("JET.num_jets_to_analyze");
    jetPtMin        = pt.get<float>("JET.jet_pt_min"         );
    jetEtaMax       = pt.get<float>("JET.jet_eta_max"        );

    jsonSelect      = pt.get<bool>("JSON.use_JSON");

    string triggerStr_muon    = pt.get<string>("TRIGGERS.muon_triggers"                );
    string triggerStr_elec    = pt.get<string>("TRIGGERS.elec_triggers"                );
    string binStr_jet_pt      = pt.get<string>("DIFFERENTIAL ANALYSIS.jet_pt_bins"     );
    string binStr_jet_eta     = pt.get<string>("DIFFERENTIAL ANALYSIS.jet_eta_bins"    );
    string binStr_dilepton_pt = pt.get<string>("DIFFERENTIAL ANALYSIS.dilepton_pt_bins");

  // Complete processing on config file variables.
    processTriggerString( muonTriggers  , triggerStr_muon    );
    processTriggerString( elecTriggers  , triggerStr_elec    );

    processBinString(     jetPtBins     , binStr_jet_pt      );
    processBinString(     jetEtaBins    , binStr_jet_eta     );
    processBinString(     dileptonPtBins, binStr_dilepton_pt );

    cout << "  TEST CONFIG FILE READ COMPLETE!! \n" << endl;

}


void ControlPlotConfig::processBinString(vector<pair<float,float> >& binSet, string& inputString)
{
    binSet.clear();
  // Debug:
    //cout << "  ControlPlotConfig::processBinString:\n  Initial String:     " << inputString << endl;

  // Define a regex expression to use to match strings of (float, float) format and a size variable to pass as the size of the resulting match.
    TRegexp numPairRegex("\\(-?[0-9]+\\.?[0-9]?,-?[0-9]+\\.?[0-9]?\\)");
    int* matchLength = new int;

  // Get list of bin strings from input string.
    vector<string> binStrings;
    getListFromString(inputString, binStrings);

  // Extract number information from list of bin strings (Should have format: (i,j) ).
    for(auto& binStr : binStrings)
    {
      // Check if the bin string is a formatted properly.
        *matchLength = 0;
        if(numPairRegex.Index(binStr, matchLength) != 0 || (unsigned int)(*matchLength) != binStr.length())
        { // If the regex expression doesn't find itself in the string *OR* the matched string isn't the full length of the input string...
            cout << "  ERROR (ControlPlotConfig::processBinString)\n: Bin string does not have (p,q) format: " << binStr << endl << endl;
          // Debug:
            //cout << "    Returned index:  " << numPairRegex.Index(binStr, matchLength) << endl;
            //cout << "    Returned length: " << *matchLength << endl;
            //cout << "    Actual length:   " << binStr.length() << endl << endl;
            return;     // KICK
        }

        float binMin, binMax;
        TString numStr = "";
        string::iterator it=binStr.begin()+1;

      // Extract bin minimum value
        do{ numStr+=*(it++); } while( it != binStr.end() && *it != ',');
        binMin = numStr.Atof();

     // Extract bin maximum value
        numStr = ""; it++;  // Reset bin number string and move past the comma
        do{ numStr+=*(it++); } while( it != binStr.end() && *it != ')');
        binMax = numStr.Atof();

      // Add pair to bin set.
        binSet.push_back({binMin, binMax});
    }
  // Debug:
    //cout << "  Final List of Bins: ";
    //for(auto& bin : binSet) cout << "(" << bin.first << "," << bin.second << ") ";
    //cout << endl << endl;
}


void ControlPlotConfig::processTriggerString(vector<int>& trigSet, string& inputString)
{
    getListFromString<int>(inputString, trigSet);
}
