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
#include <boost/property_tree/ptree.hpp>

class LeptonSFData
{
  public:
    LeptonSFData(std::string, std::string, std::string);  // Primary constructor.
   ~LeptonSFData(){}

   bool loadJSONFromFile(std::string);   // Attempts to SFs from file into a property tree.

  private:
    boost::property_tree::ptree sfPropTree;    // Boost property tree containing contents of SF JSON file.

    std::string jsonFileName_ ;
    std::string leptonType_   ;
    std::string binningPref_  ;

    bool populated_;
};

#endif
