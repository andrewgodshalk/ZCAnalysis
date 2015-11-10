/*
TreeIterator.cpp

 Created : 2015-05-14  godshalk
 Modified: 2015-10-30  godshalk

*/

#include <iomanip>
#include <iostream>
#include <string>
#include <TString.h>
//#include "../../ZCLibrary/timestamp.h"    // Project specific
#include "../interface/TreeIterator.h"

using std::cout;  using std::endl;   using std::setw;   using std::string;


void TreeIterator::Begin(TTree * /*tree*/){}


void TreeIterator::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  // Initialize log, counters
    nEntries          = 0;
    nEntriesProcessed = 0;


  // TEST
    cout << "    TreeIterator::SlaveBegin: Initialized." << endl;
}


void TreeIterator::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize a new tree or chain.
  // Map tree variables to local in this function.
  // Init() will be called many times when running on PROOF (once per file to be processed).

  // Initialize the current tree.
    if (!tree) return;
    fChain = tree;              // Commented, don't think I need to access tree information outside of this function.
    fChain->SetMakeClass(1);    // ??? Can't seem to get a good answer about what this is.
                                //    I think it recreates classes stored in the root tree as opposed to just mapping variables from the object.
    nEntries += tree->GetEntries();

  // Initialize Event Handler, adding the criteria of each HistoMaker to it's list of criteria.
    //eHandler = new EntryHandler(fChain, GetOption());

}


Bool_t TreeIterator::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


Bool_t TreeIterator::Process(Long64_t entry)
{
  // This function should contain the "body" of the analysis.
  // It can contain simple or elaborate selection criteria, run algorithms on the data of the event and typically fill histograms.
  // The processing can be stopped by calling Abort().

  // Load current entry
    fChain->GetEntry(entry);
    nEntriesProcessed++;
    if(entry%100000 == 0) cout << "  #" << entry << endl;
    //cout << "  #" << entry << endl;

  // Evaluate the criteria for this entry
    eHandler->evalCriteria();
    //cout << "  Entry #" << entry << ": Criteria Evaluated" << endl;

// Call each of the HistogramMakers on this entry.
    //for(HistogramMaker* h : histoMakers) log << h->fillHistos(eHandler);

return true;
}


void TreeIterator::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

// Call each of the HistogramMakers and close it out
    //for(HistogramMaker* h : histoMakers) log << h->saveToFile();


}


void TreeIterator::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.



// Create the log file text.
//    log << "\n"
//           "  ===TreeIterator ===\n"
//           "  "  << timeStamp() << "\n"
//           "  Counts -\n"
//           "    Entries in chain: " << setw(14) << nEntries          << "\n"
//           "    Events Processed: " << setw(14) << nEntriesProcessed << "\n"
//           "\n\n";

  // Call Terminate on each of the HistogramMakers

  // Output the log.
    //cout << log.str();

}


//void TreeIterator::addHistogramMaker(HistogramMaker* h)
//{
//  // Add the histogram maker to the list that needs to be processed for each entry.
//    //histoMakers.push_back(h);
//
//}
