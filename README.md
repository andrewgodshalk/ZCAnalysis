# ZCAnalysis

Code that runs through Z+c analysis.

To compile, run: g++ -o ZCNtupleProcessor main.cpp `root-config --cflags --glibs`
To run, type: ZCNtupleProcess <inputconfig>


====

ZCNtupleProcessor
  /TreeIterator
  /EntryHandler
  /HistogramMaker

Three classes used to extract histograms from the ntuples.
INPUT: 

TreeIterator - Runs through the tree event by event, calling the EntryHandler on each event.
INPUT : Ntuple to run over, EntryHandler to run through steps on each entry.
OUTPUT: 

EntryHandler - Takes a list of criteria to check for each event.
INPUT : Config file with the list of criteria
OUTPUT: 


====
