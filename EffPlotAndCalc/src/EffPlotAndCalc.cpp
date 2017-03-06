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
{ // Set up EffPlotAndCalc

  // Try/catch set up for the sole pupose of catching false return on
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

  // Set up templates for efficiency histograms.
    int    nPtBins_  = 7;
    int    nEtaBins_ = 1;
    double ptBinBounds_[]  = {30,50,70,100,140,200,300,670};
    double etaBinBounds_[] = {-2.4, 2.4};
    h_temp_ = new TH2F("h_temp","", nPtBins_, ptBinBounds_, nEtaBins_, etaBinBounds_);  h_temp_->Sumw2();

    cout << "\n"
            "===================================================================\n"
            "===  calcDiffTagEff \n"  << endl;

  // Attempt to open input file and output file.
    cout << "  Opening file: " << fn_input_ << "..." << endl;
    f_input_  = TFile::Open(fn_input_ .c_str());
    cout << "  Output to file: " << fn_output_ << "...\n" << endl;
    f_output_ = TFile::Open(fn_output_.c_str(), "RECREATE"); f_output_->cd();

  // Set up output histograms
    array<char,3>   flavors = {'l','c','b'};
    // const vector<TString> svTypes = {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
    const vector<TString> svTypes = {"noSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
    const vector<TString> tags    = {"NoHF","CSVL","CSVM","CSVT", "CSVS"};
    for( char& f : flavors)
    {   h_nJets[f]  =    (TH2F*) h_temp_ ->Clone(TString::Format("h_n%cJets", f));
        hr_nJets[f] =    (TH2F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zuu_Z%c/n_%cJets_2D", f, f))
                                         ->Clone(TString::Format("n_%cJets_2D_comb",f));
        hr_nJets[f]->Add((TH2F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zee_Z%c/n_%cJets_2D", f, f)));
      // Rebin plots
        transferContents(hr_nJets[f], h_nJets[f]);
        for( const TString & tag : tags )
            for( const TString & sv : svTypes)
        {   cout << "\tCreating Eff. for conditions: " << f << "," << tag << "," << sv << "..." << endl;
            h_nTaggedJets [f][tag][sv] =    (TH2F*) h_temp_ ->Clone(TString::Format("h_nt%cJets_%s%s"                             ,f,tag.Data(),sv.Data()));
            hr_nTaggedJets[f][tag][sv] =    (TH2F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zuu_Z%c/nt_%cJets_2D_%s%s",f,f,tag.Data(),sv.Data()))
                                                        ->Clone(TString::Format("nt_%cJets_2D_%s%s_comb",f,tag.Data(),sv.Data()));
            hr_nTaggedJets[f][tag][sv]->Add((TH2F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zee_Z%c/nt_%cJets_2D_%s%s",f,f,tag.Data(),sv.Data())));
          // Rebin plots
            transferContents(hr_nTaggedJets [f][tag][sv], h_nTaggedJets[f][tag][sv]);
          // Make eff plot
            h_JetTagEff[f][tag][sv] = makeEffPlots(TString::Format("h_%cJetTagEff_%s%s",f,tag.Data(),sv.Data()), h_nTaggedJets[f][tag][sv], h_nJets[f]);
            printPlotValues(h_JetTagEff[f][tag][sv]);
        }
    }

  // Write histograms to file
    for( char& f : flavors)
    {   //h_nJets [f]->Write();
        //hr_nJets[f]->Write();
        for( const TString & tag : tags )
            for( const TString & sv : svTypes)
                h_JetTagEff[f][tag][sv]->Write();
        // for( const TString & tag : tags ) h_nTaggedJets [f][tag]->Write();
        // for( const TString & tag : tags ) hr_nTaggedJets[f][tag]->Write();
    }
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
    // for( auto& kv : h_nJets  ) delete kv.second;
    // for( auto& kv : hr_nJets ) delete kv.second;
    // for( auto& kv1 : h_nTaggedJets  ) for(auto kv2 : kv1.second) delete kv2.second;
    // for( auto& kv1 : hr_nTaggedJets ) for(auto kv2 : kv1.second) delete kv2.second;
    // for( auto& kv1 : h_JetTagEff    ) for(auto kv2 : kv1.second) delete kv2.second;
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
