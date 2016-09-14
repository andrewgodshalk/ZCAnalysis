/*------------------------------------------------------------------------------
LeptonSFData.cpp

 Created : 2016-09-08  godshalk
 Modified: 2016-09-14  godshalk

 TO DO: Make a function to search tree for the correct bin range. Used twice,
   for pt and eta.
   - Have function return NULL if no extrapolation, iterator (or bin?) otherwise?

2016-09-11 - 
  Added an include for json_parser_read header file, included in the ZCLibrary
  directory. LPC boost libraries are out of date and contain a bug that prevents
  the read-in of json files. Header is a bandaid that needs to be uncommented 
  when compiling on LPC.
------------------------------------------------------------------------------*/

#include "LeptonSFData.h"                        // Class header
#include <algorithm>                             // STL
#include <iostream>
//#include "json_parser_read.hpp"   // Boost libs
#include <boost/property_tree/json_parser.hpp>   // Boost libs

using std::cout;   using std::cerr;     using std::pair;
using std::endl;   using std::string;   using std::min;
using boost::property_tree::ptree;

LeptonSFData::LeptonSFData()
: sfFileName_(""), leptonType_(""), binningPref_(""), extrapolateFromClosestBin_(true), jsonTreePopulated_(false),
  loadFromJSONSuccessful_(false), loadFromROOTSuccessful_(false), histEtaMaxBin_(-1), histPtMaxBin_(-1)
{}


LeptonSFData::LeptonSFData(string fn, string lt, string bp, bool efcb)
: sfFileName_(fn), leptonType_(lt), binningPref_(bp), extrapolateFromClosestBin_(efcb), jsonTreePopulated_(false),
  loadFromJSONSuccessful_(false), loadFromROOTSuccessful_(false), histEtaMaxBin_(-1), histPtMaxBin_(-1)
{
    f_sfFile_  = NULL;
    h_sfHisto_ = NULL;

  // TEST output
    cout << "  LeptonSFData: Created.\n"
            "    JSON File name     : " << sfFileName_ << "\n"
            "    Lepton Type        : " << leptonType_   << "\n"
            "    Binning preference : " << binningPref_  << "\n"
         << endl;


  // Determine what kind of file sfFileName is.
    string file_sfx = sfFileName_.substr(sfFileName_.size()-5, 5);
//    if     ( file_sfx==".json" && (loadFromJSONSuccessful_=loadSFFromJSON(sfFileName_)) ) sfRetrievalFunction_ = &LeptonSFData::getSFFromJSON;
//    else if( file_sfx==".root" &&                          loadSFFromROOT(sfFileName_)  ) sfRetrievalFunction_ = &LeptonSFData::getSFFromROOT;
//    else {
//        cerr << "    ERROR: Unable to load scale factors from file: " << sfFileName_ << endl;
//        sfRetrievalFunction_ = &LeptonSFData::getNonSF;
//    }
    if( file_sfx==".json") loadFromJSONSuccessful_=loadSFFromJSON(sfFileName_);
//    if( file_sfx==".root") loadFromROOTSuccessful_=loadSFFromROOT(sfFileName_);
    if(!loadFromJSONSuccessful_ && !loadFromROOTSuccessful_)
        cerr << "    ERROR: Unable to load scale factors from file: " << sfFileName_ << endl;

    // If JSON file loaded, check the json file to see if the input leptonType is a member.
    if(loadFromJSONSuccessful_)
    {
        if(sfPropTree_.find(leptonType_) == sfPropTree_.not_found())
            cerr << "    ERROR: Lepton type " << leptonType_ << " not found in file " << sfFileName_ << endl;

      // Check if the desired input binning is found in the leptonType tree
//        else if(sfPropTree_.find(leptonType_+"."+binningPref_) == sfPropTree_.not_found())
//        cerr << "    ERROR: Binning preference " << binningPref_ << " not found tree for lepton type " << leptonType_ << endl;

        else  // All inputs are valid with the given parameters.
        { // Set the main prop tree to the child tree designated by leptonType_
            sfPropTree_ = sfPropTree_.get_child(leptonType_+"."+binningPref_);
            //cout << "    LeptonSF JSON Successfully loaded from file: " << sfFileName_ << endl;
            jsonTreePopulated_ = true;
        }
    }

}


bool LeptonSFData::loadSFFromJSON(string fn)
{  // Attempts to load SFs from file into a property tree.
    try { read_json (fn, sfPropTree_); }
    catch (boost::property_tree::json_parser::json_parser_error)
    {
        cerr << "    ERROR: Unable to load JSON from file: " << fn << endl;
        return false;
    }
    return true;
}


bool LeptonSFData::loadSFFromROOT(string fn)
{ // Attempts to load SF histogram from root file.
  // Attempt to open file.
    f_sfFile_ = TFile::Open(fn.c_str());
    if(!f_sfFile_){
        cerr << "    ERROR: Unable to load SF Histogram from file. Unable to open file: " << fn << endl;
        return false;
    }

  // Attempt to load histogram from file.
    f_sfFile_->GetObject(leptonType_.c_str(), h_sfHisto_);
    if(!h_sfHisto_){
        cerr << "    ERROR: Unable to load SF Histogram from file. Histogram not found: " << leptonType_ << endl;
        return false;
    }

  // Set maximum bin values, then return true;
    histEtaMaxBin_ = h_sfHisto_->GetXaxis()->GetNbins();
    histPtMaxBin_  = h_sfHisto_->GetYaxis()->GetNbins();
return true;
}


pair<double, double> LeptonSFData::getSF(double eta, double pt)
{
    if(loadFromJSONSuccessful_ && jsonTreePopulated_ ) return getSFFromJSON(eta,pt);
    if(loadFromROOTSuccessful_)                        return getSFFromROOT(eta,pt);
    return getNonSF(eta,pt);
}


pair<double, double> LeptonSFData::getSFFromJSON(double pt, double eta)   // Get the lepton scale factor based on its pt and eta. from JSON file.
{
  // If not work with a valid SF tree, return 1 +- 0.
    if(!jsonTreePopulated_) return std::make_pair(1.0, 0.0);

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


pair<double, double> LeptonSFData::getSFFromROOT(double pt, double eta)   // Get the lepton scale factor based on its pt and eta.
{
  // If the binning preference indicates absolute value of eta...
    if(binningPref_.find("abseta")!=string::npos) eta = fabs(eta);

  // Find bins corresponding to pt & eta.
    int etaBin = h_sfHisto_->GetXaxis()->FindBin(eta);
    int  ptBin = h_sfHisto_->GetYaxis()->FindBin( pt);
    bool outOfBounds = false;
    if( etaBin==0                ) { etaBin = 1             ; outOfBounds=true; }
    if( etaBin==histEtaMaxBin_+1 ) { etaBin = histEtaMaxBin_; outOfBounds=true; }
    if(  ptBin==0                ) {  ptBin = 1             ; outOfBounds=true; }
    if(  ptBin==histPtMaxBin_ +1 ) {  ptBin = histPtMaxBin_ ; outOfBounds=true; }
    if(outOfBounds && !extrapolateFromClosestBin_) return std::make_pair(1.0, 0.0);

  // Find value of corresponding bins.
    double value = h_sfHisto_->GetBinContent(etaBin, ptBin);
    double error = h_sfHisto_->GetBinError  (etaBin, ptBin);

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
