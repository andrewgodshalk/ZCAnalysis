/*------------------------------------------------------------------------------
   main.cpp

Modified: 2016-09-08

Main file for leptonSF_test.

Creates a LeptonSFData object from json, then outputs a test query.

------------------------------------------------------------------------------*/

// HEADERS
#include <iostream>                 // stdlib
#include <utility>
#include "LeptonSFData.h"           // Class to test.

using std::cout;     using std::endl;

int main(int argc, char* argv[])
{

  // COMMAND LINE OUTPUT
    cout << "\n\n"
            "================================================================================\n"
            "===leptonSF_test================================================================\n"
         << endl;


  // Create LeptonSFData Object
    LeptonSFData lsfd( "../../data/leptonSF/CutBasedID_TightWP.json",
                       "CutBasedID_TightWP",
                       "abseta_pt_ratio"
//                       "eta_pt_ratio"
                     );

  // Test to see if you can extract a SF
    float pt1 = 15.  , eta1 =  3.9 ;
    float pt2 = 40.  , eta2 = -2.0 ;
    float pt3 = 35.2 , eta3 = -1.0 ;
    float pt4 = 150. , eta4 =  0.0 ;
    float pt5 = 10000. , eta5 =  4.0 ;
    std::pair<double, double> sf1 = lsfd.getSF(pt1, eta1);
    std::pair<double, double> sf2 = lsfd.getSF(pt2, eta2);
    std::pair<double, double> sf3 = lsfd.getSF(pt3, eta3);
    std::pair<double, double> sf4 = lsfd.getSF(pt4, eta4);
    std::pair<double, double> sf5 = lsfd.getSF(pt5, eta5);
    cout << "      SF for (pt,eta)=(" << pt1 << "," << eta1 << "): " << sf1.first << " +- " << sf1.second << endl;
    cout << "      SF for (pt,eta)=(" << pt2 << "," << eta2 << "): " << sf2.first << " +- " << sf2.second << endl;
    cout << "      SF for (pt,eta)=(" << pt3 << "," << eta3 << "): " << sf3.first << " +- " << sf3.second << endl;
    cout << "      SF for (pt,eta)=(" << pt4 << "," << eta4 << "): " << sf4.first << " +- " << sf4.second << endl;
    cout << "      SF for (pt,eta)=(" << pt5 << "," << eta5 << "): " << sf5.first << " +- " << sf5.second << endl;


  // CLOSING OUTPUT.
    cout << "\n"
            "===leptonSF_test - COMPLETE=====================================================\n"
            "================================================================================\n"
         << endl;

    return 0;
}
