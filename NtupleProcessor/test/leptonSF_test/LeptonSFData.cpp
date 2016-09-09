/*------------------------------------------------------------------------------
LeptonSFData.cpp

 Created : 2016-09-08  godshalk
 Modified: 2016-09-08  godshalk

------------------------------------------------------------------------------*/

#include "LeptonSFData.h"                        // Class header
#include <iostream>                              // STL
#include <boost/property_tree/json_parser.hpp>   // Boost libs

using std::cout;   using std::cerr;
using std::endl;   using std::string;

LeptonSFData::LeptonSFData(string fn, string lt, string bp)
: jsonFileName_(fn), leptonType_(lt), binningPref_(bp), populated_(false)
{
  // TEST output
    cout << "  LeptonSFData: Created.\n"
            "    JSON File name     : " << jsonFileName_ << "\n"
            "    Lepton Type        : " << leptonType_   << "\n"
            "    Binning preference : " << binningPref_  << "\n"
         << endl;

  // Process JSON file
    if(!loadJSONFromFile(jsonFileName_))
        cout << "    ERROR: Unable to load JSON from file: " << jsonFileName_ << endl;
    else
    {      // Check the json file for proper field
//        if lep_name not in results.keys():
//            self.valid = False
//            print "LeptonSF: ", lep_name , " is not a valid identifier"
//            return False
//        self.res = results[lep_name]
//        self.lep_name = lep_name
//        self.lep_binning = lep_binning

//        self.valid = True
        cout << "    LeptonSF JSON Successfully loaded from file: " << jsonFileName_ << endl;
        populated_ = true;
//        self.extrapolateFromClosestBin = extrapolateFromClosestBin
//        f.close()


    }
}


bool LeptonSFData::loadJSONFromFile(string fn)
{
    try { read_json (fn, sfPropTree); }
    catch (boost::property_tree::json_parser::json_parser_error)
    {
        cerr << "    ERROR: Unable to load JSON from file: " << fn << endl;
        return false;
    }
    return true;
}

