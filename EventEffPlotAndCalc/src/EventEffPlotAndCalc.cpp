//==============================================================================
// Diff. Event Selection Efficiency Plot Creation.
// EventEffPlotAndCalc
//
// Calculate the differential (pT, eta) event selection efficiency.
//
//==============================================================================

// Header
#include "EventEffPlotAndCalc.h"
// Standard Libraries
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
// ROOT Libraries
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"
#include "TLegend.h"

using std::cout;   using std::array;
using std::endl;   using std::string;
using std::map;    using std::vector;
using std::sqrt;   using std::pair;

// MAIN() - Used only to pass on input to EventEffPlotAndCalc class.
int main(int argc, char* argv[])
{ // Set up EventEffPlotAndCalc

  // Try/catch set up for the sole pupose of catching false return on
    try { EventEffPlotAndCalc ePaC (argc, argv); }
    catch(const char* msg)
    {   if(string(msg) != "help") std::cerr << msg << endl;
        return 1;
    }
    return 0;
}


EventEffPlotAndCalc::EventEffPlotAndCalc(int argc, char* argv[])
{
  // Handle commandl line input
    if(!processCommandLineInput(argc, argv)) throw("help");

    cout << "\n"
            "===================================================================\n"
            "===  calcDiffTagEff \n"  << endl;

  // Set up templates for jet efficiency histograms.
  // Old, nonuniform binning (not as pretty)
    // nPtBins_  = 40;
    // double ptBinBounds_[]   = {30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100,
    //                           110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 220, 240, 260, 280, 300,
    //                           350, 400, 450, 500, 670};
    // h_temp_ = new TH1F("h_temp","", nPtBins_, ptBinBounds_);  h_temp_->Sumw2();
    int maxX = 660;
    int binWidth = 20;
    h_temp_ = new TH1F("h_temp","", maxX/binWidth, 0, maxX);  h_temp_->Sumw2();

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
    { // Set up properly binned storage histos.
        h_nEvts     [f] = (TH1F*) h_temp_ ->Clone(TString::Format("h_nZ%cEvents", f));
        hr_nEvts[f] = (TH1F*) f_input_ ->Get(  TString::Format("raw_eff_plots/dy/Zuu_Z%c/n_Z%cEvents", f, f))
                                         ->Clone(TString::Format("n_Z%cEvents_comb",f));
        // hr_nEvts[f]->Add((TH1F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zee_Z%c/n_Z%cEvents", f, f)));
      // Rebin plots
        transferContents(hr_nEvts[f], h_nEvts[f]);
        for( const TString & tag : tags )
            for( const TString & sv : svTypes)
        {   cout << "\tCreating Eff. for conditions: " << f << "," << tag << "," << sv << "..." << endl;
            h_nTaggedEvts [f][tag][sv] = (TH1F*) h_temp_ ->Clone(TString::Format("h_nt_Z%cEvts_%s%s"                           ,f,tag.Data(),sv.Data()));
            hr_nTaggedEvts[f][tag][sv] = (TH1F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zuu_Z%c/nt_Z%cEvents_%s%s",f,f,tag.Data(),sv.Data()))
                                                         ->Clone(TString::Format("h_nt_Z%cEvts_%s%s_comb"                      ,f,tag.Data(),sv.Data()));
            // hr_nTaggedEvts[f][tag][sv]->Add((TH1F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zee_Z%c/nt_%cJets_2D_%s%s",f,f,tag.Data(),sv.Data())));
          // Rebin plots
            transferContents(hr_nTaggedEvts[f][tag][sv], h_nTaggedEvts[f][tag][sv]);
          // Make eff plot
            h_EvtTagEff[f][tag][sv] = makeEffPlots(TString::Format("h_Z%cEvtTagEff_%s%s",f,tag.Data(),sv.Data()), h_nTaggedEvts[f][tag][sv], h_nEvts[f]);
            //printPlotValues(h_EvtTagEff[f][tag][sv]);
        }
    }
    // Set up histograms from data.
    for( const TString & tag : tags )
        for( const TString & sv : svTypes)
    {   // cout << "\tExtracting Results from Data for conditions: " << tag << "," << sv << "..." << endl;
        h_nTaggedEvts_Data [tag][sv] = (TH1F*) h_temp_ ->Clone(TString::Format("h_nt_ZHFEvts_%s%s"                 ,tag.Data(),sv.Data()));
        hr_nTaggedEvts_Data[tag][sv] = (TH1F*) f_input_->Get(  TString::Format("raw_eff_plots/muon/Zuu/nt_Evt_%s%s",tag.Data(),sv.Data()))
                                                       ->Clone(TString::Format("h_nt_ZHFEvts_%s%s_comb"            ,tag.Data(),sv.Data()));
        // hr_nTaggedEvts[f][tag][sv]->Add((TH1F*) f_input_->Get(  TString::Format("raw_eff_plots/dy/Zee_Z%c/nt_%cJets_2D_%s%s",f,f,tag.Data(),sv.Data())));
      // Rebin plots
        transferContents(hr_nTaggedEvts_Data[tag][sv], h_nTaggedEvts_Data[tag][sv]);
    }

  // For each tag/sv, Calculate the average efficiency for each flavor, then make presentable efficiency plots.
    for( const TString & tag : tags )
        for( const TString & sv : svTypes)
        { for( char& f : flavors) avgEff[f][tag][sv] = calculateAvgEff(h_EvtTagEff[f][tag][sv], h_nTaggedEvts_Data[tag][sv]);
          c_combinedEffPlot[tag][sv] = makeCombinedEffPlots(tag, sv, h_EvtTagEff['b'][tag][sv], h_EvtTagEff['c'][tag][sv], h_EvtTagEff['l'][tag][sv]);
        }

  // Write histograms, canvases to file
    TDirectory* individualEffPlotDir = f_output_->mkdir("IndividualEffPlots");
    individualEffPlotDir->cd();
    for( char& f : flavors)
    {   //h_nJets [f]->Write();
        //hr_nJets[f]->Write();
        for( const TString & tag : tags )
            for( const TString & sv : svTypes)
                h_EvtTagEff[f][tag][sv]->Write();
    }
    TDirectory* combinedEffPlotDir = f_output_->mkdir("CombinedEffPlots");
    combinedEffPlotDir->cd();
    for( const TString & tag : tags )
        for( const TString & sv : svTypes)
            c_combinedEffPlot[tag][sv]->Write();


    f_output_->Close();

    cout << "\n"
            "                                                                ===\n"
            "===================================================================\n"
         << endl;
}


EventEffPlotAndCalc::~EventEffPlotAndCalc()
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

bool EventEffPlotAndCalc::processCommandLineInput(int argc, char* argv[])
{ // Process command line input.
    if(argc != 3)
    {   cout << "Please include two arguements: ./EventEffPlotAndCalc/bin/EventEffPlotAndCalc <INPUT FILE> <OUTPUT FILE>" << endl;
        return false;
    }
    fn_input_  = argv[1];
    fn_output_ = argv[2];
    return true;
}


void EventEffPlotAndCalc::transferContents(TH1F* h_old, TH1F* h_new)
{ // Assumes that bin borders line up, and that old has a finer split than new,
  //  and that the axis of old have a wider range than new.
    int nBinsXOld = h_old->GetNbinsX();
    // int nBinsYOld = h_old->GetNbinsY();
    int nBinsXNew = h_new->GetNbinsX();
  //   int nBinsYNew = h_new->GetNbinsY();
    // cout << "    " << h_old->GetName() << " -> " << h_new->GetName() << endl;
    // cout << TString::Format("    Transferring old (%i,%i) into new (%i, %i)...",nBinsXOld,nBinsYOld,nBinsXNew,nBinsYNew) << endl;
    // cout << TString::Format("    Transferring old (%i) into new (%i)...",nBinsXOld,nBinsXNew) << endl;

  // Set up histogram to store errors.
    TH1F* h_err = (TH1F*) h_new->Clone("h_err");

  // Start transferring contents, starting with underflow bins.
    int ixOld, ixNew;    // Bin indices for old/new histograms
    //int iyOld, iyNew;    // Bin indices for old/new histograms
    for(ixOld=0; ixOld<=nBinsXOld+1; ixOld++)
    { // Transfer the contents
        double xOfBin = h_old->GetXaxis()->GetBinCenter(ixOld);
        double binContent = h_old->GetBinContent(ixOld);
        double binError   = h_old->GetBinError  (ixOld);
        h_new->Fill(xOfBin, binContent       );
        h_err->Fill(xOfBin, binError*binError);
    }
    // 2D work.
    // for(ixOld=0; ixOld<=nBinsXOld+1; ixOld++)
    // { // Transfer the contents
    //     double xOfBin = h_old->GetXaxis()->GetBinCenter(ixOld);
    //     for(iyOld=0; iyOld<=nBinsYOld+1; iyOld++)
    //     {   double yOfBin = h_old->GetYaxis()->GetBinCenter(iyOld);
    //         double binContent = h_old->GetBinContent(ixOld,iyOld);
    //         double binError   = h_old->GetBinError  (ixOld,iyOld);
    //         h_new->Fill(xOfBin, yOfBin, binContent       );
    //         h_err->Fill(xOfBin, yOfBin, binError*binError);
    //     }
    // }

  // Set the error using the error historam
    for(ixNew=0; ixNew<=nBinsXNew+1; ixNew++)
        h_new->SetBinError(ixNew, sqrt(h_err->GetBinContent(ixNew)));
        // for(iyNew=0; iyNew<=nBinsYNew+1; iyNew++)
            // h_new->SetBinError(ixNew, iyNew, sqrt(h_err->GetBinContent(ixNew, iyNew)));

    delete h_err;
}


TH1F* EventEffPlotAndCalc::makeEffPlots(TString effPlotTitle, TH1F* h_num, TH1F* h_den)
{
    TH1F* h_eff = NULL;
    h_eff = (TH1F*) h_num->Clone(effPlotTitle);
    h_eff->Divide(h_den);
  // Set error by bin by bin.
    // REDUNDANT - Sumw2 plots handle these error calculations automatically.
    // int bins = h_num->GetNbinsX();
    // float n_val, n_err, d_val, d_err, /*e_val,*/ e_err;
    // for(int i=1; i<=bins; i++)
    // {   n_val = h_num->GetBinContent(i);   n_err = h_num->GetBinError  (i);
    //     d_val = h_den->GetBinContent(i);   d_err = h_den->GetBinError  (i);
    //     // e_val = h_eff->GetBinContent(i);
    //     if(n_val == 0 || d_val == 0) e_err = 0;
    //     else e_err = sqrt( pow(n_err/d_val, 2) + pow(n_val*d_err/d_val/d_val, 2) );
    //     h_eff->SetBinError(i, e_err);
    //     //cout << "    makeEfficiencyPlot: Calculated eff in bin " << i << ": " << e_val << " +- " << e_err << endl;
    // }
    return h_eff;
}


void EventEffPlotAndCalc::printPlotValues(TH1F* h)
{
    cout << "\t" << h->GetName() << "  =======================" << endl;
    int nBinsX = h->GetNbinsX();

    for(int ix=0; ix <= nBinsX+1; ix++)
    {
      cout << "\t\t";    // Give plot a log axis
      cout << TString::Format("{%0.0f,%0.0f}\t", h->GetXaxis()->GetBinLowEdge(ix), h->GetXaxis()->GetBinUpEdge(ix));
      cout << TString::Format("%0.6f\t", h->GetBinContent(ix)) << endl;
    }
    cout << endl;
}


TCanvas* EventEffPlotAndCalc::makeCombinedEffPlots(TString tag, TString sv, TH1F* ho_b, TH1F* ho_c, TH1F* ho_l)
{ // Takes three efficiency plots and a data distribution of the differential
  // variable. Plots the three plots on a canvas.
    // cout << "Drawing combined efficiency plot for tag/sv: " << tag << "/" << sv << endl;
    TCanvas* plot = new TCanvas(TString("effPlot_")+tag+sv, TString("effPlot_")+tag+sv);
    plot->cd();
    TH1F* h_beff = (TH1F*) ho_b->Clone("h_beff");
    TH1F* h_ceff = (TH1F*) ho_c->Clone("h_ceff");
    TH1F* h_leff = (TH1F*) ho_l->Clone("h_leff");
    // h_beff->SetTitle("Eff. vs. Lead Tagged Jet p_{T};Jet p_{T} (GeV);Efficiency");
    h_beff->SetLineColor(kRed    );  h_beff->SetLineWidth(2.5);  //beff->SetMarkerColor(kRed);  beff->SetMarkerStyle(20);  beff->SetMarkerSize(1);
    h_ceff->SetLineColor(kBlue   );  h_ceff->SetLineWidth(2.5);
    h_leff->SetLineColor(kGreen+2);  h_leff->SetLineWidth(2.5);

  // Set Y-axis to (0.0001, 1)
    h_beff->SetAxisRange(0.001, 1.2, "Y");
  // Give plot a log axis
    plot->SetLogy();

  // Draw them histos.
    h_beff->GetXaxis()->SetTitle("Lead HF Jet p_{T} (GeV)");
    h_beff->GetYaxis()->SetTitle("Event Selection Efficiency");
    h_beff->Draw("hist PE");
    h_ceff->Draw("hist PE sames");
    h_leff->Draw("hist PE sames");

  // Set up legend labels
    TString bLegLabel = TString::Format("Z+b (avg: %0.5f#pm%0.5f)", avgEff['b'][tag][sv].first, avgEff['b'][tag][sv].second);
    TString cLegLabel = TString::Format("Z+c (avg: %0.5f#pm%0.5f)", avgEff['c'][tag][sv].first, avgEff['c'][tag][sv].second);
    TString lLegLabel = TString::Format("Z+l (avg: %0.5f#pm%0.5f)", avgEff['l'][tag][sv].first, avgEff['l'][tag][sv].second);

  // The legend continues
    TLegend *leg = new TLegend(0.5,0.15,0.95,0.35);
    leg->SetHeader(TString::Format("Z+jet Event Selection Eff. for %s/%s", tag.Data(), sv.Data()));
    leg->AddEntry(h_beff, bLegLabel, "L");
    leg->AddEntry(h_ceff, cLegLabel, "L");
    leg->AddEntry(h_leff, lLegLabel, "L");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

    return plot;
}


std::pair<double, double> EventEffPlotAndCalc::calculateAvgEff(TH1F* h_eff, TH1F* ho_wt)
{ // Takes an efficiency histogram and averages the contents of its bins,
  // calculating error contribution from each bin and weighting the each bin
  // by the contents of the bin in the second weight histogram.
  // Efficiency = sum(eff_i*wt_i) over i bins
  // Error = sqrt( (e_eff*wt)^2 + (e_wt*eff)^2 + (eff*wt*sqrtN/N^2)^2)

  // Set up storage for the efficiency and its error.
    double eff  = 0;   double err  = 0;   double err1 = 0;
                       double err2 = 0;   double err3 = 0;
    double bin_eff, bin_err, bin_wt, bin_wterr;
    double wtIntegral = ho_wt->Integral();

  // Clone the data histogram and normalize to 1.
    TH1F* h_wt = (TH1F*) ho_wt->Clone("h_wt");
    h_wt->Scale(1.0/wtIntegral);

  // Iterate through bins, adding the appropriate values to each term.
    int bins = h_wt->GetNbinsX();
    for(int i=1; i<=bins; i++)
    {
        bin_eff   = h_eff->GetBinContent(i);   bin_err   = (bin_eff == 0 ? 0 : h_eff->GetBinError(i));
        bin_wt    = h_wt ->GetBinContent(i);   bin_wterr = h_wt ->GetBinError(i); ///wtIntegral;
        // cout << " BIN WEIGHT TEST: " << ho_wt->GetBinContent(i) << "+-" << ho_wt->GetBinError(i) << " --> " << bin_wt << "+-" << bin_wterr << endl;
        eff  += bin_eff*bin_wt   ;
        err1 += bin_err*bin_wt   ;
        err2 += bin_eff*bin_wterr;
        err3 += bin_eff*bin_wt*sqrt(wtIntegral)/pow(wtIntegral, 2.0);
        //cout << "       err2 calc: bin_eff = " << bin_err << " bin_wterr = " << bin_wterr << endl;
    }
    err = sqrt( err1*err1 + err2*err2 + err3*err3);
    //cout << "       " << setprecision(4) << eff << " " << err1 << " " << err2 << " " << err3 << " " << err << endl;
    delete h_wt;

    return { eff, err };
}
