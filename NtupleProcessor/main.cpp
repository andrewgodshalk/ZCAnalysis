/*------------------------------------------------------------------------------
   main.cpp

Modified: 2015-09-14

Main file for the NtupleProcessor program.
Handles command line arguements.

------------------------------------------------------------------------------*/

// HEADERS
#include <algorithm>
#include <iostream>                 // stdlib
#include <string>
#include <vector>
#include <TFile.h>                  // ROOT class headers
#include <TString.h>
#include <TTree.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "../ZCLibrary/timestamp.h"
#include "../NtupleProcessor/interface/NtupleProcessor.h"

using std::cout;     using std::endl;
using std::string;   using std::ifstream;
using std::vector;

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  // Record the time main starts processing.
    TString ts_mainBegin  = timeStamp();
    TString fts_mainBegin = fileTimeStamp();

  // Variables needed for Ntuple processor
    string dataset  = "dy";
    string npconfig = "NtupleProcessor/etc/zcNtupleProcConfig_default.ini";
    string anconfig = "ZCLibrary/zcAnalysisConfig_default.ini";
    string options  = "";
    int maxEvents = -1;

  // Set up list of valid datsets
    vector<string> datasetNames = {
      "dy1j_1of7","dy1j_2of7","dy1j_3of7","dy1j_4of7","dy1j_5of7","dy1j_6of7","dy1j_7of7","dy",
      "st_s","st_t_4f_lep","st_t_5f_lep","stbar_t_4f_lep","stbar_t_5f_lep","stbar_tw_5f_nohad","stbar_tw_5f","st_tw_5f_nohad","st_tw_5f",
      "elec_c","elec_d_1of8","elec_d_2of8","elec_d_3of8","elec_d_4of8","elec_d_5of8","elec_d_6of8","elec_d_7of8","elec_d_8of8",
      "muon_c","muon_d_1of6","muon_d_2of6","muon_d_3of6","muon_d_4of6","muon_d_5of6","muon_d_6of6",
      "tt_lep_1of2","tt_lep_2of2","tt_1of5","tt_2of5","tt_3of5","tt_4of5","tt_5of5","ww","wzz","wz","zz" 
    };

  // Process command line input.
    // Set up options
    po::options_description opDesc("Z+c Ntuple Processor options", 150);
    opDesc.add_options()
        ("help"     ",h",                                                 "Print help message"          )
        ("dataset"  ",d",  po::value<string>()->default_value(dataset  ), "Dataset to process"          )
        ("npconfig" ",n",  po::value<string>()->default_value(npconfig ), "NtupleProcConfig file"       )
        ("anconfig" ",a",  po::value<string>()->default_value(anconfig ), "AnalysisConfig file"         )
        ("options"  ",o",  po::value<string>()->default_value(options  ), "Misc. options"               )
        ("maxevents"",m",  po::value<int>()   ->default_value(maxEvents), "Number of events to process" )
    ;
    po::variables_map cmdInput;
    po::store(po::parse_command_line(argc, argv, opDesc), cmdInput);
    po::notify(cmdInput);
    if(cmdInput.count("help"))
    {

        cout << opDesc << "\n" << "Possible dataset input options: ";
        for(auto& dsNm : datasetNames) cout << dsNm << ", ";
        cout << endl;
        return 1;
    }

  // Get dataset name to run on - NtupleProc should know where the ntuple is and how to run on it.
    if(    !(cmdInput.count("dataset"))    // If no dataset is specified...
        || std::find(datasetNames.begin(), datasetNames.end(), (dataset=cmdInput["dataset"].as<string>()))==datasetNames.end()    // .. or an improper dataset is specified...
      )
    { // KICK
        cout << "No valid dataset specified. Please use one of the following options: ";
        for(auto& dsNm : datasetNames) cout << dsNm << ", ";
        cout << endl;
        return 1;
    }

  // Get config files (use dashed input for custom runs. Standard will have all the extractors set up).
    // Check if the input file is valid before use.
    ifstream testFile;
    if(cmdInput.count("npconfig")) npconfig = cmdInput["npconfig"].as<string>();
    testFile.open(npconfig);
    if(!testFile) {testFile.close(); cout << "Invalid npconfig file specified: " << npconfig << endl; return 1;}
    testFile.close();
    // Check if the input file is valid before use.
    if(cmdInput.count("anconfig")) anconfig = cmdInput["anconfig"].as<string>();
    testFile.open(anconfig);
    if(!testFile) {testFile.close(); cout << "Invalid anconfig file specified: " << anconfig << endl; return 1;}
    testFile.close();

    if(cmdInput.count("options")) options = cmdInput["options"].as<string>();

    // Get maxevents. Specified as -1 if
    maxEvents = ( cmdInput.count("maxevents") ? cmdInput["maxevents"].as<int>() : -1);

  // COMMAND LINE OUTPUT
    cout << "\n\n"
            "================================================================================\n"
            "===NtupleProcessor==============================================================\n"
            "  Processing Begun: " << ts_mainBegin << "\n"
            "\n";

  // Add secondary options to options input.
    if( !(TString(dataset).Contains("muon") && TString(dataset).Contains("elec"))) options += "Sim";
    if(   TString(dataset).Contains("dy", TString::kIgnoreCase)                  ) options += "DY" ;

  // Create the NtupleProcessor
    NtupleProcessor ntplproc(dataset, npconfig, anconfig, options, maxEvents);

  // CLOSING OUTPUT.
    TString ts_mainEnd = timeStamp();
    cout << "\n"
            "  Completion time: " << ts_mainEnd <<      "\n"
            "===NtupleProcessor - COMPLETE===================================================\n"
            "================================================================================\n"
         << endl;

    return 0;
}
