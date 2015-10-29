/*------------------------------------------------------------------------------
   main.cpp

Modified: 2015-09-14

Main file for the NtupleProcessor program.

------------------------------------------------------------------------------*/

// HEADERS
#include <iostream>                 // stdlib
#include <string>
#include <TFile.h>                  // ROOT class headers
#include <TString.h>
#include <TTree.h>
#include "../ZCLibrary/timestamp.h"
#include "../NtupleProcessor/interface/NtupleProcessor.h"

using namespace std;

int main(int argc, char* argv[])
{
  // Record the time main starts processing.
    TString ts_mainBegin  = timeStamp();
    TString fts_mainBegin = fileTimeStamp();

  // COMMAND LINE OUTPUT
    cout << "\n\n"
            "================================================================================\n"
            "===NtupleProcessor==============================================================\n"
            "  Processing Begun: " << ts_mainBegin << "\n"
            "\n";

  // Create the NtupleProcessor
    NtupleProcessor ntplproc("");


  // CLOSING OUTPUT.
    TString ts_mainEnd = timeStamp();
    cout << "\n"
            "  Completion time: " << ts_mainEnd <<      "\n"
            "===NtupleProcessor - COMPLETE===================================================\n"
            "================================================================================\n"
         << endl;

    return 0;
}
