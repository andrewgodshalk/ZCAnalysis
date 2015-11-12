#ifndef GUARD_TreeIterator_h
#define GUARD_TreeIterator_h

/*------------------------------------------------------------------------------
   TreeIterator

 Created : 2015-05-14  godshalk
 Modified: 2015-10-30  godshalk

 Class inheritted from TSelector (from root libraries).
 Created to run over Z+c ntuple trees.
 Moves through a tree, updating the entry number with the EntryHandler.
 The Entry Handler does most of the work.

------------------------------------------------------------------------------*/

#include <map>
#include <sstream>
#include <vector>
#include <TBranch.h>
#include <TChain.h>
#include <TSelector.h>
#include <TTree.h>
#include "EventHandler.h"
#include "HistogramExtractor.h"

typedef unsigned long counter;

class TreeIterator : public TSelector
{
public:
    TreeIterator(EventHandler &eh, std::map<TString, HistogramExtractor*> &he) : eHandler(eh), hExtractors(he), fChain(0) {}
    virtual ~TreeIterator(){}

  // Overloaded TSelector Functions
    virtual Int_t   Version() const { return 2; }
    virtual void    Begin     (TTree *tree);
    virtual void    SlaveBegin(TTree *tree);
    virtual void    Init      (TTree *tree);
    virtual Bool_t  Notify();
    virtual Bool_t  Process   (Long64_t entry);

    virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
    virtual void    SetOption(const char *option) { fOption = option; }
    virtual void    SetObject(TObject *obj)       { fObject = obj;    }
    virtual void    SetInputList(TList *input)    { fInput  = input;  }
    virtual TList  *GetOutputList() const         { return fOutput;   }

    virtual void    SlaveTerminate();
    virtual void    Terminate();

  // Entry handler & list of Histogram Extractors
    EventHandler &eHandler;
    std::map<TString, HistogramExtractor*> &hExtractors;

  // TTree
    TTree *fChain;

  // Counters
    counter nEntries;             // Total number of entries in tree/chain
    counter nEntriesProcessed;    // Total number of events processed from chain

};


#endif
