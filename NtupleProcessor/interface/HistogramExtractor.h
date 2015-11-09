#ifndef GUARD_HistogramExtractor_h
#define GUARD_HistogramExtractor_h

/*------------------------------------------------------------------------------
   HistogramExtractor Virtual Class

 Created : 2015-05-14  godshalk
 Modified: 2015-11-09  godshalk

 Virtual class used as a base for classes that extract information from ntuples
 in the form of ROOT histograms.

 Designed to save the histograms as output to a folder in a root object.

------------------------------------------------------------------------------*/

#include <map>
#include <TH1.h>
#include <TString.h>
#include "EventHandler.h"
#include "../../ZCLibrary/timestamp.h"


class HistogramExtractor {
  public:
    virtual ~HistogramExtractor(){}
    HistogramExtractor(TString fn, TString o = "") : fn_output(fn), fTimeStamp(fileTimeStamp()), options(o){}

  // Function that fills histograms based on event criteria
    virtual void fillHistos(EntryHandler*){}

  // Function that saves the histograms, log, and performs any final actions.
    virtual void saveToFile(){}

  // Histogram collection
    std::map<TString, TH1*> h;    // List of histograms that will be filled by the fillHistos function

  // File information for output root and log files.
    TString fn_output;   // output filename.
    TFile  *outFile;     // Output file where histograms are stored.
    TString fTimeStamp;  // Time program was started, stored for use in file timestamps.

  // Option string
    TString options;

};

#endif