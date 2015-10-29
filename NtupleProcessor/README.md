================================================================================
#NtupleProcessor
* /NtupleProcessor
* /TreeIterator
* /EntryHandler
* /HistogramMaker
* /...


NtupleProcessor is a program used to run through the ntuples created by the
ZCAnalysis/Ntupler and extract information and histograms necessary for the 
analysis of the 

**To compile**:

From ZCAnalysis folder:

```
make -f NtupleProcessor/makefile
```

**To run**

```
./NtupleProcessor.exe \<inputconfigfile\>
```

Three classes are used to extract histograms from the ntuples:


####TreeIterator
Runs through the tree event by event, calling the EntryHandler on each event.

INPUT :
- **Config File** to feed to Entry Handler
- **Maximum number of entries to process** and a **starting entry**

OUTPUT: Simple timestamped log file.


####EntryHandler
Takes a list of criteria to check for each event.

INPUT :
- **Config file** with the list of standard cut parameters

OUTPUT: Simple timestamped log file.


####HistogramMaker
Uses selection variables from EntryHandler to create histograms. This class is a
_virtual class_. The analysis is actually done by child classes inheritted from
this class.

INPUT: Dependent on class.

OUTPUT:
- **Histogram file** in the root format containing the histograms created. 
- **Log file** showing stats on information the HistogramMaker collected.
- **Formatted text files**

================================================================================

