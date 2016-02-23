/*------------------------------------------------------------------------------
   main.cpp

Created: 2015-11-23

Main file for the ControlPlotMaker program.
Handles command line arguements.

------------------------------------------------------------------------------*/

// HEADERS
#include <algorithm>
#include <iostream>                 // stdlib
#include <fstream>
#include <string>
#include <vector>
#include <TFile.h>                  // ROOT class headers
#include <TString.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "../ZCLibrary/timestamp.h"
#include "interface/ControlPlotMaker.h"

using std::cout;   using std::endl;   using std::vector;   using std::string;   using std::ifstream;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  // Record the time main starts processing.
    TString ts_mainBegin  = timeStamp();
    TString fts_mainBegin = fileTimeStamp();

  // Command line variables
    string anconfig            = "ZCLibrary/zcAnalysisConfig_default.ini";
    string input               = "zc_analysis.root";
    string output              = "zc_analysis.root";
    string options             = "";
    bool   usingLogScale       = true;
    bool   splitDYByFlavor     = true;
    bool   splitTauDecayFromDY = true;
    bool   outputAsEPS         = true;

  // Process command line input.
    // Set up options
    po::options_description opDesc("Z+c Ntuple Processor options", 150);
    opDesc.add_options()
        ("help"     ",h"                                                          , "Print help message"                                )
        ("anconfig" ",a",  po::value<string>()->default_value(anconfig           ), "AnalysisConfig file"                               )
        ("input"    ",I",  po::value<string>()->default_value(input              ), "Input file containing dataset-specific histograms" )
        ("output"   ",O",  po::value<string>()->default_value(output             ), "File to output histograms"                         )
        ("options"  ",o",  po::value<string>()->default_value(options            ), "Misc. options"                                     )
        ("log"      ",l",  po::value<bool>  ()->default_value(usingLogScale      ), "Use log scale on y-axis"                           )
        ("flavor"   ",f",  po::value<bool>  ()->default_value(splitDYByFlavor    ), "Split DY events by first jet's flavor"             )
        ("tautau"   ",t",  po::value<bool>  ()->default_value(splitTauDecayFromDY), "Split DY events with Z->tautau decays"             )
        ("eps"      ",e",  po::value<bool>  ()->default_value(outputAsEPS        ), "Output histos in EPS format as well."              )
    ;
    po::variables_map cmdInput;
    po::store(po::parse_command_line(argc, argv, opDesc), cmdInput);
    po::notify(cmdInput);

  // Process inputs
    ifstream testFile;
    if(cmdInput.count("anconfig")) anconfig = cmdInput["anconfig"].as<string>();
    testFile.open(anconfig);
    if(!testFile) {testFile.close(); cout << "Invalid anconfig file specified: " << anconfig << endl; return 1;}
    testFile.close();
    if(cmdInput.count("options")) options             = cmdInput["options"].as<string>();
    if(cmdInput.count("input"  )) input               = cmdInput["input"  ].as<string>();
    if(cmdInput.count("output" )) output              = cmdInput["output" ].as<string>();
    if(cmdInput.count("log"    )) usingLogScale       = cmdInput["log"    ].as<bool  >();
    if(cmdInput.count("flavor" )) splitDYByFlavor     = cmdInput["flavor" ].as<bool  >();
    if(cmdInput.count("tautau" )) splitTauDecayFromDY = cmdInput["tautau" ].as<bool  >();
    if(cmdInput.count("eps"    )) outputAsEPS         = cmdInput["eps"    ].as<bool  >();

  // Begin output
    if(cmdInput.count("help"))
    { // If help flag given, print help message.
        cout << opDesc << "\n" << endl;
        return 1;
    }

    cout << "\n\n"
            "================================================================================\n"
            "===ControlPlotMaker=============================================================\n"
            "  Processing Begun: " << ts_mainBegin << "\n"
            "\n";

  // Create the ControlPlotMaker
/*PSEUDO*/ cout << "main.cpp(): Creating ControlPlotMaker class with inputs..."
                 "\n  Options: " << options
                << endl;

    ControlPlotMaker cp(anconfig, input, output, options, usingLogScale, splitDYByFlavor, splitTauDecayFromDY, outputAsEPS);

  // CLOSING OUTPUT.
    TString ts_mainEnd = timeStamp();
    cout << "\n"
            "  Completion time: " << ts_mainEnd <<      "\n"
            "===ControlPlotMaker - COMPLETE==================================================\n"
            "================================================================================\n"
         << endl;

return 0;
}
