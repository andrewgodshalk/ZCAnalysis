/*------------------------------------------------------------------------------
   main.cpp

Modified: 2016-09-08

Main file for leptonSF_test.

Creates a LeptonSFData object from json, then outputs a test query.

------------------------------------------------------------------------------*/

// HEADERS
#include <iostream>                 // stdlib
#include "LeptonSFData.h"

using std::cout;     using std::endl;

int main(int argc, char* argv[])
{

  // COMMAND LINE OUTPUT
    cout << "\n\n"
            "================================================================================\n"
            "===leptonSF_test================================================================\n"
         << endl;


  // Create LeptonSFData Object
    LeptonSFData( "leptonSF/CutBasedID_TightWP.json",
                  "CutBasedID_TightWP"     ,
                  "eta_pt_ratio"
                );


  // CLOSING OUTPUT.
    cout << "\n"
            "===leptonSF_test - COMPLETE=====================================================\n"
            "================================================================================\n"
         << endl;

    return 0;
}
