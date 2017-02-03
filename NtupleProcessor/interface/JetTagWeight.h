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

#include "EventHandler.h"

class JetTagWeight
{
  public:
    JetTagWeight();     // Primary constructor.
   ~JetTagWeight(){}

    bool setEffFile(const std::string&);
    bool setSFFile( const std::string&);
    getEventWeight(EventHandler *e);

  private:
    std::string fn_eff_;
    std::string fn_btag_sf_;
};

#endif
