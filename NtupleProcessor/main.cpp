/*------------------------------------------------------------------------------
   main.cpp

------------------------------------------------------------------------------*/

// HEADERS
#include <iostream>                 // stdlib
#include <string>
#include "TFile.h"                  // ROOT class headers
#include "TString.h"
#include "TTree.h"
#include "../ZCLibrary/interface/timestamp.h"

using namespace std;

int main(int argc, char* argv[])
{
  // Record the time main starts processing.
    TString ts_mainBegin  = timeStamp();
    TString fts_mainBegin = fileTimeStamp();

  // TEST OUTPUT
    cout << "\n\n"
            "============================================\n"
            "===ZCHistogramExtractor=====================\n"
            "  Processing Begun: " << ts_mainBegin << "\n"
            "\n";



    cout << "\n"
            "===ZCHistogramExtractor - FINISHED==========\n"
            "============================================\n" << endl;

    return 0;
}
