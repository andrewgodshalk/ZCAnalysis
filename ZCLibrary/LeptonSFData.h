#ifndef GUARD_LeptonSFData_h
#define GUARD_LeptonSFData_h

/*------------------------------------------------------------------------------
   LeptonSFData

 Created : 2016-09-08  godshalk
 Modified: 2016-09-09  godshalk

 Outputs Lepton Scale Factors based on three main inputs:
     - A JSON file that stores lepton scale factors of a given category
     - The type of lepton we're trying to extract scale factors for.
     - The desired binning we'd like to use.

 Implementation adapted from leptonSF python class used by CMS VHbb group: 
    https://twiki.cern.ch/twiki/bin/viewauth/CMS/VHiggsBBCodeUtils#Lepton_SF
    https://github.com/vhbb/cmssw/blob/vhbbHeppy76X/VHbbAnalysis/Heppy/python/leptonSF.py
 More information on Muon SFs given at:
    https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceEffsRun2

 Currently uses JSON parser to convert JSON into a boost prop tree.
 Documentation can be found at:
    http://www.boost.org/doc/libs/1_41_0/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.json_parser

------------------------------------------------------------------------------*/

#include <string>
#include <utility>
//#include "json_parser_read.hpp"
#include <boost/property_tree/ptree.hpp>
#include <TFile.h>
#include <TH2D.h>

class LeptonSFData
{
  public:
    LeptonSFData(std::string, std::string, std::string, bool efcb = true);  // Primary constructor.
    LeptonSFData();     // Empty Constructor
//   ~LeptonSFData(){delete f_sfFile_; delete h_sfHisto_;}
   ~LeptonSFData(){}

    bool loadSFFromJSON(std::string);                // Attempts to SFs from file into a property tree.
    bool loadSFFromROOT(std::string);                // Attempts to SFs from file into a property tree.
//    std::pair<double, double> getSF(double eta, double phi) {return (this->*sfRetrievalFunction_)(eta,phi);}
    std::pair<double, double> getSF(double, double);
        // Calls the SF function specified by the constructor.

  private:
//    typedef std::pair<double, double> (LeptonSFData::*sfFunc)(double,double);
//    sfFunc sfRetrievalFunction_ = NULL;       // Member function pointer set in the constructor based on input type.
    std::pair<double, double> getSFFromJSON(double, double);   // Get the lepton scale factor based on its pt and eta (respectively) from JSON.
    std::pair<double, double> getSFFromROOT(double, double);   // Get the lepton scale factor based on its pt and eta (respectively) from ROOT.
    std::pair<double, double> getNonSF(double, double) { return std::make_pair(1.0, 0.0); }  // Returns unity if no SFs successfuly loaded.

    std::pair<float, float> getRangeFromKey(const std::string&);
        // Returns pair of numbers indicating a range from a string formatted like so: "label:[NUM,NUM]"

    boost::property_tree::ptree sfPropTree_;    // Boost property tree containing contents of SF JSON file.
    std::string sfFileName_ ;
    std::string leptonType_ ;
    std::string binningPref_;

    bool extrapolateFromClosestBin_;
    bool jsonTreePopulated_;
    bool loadFromJSONSuccessful_;
    bool loadFromROOTSuccessful_;

    // Max bins. Only valid when working with ROOT file.
    int   histEtaMaxBin_;        // Max bin of histogram eta axis.
    int   histPtMaxBin_ ;        // Max bin of histogram pt axis.

  // Root objects.
    // RECEIVING SEG FAULTS IF THEY AREN'T ADDED AT END OF CLASS DEF. SOMETHING TO LOOK INTO, MAYBE?
    TFile* f_sfFile_ = 0;                       // ROOT file containing SF.
    TH2F* h_sfHisto_ = 0;                       // 2D histogram containing sf.

};

#endif
