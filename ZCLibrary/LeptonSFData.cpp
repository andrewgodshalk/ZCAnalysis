/*------------------------------------------------------------------------------
LeptonSFData.cpp

 Created : 2016-09-08  godshalk
 Modified: 2016-09-09  godshalk

 TO DO: Make a function to search tree for the correct bin range. Used twice,
   for pt and eta.
   - Have function return NULL if no extrapolation, iterator (or bin?) otherwise?

------------------------------------------------------------------------------*/

#include "LeptonSFData.h"                        // Class header
#include <algorithm>                             // STL
#include <iostream>
#include "json_parser_read.hpp"   // Boost libs
#include <boost/property_tree/json_parser.hpp>   // Boost libs

using std::cout;   using std::cerr;     using std::pair;
using std::endl;   using std::string;   using std::min;
using boost::property_tree::ptree;

LeptonSFData::LeptonSFData()
: jsonFileName_(""), leptonType_(""), binningPref_(""), extrapolateFromClosestBin_(true), populated_(false)
{}


LeptonSFData::LeptonSFData(string fn, string lt, string bp, bool efcb)
: jsonFileName_(fn), leptonType_(lt), binningPref_(bp), extrapolateFromClosestBin_(efcb), populated_(false)
{
  // TEST output
//    cout << "  LeptonSFData: Created.\n"
//            "    JSON File name     : " << jsonFileName_ << "\n"
//            "    Lepton Type        : " << leptonType_   << "\n"
//            "    Binning preference : " << binningPref_  << "\n"
//         << endl;

  // Attempt to extract the SF Tree for the given file name, lepton type, and binning preference.
    if(!loadJSONFromFile(jsonFileName_))   // Attempt to open file.
        cerr << "    ERROR: Unable to load JSON from file: " << jsonFileName_ << endl;

    // Check the json file to see if the input leptonType is a member.
    else if(sfPropTree_.find(leptonType_) == sfPropTree_.not_found())
        cerr << "    ERROR: Lepton type " << leptonType_ << " not found in file " << jsonFileName_ << endl;

    // Check if the desired input binning is found in the leptonType tree
//    else if(sfPropTree_.find(leptonType_+"."+binningPref_) == sfPropTree_.not_found())
//        cerr << "    ERROR: Binning preference " << binningPref_ << " not found tree for lepton type " << leptonType_ << endl;

    else  // All inputs are valid with the given parameters.
    { // Set the main prop tree to the child tree designated by leptonType_
        sfPropTree_ = sfPropTree_.get_child(leptonType_+"."+binningPref_);
        //cout << "    LeptonSF JSON Successfully loaded from file: " << jsonFileName_ << endl;
        populated_ = true;
    }
}


bool LeptonSFData::loadJSONFromFile(string fn)
{  // Attempts to load SFs from file into a property tree.
    try { read_json (fn, sfPropTree_); }
    catch (boost::property_tree::json_parser::json_parser_error)
    {
        cerr << "    ERROR: Unable to load JSON from file: " << fn << endl;
        return false;
    }
    return true;
}


pair<double, double> LeptonSFData::getSF(double pt, double eta)   // Get the lepton scale factor based on its pt and eta.
{
  // If not work with a valid SF tree, return 1 +- 0.
    if(!populated_) return std::make_pair(1.0, 0.0);

  // If the binning preference indicates absolute value of eta...
    if(binningPref_.find("abseta")!=string::npos) eta = fabs(eta);

  // Store location and offset of the closest eta bin.
    ptree::const_iterator closestEtaBin = sfPropTree_.end();
    float distFromClosestEtaBin = 9999;
    pair<float,float> currentRange(-1000,1000);

  // Search tree for closest eta bin.
    ptree::const_iterator etaBin = sfPropTree_.begin();
    while(etaBin!=sfPropTree_.end())
    { // Looping over eta bins...
        //cout << "      etaBin: " << etaBin->first << endl;

      // Extract range of this bin.
        currentRange = getRangeFromKey(etaBin->first);

      // Check if this is the bin. Break if it is.
        if( currentRange.first <= eta && eta < currentRange.second) break;

        // Check to see if this is the closest bin yet.
        if(fabs(currentRange.first-eta) < distFromClosestEtaBin || fabs(currentRange.first-eta) < distFromClosestEtaBin )
        {
            distFromClosestEtaBin = min( fabs(currentRange.first-eta), fabs(currentRange.first-eta));
            closestEtaBin = etaBin;
        }
        etaBin++;
    }

  // If the end of the loop was reached and extrapolation is set, just use the closest bin.
    if(etaBin == sfPropTree_.end())
    {
        if(extrapolateFromClosestBin_) etaBin = closestEtaBin;
        else return std::make_pair(1.0, 0.0);
    }

  // Store location and offset of the closest pt bin.
    ptree ptTree = etaBin->second;
    ptree::const_iterator closestPtBin = ptTree.end();
    float distFromClosestPtBin = 9999;
    currentRange = std::make_pair(-1000,1000);

  // Search the etaBin tree for the closest pt bin.
    ptree::const_iterator ptBin = ptTree.begin();
    while(ptBin!=ptTree.end())
    { // Looping over eta bins...
        //cout << "      ptBin: " << ptBin->first << endl;

      // Extract range of this bin.
        currentRange = getRangeFromKey(ptBin->first);

      // Check if this is the bin. Break if it is.
        if( currentRange.first <= pt && pt < currentRange.second) break;

        // Check to see if this is the closest bin yet.
        if(fabs(currentRange.first-pt) < distFromClosestPtBin || fabs(currentRange.first-pt) < distFromClosestPtBin )
        {
            distFromClosestPtBin = min( fabs(currentRange.first-pt), fabs(currentRange.first-pt));
            closestPtBin = ptBin;
        }
        ptBin++;
    }

  // If the end of the loop was reached and extrapolation is set, just use the closest bin.
    if(ptBin == ptTree.end())
    {
        if(extrapolateFromClosestBin_) ptBin = closestPtBin;
        else return std::make_pair(1.0, 0.0);
    }

    double value = ptBin->second.get<double>("value");
    double error = ptBin->second.get<double>("error");

return std::make_pair(value, error);

//return std::make_pair(1.0, 0.0);
}



pair<float, float> LeptonSFData::getRangeFromKey(const string& key)
{ // Returns pair of numbers indicating a range from a string formatted like so: "label:[NUM,NUM]"
    float minR, maxR;
    string::size_type numBegin, numLen;    // Beginning and length of number in string.

  // Find beginning of range
    numBegin = key.find('[') + 1;
    numLen   = key.find(',', numBegin)-numBegin;
    minR = std::stof(key.substr(numBegin, numLen));

  // Find end of range
    numBegin += numLen+1;
    numLen   = key.find(']', numBegin)-numBegin;
    maxR = std::stof(key.substr(numBegin, numLen));

//    cout << "         " << key << " " << minR << " " << maxR << endl;

return std::make_pair(minR, maxR);
}
