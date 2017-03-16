//==============================================================================
// Diff. Jet Tagging Efficiency Plot Creation.
// EffPlotAndCalc
//
// Calculate the differential (pT, eta) heavy flavor jet tagging efficiency.
//
//==============================================================================

// Header
#include "EffPlotAndCalc.h"
// Standard Libraries
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
// ROOT Libraries
#include "TFile.h"
#include "TH2F.h"
#include "TString.h"

using std::cout;   using std::array;
using std::endl;   using std::string;
using std::map;    using std::vector;
using std::sqrt;

// MAIN() - Used only to pass on input to EffPlotAndCalc class.
int main(int argc, char* argv[])
{ // Set up EffPlotAndCalc -  Try/catch set up for the sole pupose of catching false return on
    try { EffPlotAndCalc ePaC (argc, argv); }
    catch(const char* msg)
    {   if(string(msg) != "help") std::cerr << msg << endl;
        return 1;
    }
    return 0;
}

EffPlotAndCalc::EffPlotAndCalc(int argc, char* argv[])
{
  // Handle commandl line input
    if(!processCommandLineInput(argc, argv)) throw("help");

  // Initial output
    cout << "\n"
            "===================================================================\n"
            "===  calcDiffTagEff \n"  << endl;

  // Set up templates for 2D jet efficiency histograms.
    int    nPtBins_  = 7;
    int    nEtaBins_ = 1;
    double ptBinBounds_[]  = {30,50,70,100,140,200,300,670};
    double etaBinBounds_[] = {-2.4, 2.4};
    h_temp_ = new TH2F("h_temp","", nPtBins_, ptBinBounds_, nEtaBins_, etaBinBounds_);
    h_temp_->Sumw2();   h_temp_->SetOption("colz text");

  // Attempt to open input file and output file.
    cout << "  Opening file: " << fn_input_ << "..." << endl;
    f_input_  = TFile::Open(fn_input_ .c_str());
    cout << "  Output to file: " << fn_output_ << "...\n" << endl;
    f_output_ = TFile::Open(fn_output_.c_str(), "RECREATE"); f_output_->cd();

  // Set up arrays of flavors and tags to cycle through
    const vector<TString> datasetLabels = {"dy", "tt", "ww", "wz", "zz"};
    const vector<TString> decayChainLabels = {"Zuu", "Zee", "Zll"};
    // const vector<TString> decayChainLabels = {"Zuu", "Zee"};
    const vector<TString> selectionLabels = {"loose", "zpjmet"};
    // const vector<TString> svTypes = {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
    const vector<TString> svTypes = {"noSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
    const vector<TString> tags    = {"NoHF","CSVL","CSVM","CSVT", "CSVS"};
    array<char,3>   flavors = {'l','c','b'};

  // Set up plot names.
    efficiencyPlotNameFormat    = "%s_%s_%s_%s%s_%cJetEff"    ;
    numeratorPlotNameFormat     = "%s_%s_%s_n_%s%s_%cJets"    ;
    denominatorPlotNameFormat   = "%s_%s_%s_n_NoHFnoSV_%cJets";  // "DS_DC_SEL_n_HFSV_fJets"
    numeratorPlotSourceFormat   = "raw_jet_eff_plots/%s/%s/%s/n_%s%s_%cJets_2D"    ;
    denominatorPlotSourceFormat = "raw_jet_eff_plots/%s/%s/%s/n_NoHFnoSV_%cJets_2D";
    // "raw_jet_eff_plots/<DATASET>/<DECAYCHAIN>/<SELECTION>/nt_<HFTAG><SVTYPE>_<FLAVOR>Jets<1Dvs2D>"
    // Denominator plot is same as tagged plots with no tag selection.

  // Cycle through everything. Extrat plots from file, then create efficiency plots.
    for(     const TString&  ds : datasetLabels    )
      for(   const TString& sel : selectionLabels  )
        for( const TString&  dc : decayChainLabels )
          for(        char& flv : flavors          )
    { // First create the denominator plots, using the NoHFnoSV plots.
        TString denNewPlotLabel = TString::Format(denominatorPlotNameFormat  .Data(), ds.Data(), dc.Data(), sel.Data(), flv );
        h_nJets[denNewPlotLabel] = (TH2F*) h_temp_ ->Clone(denNewPlotLabel);
      // If not making a composite, grab source histograms from file.
        if(dc != "Zll")
        {   TString denSrcPlotLabel = TString::Format(denominatorPlotSourceFormat.Data(), ds.Data(), dc.Data(), sel.Data(), flv );
            hr_nJets[denNewPlotLabel] = (TH2F*) f_input_->Get(denSrcPlotLabel)->Clone(denNewPlotLabel+"_src");
        }
        else
        { // If using Zll, combine previously extracted Zee and Zuu plots.
            TString denZeeNewPlotLabel = TString::Format(denominatorPlotNameFormat.Data(), ds.Data(), "Zee", sel.Data(), flv );
            TString denZuuNewPlotLabel = TString::Format(denominatorPlotNameFormat.Data(), ds.Data(), "Zuu", sel.Data(), flv );
            // hr_nJets[denNewPlotLabel] = (TH2F*) f_input_->Get(denSrcPlotLabel)->Clone(denNewPlotLabel+"_src");
            hr_nJets[denNewPlotLabel] = (TH2F*) hr_nJets[denZeeNewPlotLabel]->Clone(denNewPlotLabel+"_src");
            hr_nJets[denNewPlotLabel]->Add(hr_nJets[denZuuNewPlotLabel]);
        }
      // Rebin source plots into new plots
        transferContents(hr_nJets[denNewPlotLabel], h_nJets[denNewPlotLabel]);

      // Now cycle through tag and sv types, creating numerator plots, then efficiency plots.
        for(     const TString & tag : tags    )
            for( const TString & sv  : svTypes )
        {
            TString numNewPlotLabel = TString::Format(numeratorPlotNameFormat  .Data(), ds.Data(), dc.Data(), sel.Data(), tag.Data(), sv.Data(), flv );
            TString numSrcPlotLabel = TString::Format(numeratorPlotSourceFormat.Data(), ds.Data(), dc.Data(), sel.Data(), tag.Data(), sv.Data(), flv );
            h_nTaggedJets [numNewPlotLabel] = (TH2F*) h_temp_ ->Clone(numNewPlotLabel);
          // If not making a composite, grab source histograms from file.
            if(dc != "Zll")
                hr_nTaggedJets[numNewPlotLabel] = (TH2F*) f_input_->Get(numSrcPlotLabel)->Clone(numNewPlotLabel+"_src");
            else
            { // If using Zll, combine previously extracted Zee and Zuu plots.
                TString numZeeNewPlotLabel = TString::Format(numeratorPlotNameFormat.Data(), ds.Data(), "Zee", sel.Data(), tag.Data(), sv.Data(), flv );
                TString numZuuNewPlotLabel = TString::Format(numeratorPlotNameFormat.Data(), ds.Data(), "Zuu", sel.Data(), tag.Data(), sv.Data(), flv );
                hr_nTaggedJets[numNewPlotLabel] = (TH2F*) hr_nTaggedJets[numZeeNewPlotLabel]->Clone(numNewPlotLabel+"_src");
                hr_nTaggedJets[numNewPlotLabel]->Add(hr_nTaggedJets[numZuuNewPlotLabel]);
            }
          // Rebin source plots into new plots
            transferContents(hr_nTaggedJets[numNewPlotLabel], h_nTaggedJets[numNewPlotLabel]);

          // Make eff plot
            TString effPlotName = TString::Format(efficiencyPlotNameFormat.Data(), ds.Data(), dc.Data(), sel.Data(), tag.Data(), sv.Data(), flv );
            h_JetTagEff[effPlotName] = makeEffPlots(effPlotName, h_nTaggedJets[numNewPlotLabel], h_nJets[denNewPlotLabel]);
            // printPlotValues(h_JetTagEff[effPlotName]);
        }
    }

  // Write histograms to file
    for( auto& label_effPlot : h_JetTagEff )
        label_effPlot.second->Write();
    f_output_->Close();

    cout << "\n"
            "                                                                ===\n"
            "===================================================================\n"
         << endl;
}


EffPlotAndCalc::~EffPlotAndCalc()
{
    delete f_input_;
    delete f_output_;
    delete h_temp_;
}

bool EffPlotAndCalc::processCommandLineInput(int argc, char* argv[])
{ // Process command line input.
    if(argc != 3) return false;
    fn_input_  = argv[1];
    fn_output_ = argv[2];
    return true;
}


void EffPlotAndCalc::transferContents(TH2F* h_old, TH2F* h_new)
{ // Assumes that bin borders line up, and that old has a finer split than new,
  //  and that the axis of old have a wider range than new.
    int nBinsXOld = h_old->GetNbinsX();
    int nBinsYOld = h_old->GetNbinsY();
    int nBinsXNew = h_new->GetNbinsX();
    int nBinsYNew = h_new->GetNbinsY();
    // cout << "    " << h_old->GetName() << " -> " << h_new->GetName() << endl;
    // cout << TString::Format("    Transferring old (%i,%i) into new (%i, %i)...",nBinsXOld,nBinsYOld,nBinsXNew,nBinsYNew) << endl;

  // Set up histogram to store errors.
    TH2F* h_err = (TH2F*) h_new->Clone("h_err");

  // Start transferring contents, starting with underflow bins.
    int ixOld, iyOld, ixNew, iyNew;    // Bin indices for old/new histograms
    for(ixOld=0; ixOld<=nBinsXOld+1; ixOld++)
    { // Transfer the contents
        double xOfBin = h_old->GetXaxis()->GetBinCenter(ixOld);
        for(iyOld=0; iyOld<=nBinsYOld+1; iyOld++)
        {   double yOfBin = h_old->GetYaxis()->GetBinCenter(iyOld);
            double binContent = h_old->GetBinContent(ixOld,iyOld);
            double binError   = h_old->GetBinError  (ixOld,iyOld);
            h_new->Fill(xOfBin, yOfBin, binContent       );
            h_err->Fill(xOfBin, yOfBin, binError*binError);
        }
    }

  // Set the error using the error historam
    for(ixNew=0; ixNew<=nBinsXNew+1; ixNew++)
        for(iyNew=0; iyNew<=nBinsYNew+1; iyNew++)
            h_new->SetBinError(ixNew, iyNew, sqrt(h_err->GetBinContent(ixNew, iyNew)));

    delete h_err;
}


TH2F* EffPlotAndCalc::makeEffPlots(TString effPlotTitle, TH2F* h_num, TH2F* h_den)
{
    TH2F* h_eff = (TH2F*) h_num->Clone(effPlotTitle);
    h_eff->Divide(h_den);
    return h_eff;
}


void EffPlotAndCalc::printPlotValues(TH2F* h)
{
    cout << "\t" << h->GetName() << "  =======================" << endl;
    int nBinsX = h->GetNbinsX();
    int nBinsY = h->GetNbinsY();

    cout << "\t\t\t";
    for(int ix=0; ix <= nBinsX+1; ix++)
        cout << TString::Format("{%0.0f,%0.0f}\t", h->GetXaxis()->GetBinLowEdge(ix), h->GetXaxis()->GetBinUpEdge(ix));
    cout << endl;

    for(int iy=0; iy <= nBinsY+1; iy++)
    {   cout << TString::Format("\t{%0.1f,%0.1f}\t", h->GetYaxis()->GetBinLowEdge(iy), h->GetYaxis()->GetBinUpEdge(iy));
        for(int ix=0; ix <= nBinsX+1; ix++)
        {
            cout << TString::Format("%0.6f\t", h->GetBinContent(ix, iy));
        }
        cout << endl;
    }
    cout << endl;
}
