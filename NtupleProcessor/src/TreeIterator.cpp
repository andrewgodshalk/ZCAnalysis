/*
TreeIterator.cpp

 Created : 2015-05-14  godshalk
 Modified: 2015-10-30  godshalk

*/

#include <iomanip>
#include <iostream>
#include <string>
#include <TString.h>
#include "../interface/TreeIterator.h"

using std::cout;  using std::endl;   using std::setw;   using std::string;


void TreeIterator::Begin(TTree * /*tree*/){}


void TreeIterator::SlaveBegin(TTree * /*tree*/)
{
  // Initialize log, counters
    nEntries          = 0;
    nEntriesProcessed = 0;
}


void TreeIterator::Init(TTree *tree)
{
  // Initialize the current tree.
    if (!tree) return;
    fChain = tree;              // Commented, don't think I need to access tree information outside of this function.
    fChain->SetMakeClass(1);    // ??? Can't seem to get a good answer about what this is.
                                //    I think it recreates classes stored in the root tree as opposed to just mapping variables from the object.
    nEntries += tree->GetEntries();

  // Initialize Event Handler, adding the criteria of each HistoMaker to it's list of criteria.
    eHandler.mapTree(fChain);
}


Bool_t TreeIterator::Notify()
{
   return kTRUE;
}


Bool_t TreeIterator::Process(Long64_t entry)
{
  // Load current entry
    fChain->GetEntry(entry);
    nEntriesProcessed++;
    if(entry%100000 == 0) cout << "  #" << entry << endl;

  // Evaluate the criteria for this entry
    eHandler.evalCriteria();

  // Call each of the HistogramMakers on this entry.
    //for(HistogramMaker* h : histoMakers.second) log << h->fillHistos(eHandler);

return true;
}

void TreeIterator::SlaveTerminate(){}
void TreeIterator::Terminate(){}
