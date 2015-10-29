/*------------------------------------------------------------------------------
NtupleProcessor.cpp

 Created : 2015-09-14  godshalk
 Modified: 2015-10-28  godshalk

------------------------------------------------------------------------------*/

#include <iostream>
#include <TString.h>
#include "../interface/NtupleProcessor.h"

using std::cout;   using std::endl;

NtupleProcessor::NtupleProcessor(TString o, TString fnc)
:  options(o), fn_config(fnc), runParams(fnc)
{
  // TEST output
    cout << "    NtupleProcessor: Created.\n"
            "      Config File: " << fn_config << "\n"
            "      Options: "     << options   << "\n"
         << endl;


  // Takes input options and processes appropriately
  //   Options that can be specified:
  //     - Operating points
  //     - Differential variables (default: JetPT)
  //     - Dataset ntuple file to run on (default: all)
  //     - Desired number of entry to run on per file and entries to skip.
  //     - Modifications to the analysis (systematic analyses, etc)
  // For each dataset that needs to be run over...
  //   Opens the appropriate file and tree
  //   Creates a TreeIterator, EventHandler, and the desired HistogramMakers
  //   Runs the TreeIterator.

}
