/*------------------------------------------------------------------------------
   main.cpp

Modified: 2015-09-14

Main file for the NtupleProcessor program.
Handles command line arguments.

------------------------------------------------------------------------------*/

// HEADERS
#include <algorithm>
#include <iostream>                 // stdlib
#include <string>
#include <vector>
#include <TFile.h>                  // ROOT class headers
#include <TString.h>
#include <TTree.h>
#include "TApplication.h"
#include "TROOT.h"
#include "TRint.h"
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
  // Needed to make sure ROOT libraries "link correctly. Receive seg faults while trying to use TH2F in LeptonSFData otherwise.
  // Recommendation found at link: https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=18101&p=76968#p76968
  //  TApplication a("a", 0, 0); // just to make sure that the autoloading of ROOT libraries works

  // New recommendation from https://root.cern.ch/phpBB3/viewtopic.php?t=9838, https://root.cern.ch/doc/master/classTROOT.html
    // TRint *theApp = new TRint("ROOT example", &argc, argv);
    // APIanalyzer mainWindow(gClient->GetRoot(),600,500,theApp.Argc(), theApp.Argv());
    // theApp->Run();

  // Record the time main starts processing.
    TString ts_mainBegin  = timeStamp();
    TString fts_mainBegin = fileTimeStamp();

  // Variables needed for Ntuple processor
    string dataset     = "dy";
    string npconfig    = "NtupleProcessor/etc/zcNtupleProcConfig_default.ini";
    string anconfig    = "ZCLibrary/zcAnalysisConfig_default.ini";
    string ntupleFile  = "";
    string ntupleLabel = "";
    string options     = "";
    int maxEvents      = -1;

  // Set up list of valid datsets
    vector<string> datasetNames = {
/*	    "dy_01of20", "dy_02of20", "dy_03of20", "dy_04of20", "dy_05of20", "dy_06of20", "dy_07of20",
	    "dy_08of20", "dy_09of20", "dy_10of20", "dy_11of20", "dy_12of20", "dy_13of20", "dy_14of20",
	    "dy_15of20", "dy_16of20", "dy_17of20", "dy_18of20", "dy_19of20", "dy_20of20",
	    "elecBv1", "elecBv2_1of5", "elecBv2_2of5", "elecBv2_3of5", "elecBv2_4of5", "elecBv2_5of5",
	    "elecC_1of2", "elecC_2of2", "elecD_1of2", "elecD_2of2", "elecE_1of2", "elecE_2of2",
	    "elecF", "elecG_1of3", "elecG_2of3", "elecG_3of3",
	    "muonBv1", "muonBv2_1of5", "muonBv2_2of5", "muonBv2_3of5", "muonBv2_4of5", "muonBv2_5of5",
	    "muonC_1of2", "muonC_2of2", "muonD_1of3", "muonD_2of3", "muonD_3of3",
	    "muonE_1of3", "muonE_2of3", "muonE_3of3", "muonF_1of2", "muonF_2of2",
	    "muonG_1of4", "muonG_2of4", "muonG_3of4", "muonG_4of4",
	    "tt","ww","wz","zz",
*/
      "muonBv1","muonBv2_01of24","muonBv2_02of24","muonBv2_03of24","muonBv2_04of24","muonBv2_05of24","muonBv2_06of24",
      "muonBv2_07of24","muonBv2_08of24","muonBv2_09of24","muonBv2_10of24","muonBv2_11of24","muonBv2_12of24",
      "muonBv2_13of24","muonBv2_14of24","muonBv2_15of24","muonBv2_16of24","muonBv2_17of24","muonBv2_18of24",
      "muonBv2_19of24","muonBv2_20of24","muonBv2_21of24","muonBv2_22of24","muonBv2_23of24","muonBv2_24of24",
      "muonC", "muonD", "dy_inc_amc","dy_inc_mm","tt","ww","wz","zz",

      //"dy1j_1of7","dy1j_2of7","dy1j_3of7","dy1j_4of7","dy1j_5of7","dy1j_6of7","dy1j_7of7","dy",
      //"st_s","st_t_4f_lep","st_t_5f_lep","stbar_t_4f_lep","stbar_t_5f_lep","stbar_tw_5f_nohad","stbar_tw_5f","st_tw_5f_nohad","st_tw_5f",
      //"elec_c","elec_d_1of8","elec_d_2of8","elec_d_3of8","elec_d_4of8","elec_d_5of8","elec_d_6of8","elec_d_7of8","elec_d_8of8",
      //"muon_c","muon_d_1of6","muon_d_2of6","muon_d_3of6","muon_d_4of6","muon_d_5of6","muon_d_6of6",
      //"tt_lep_1of2","tt_lep_2of2","tt_1of5","tt_2of5","tt_3of5","tt_4of5","tt_5of5","ww","wzz","wz","zz"
    };

  // Process command line input.
    // Set up options
    po::options_description opDesc("Z+c Ntuple Processor options", 150);
    opDesc.add_options()
        ("help"        ",h",                                                 "Print help message"                      )
        ("dataset"     ",d",  po::value<string>()->default_value(dataset     ), "Dataset to process"                   )
        ("ntuplefile"  ",N",  po::value<string>()->default_value(ntupleFile  ), "Ntuple file input (overides NPC def)" )
        ("ntuplelabel" ",L",  po::value<string>()->default_value(ntupleLabel ), "Label for input Ntuple File"          )
        ("npconfig"    ",n",  po::value<string>()->default_value(npconfig    ), "NtupleProcConfig file"                )
        ("anconfig"    ",a",  po::value<string>()->default_value(anconfig    ), "AnalysisConfig file"                  )
        ("options"     ",o",  po::value<string>()->default_value(options     ), "Misc. options"                        )
        ("maxevents"   ",m",  po::value<int>()   ->default_value(maxEvents   ), "Number of events to process"          )
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

  // Check to see if an ntuple was specified.
    if(cmdInput.count("ntuplefile" )) ntupleFile  = cmdInput["ntuplefile" ].as<string>();
    if(cmdInput.count("ntuplelabel")) ntupleLabel = cmdInput["ntuplelabel"].as<string>();
    if(cmdInput.count("dataset"    )) dataset     = cmdInput["dataset"    ].as<string>();
    if( ntupleFile != "")
    {
	cout << "main(): Ntuple File Speified with dataset label: " << dataset
	     << "\nFile : " << ntupleFile
	     << "\nLabel: " << ntupleLabel << endl;
    }

  // Get dataset name to run on - NtupleProc should know where the ntuple is and how to run on it.
    else if(    !(cmdInput.count("dataset"))   // If no dataset is specified...
             || std::find(datasetNames.begin(), datasetNames.end(), dataset)==datasetNames.end()    // .. or an improper dataset is specified...
           )
    { // KICK
        cout << "main(): No valid dataset specified. Please use one of the following options: ";
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
    if( !(TString(dataset).Contains("muon") || TString(dataset).Contains("elec"))) options += "Sim";
    if(  TString(dataset).Contains("dy", TString::kIgnoreCase)                   ) options += "DY" ;

  // Create the NtupleProcessor
    NtupleProcessor ntplproc(dataset, ntupleFile, ntupleLabel, npconfig, anconfig, options, maxEvents);

  // CLOSING OUTPUT.
    TString ts_mainEnd = timeStamp();
    cout << "\n"
            "  Completion time: " << ts_mainEnd <<      "\n"
            "===NtupleProcessor - COMPLETE===================================================\n"
            "================================================================================\n"
         << endl;

    return 0;
}
