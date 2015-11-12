/*
  ControlPlotExtractor.cpp
 
   Created on: May 19, 2015
       Author: godshalk
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TH2F.h>
#include "../interface/ControlPlotExtractor.h"

using std::stringstream;   using std::string;     using std::cout;
using std::endl;           using std::ofstream;
typedef unsigned int Index;

ControlPlotConfig ControlPlotExtractor::cfg = ControlPlotConfig();

ControlPlotExtractor::ControlPlotExtractor(EventHandler& eh, TDirectory* d, TString o) : HistogramExtractor(eh, d, o)
{
    cout << "    ControlPlotExtractor: Created.\n"
            "      Options: " << options
         << endl;

/*  // Initialize cut flow counter
    nEvents["Analyzed in PAT"          ] = 0;
    nEvents["Candidates from Ntupler"  ] = 0;
    nEvents["w/ 2 valid muons"         ] = 0;
    nEvents["w/ 2 valid electrons"     ] = 0;
    nEvents["w/ 2 valid leptons"       ] = 0;
    nEvents["w/ valid Z from muons"    ] = 0;
    nEvents["w/ valid Z from electrons"] = 0;
    nEvents["w/ valid Z"               ] = 0;
    nEvents["Zuu+j"                    ] = 0;
    nEvents["Zee+j"                    ] = 0;
    nEvents["Z+j"                      ] = 0;
    nEvents["Zuu+HF"                   ] = 0;
    nEvents["Zee+HF"                   ] = 0;
    nEvents["Z+HF"                     ] = 0;
    nEvents["Zee+j w/ minMET"          ] = 0;
    nEvents["Zuu+j w/ minMET"          ] = 0;
    nEvents["Z+j w/ minMET"            ] = 0;
    nEvents["Zee+HF w/ minMET"         ] = 0;
    nEvents["Zuu+HF w/ minMET"         ] = 0;
    nEvents["Z+HF w/ minMET"           ] = 0;
*/

  // Extract options
    usingSim         = (options.Contains("Sim"       , TString::kIgnoreCase) ? true : false);
    usingDY          = (options.Contains("DY"        , TString::kIgnoreCase) ? true : false);
    selectingZee     = (options.Contains("Zee"       , TString::kIgnoreCase) ? true : false);
    selectingZuu     = (options.Contains("Zuu"       , TString::kIgnoreCase) ? true : false);
    selectingZl      = (options.Contains("Zl"        , TString::kIgnoreCase) ? true : false);
    selectingZc      = (options.Contains("Zc"        , TString::kIgnoreCase) ? true : false);
    selectingZb      = (options.Contains("Zb"        , TString::kIgnoreCase) ? true : false);
    selectingZtautau = (options.Contains("Ztautau"   , TString::kIgnoreCase) ? true : false);

  // Initialize histograms from config file.
    hDir->cd();     // Move to this extractor's output directory to initialize the histograms.
    TString histoName, binStr, minStr, maxStr, histoTitle;
    for(auto& kv : cfg.h_strings)
    {
      // Extract histogram information from config file strings.
        histoName  = kv.first;
        binStr     = kv.second[0];
        minStr     = kv.second[1];
        maxStr     = kv.second[2];
        histoTitle = kv.second[3];
        for(int i=4; i<kv.second.size(); i++) histoTitle += " " + kv.second[i];

      // Check if histo already exists in the directory. If so, overwrite.
        // TO DO: Overwrite handling.
        cout << "    ControlPlotExtractor::ControlPlotExtractor(): Creating histogram " << histoName << endl;
        h[histoName] = new TH1F(histoName, histoTitle, atoi(binStr), atof(minStr), atof(maxStr));
        hDir->WriteTObject(h[histoName], 0, "Overwrite");
    }

}


void ControlPlotExtractor::fillHistos()
{
  // If event isn't in JSON and isn't a simulation event, move on to the next event.
    if(!evt.inJSON && !usingSim) return;

  // If you don't have two valid leptons of the desired flavor, kick.
    if(selectingZuu && !evt.validMuons    ) return;
    if(selectingZee && !evt.validElectrons) return;
    //cout << "    ControlPlotExtractor(" << options << ")::fillHistos(): Found event w/ valid leptons." << endl;

  // If using a DY, check starting conditions to see if a certain type of event is desired.
    if(usingDY)
    {   if(selectingZtautau  && !evt.zBosonFromTaus        ) return;
        else if( selectingZb && !evt.hasBJet               ) return;
        else if( selectingZc && !evt.hasCJet               ) return;
        else if( selectingZl && (evt.hasBJet||evt.hasCJet) ) return;
    }

  // Fill raw histograms.
    cout << "    Filling raw histos (ex. raw_Z_mass = " << evt.m_Z_mass << ")." << endl;
    cout << h["raw_Z_mass"  ]->Fill(evt.m_Z_mass) << endl;
    //h["raw_Z_mass"  ]->Fill(evt.m_Z_mass);
    h["raw_Z_pt"    ]->Fill(evt.m_Z_pt  );
    h["raw_Z_eta"   ]->Fill(evt.m_Z_eta );
    h["raw_Z_phi"   ]->Fill(evt.m_Z_phi );
    h["raw_Z_DelR"  ]->Fill(evt.Z_DelR  );
    h["raw_Z_DelEta"]->Fill(evt.Z_DelEta);
    h["raw_Z_DelPhi"]->Fill(evt.Z_DelPhi);

}


void ControlPlotExtractor::saveToFile()
{
  // Save each file to directory or overwrite.
    cout << "   ControlPlotExtractor::saveToFile(): TEST: Saving to file: " << hDir->GetPath() << endl;
    hDir->cd();
    for(const auto& hist : h) hist.second->Write("", TObject::kOverwrite);

}
