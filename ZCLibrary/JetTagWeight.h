#ifndef GUARD_JetTagWeight_h
#define GUARD_JetTagWeight_h

/*------------------------------------------------------------------------------
 JetTagWeight

 Created : 2017-02-03  godshalk
 Modified: 2017-02-03  godshalk

Class set up to apply weights to events based on jets that are heavy-flavor
tagged.

The method of applying efficiency and scale factors as an event weight are
taken from BTVPOG's first recommendation at:
    https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods

------------------------------------------------------------------------------*/

#include <map>
#include <string>
#include <vector>
#include "BTagCalibrationStandalone.h"
#include <TH2F.h>

class JetTagWeight
{
  public:
    JetTagWeight();     // Primary constructor.
   ~JetTagWeight(){}

    bool  setEffFile(const std::string&);
    bool  setSFFile( const std::string&);
    float getJetEff( char, std::string opPt, float, float);
    float getJetSF ( char, std::string opPt, float, float, std::string type="central");

  private:
    std::string fn_eff_;
    std::string fn_btag_sf_;
    bool effLoaded_;
    bool sfLoaded_;

    BTagCalibration btagCalib_;
    std::map<std::string, BTagCalibrationReader> btagCalibReader_;
    std::map<std::string, BTagEntry::OperatingPoint> opPtMap_; // Map of BTagEntry operating points to my operating points.
    std::map<char, BTagEntry::JetFlavor> flvMap_ ;        // Map of flavor enums to my flavor characters.
    std::map<char, std::string> flvSet_ ;  // Set to load for a given flavor.
    std::vector<char> flavor_;
    std::vector<std::string> opPt_;
    std::map<char, std::map<std::string, TH2F*> > jetTagEff_;
};

#endif
