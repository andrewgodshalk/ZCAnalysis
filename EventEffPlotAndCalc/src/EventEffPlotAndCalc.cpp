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
    double ptBinBounds_[] = {30, 50, 100, 140, 200, 300, 670};
    nPtBins_ = 6;
    h_temp_ = new TH1F("h_temp","", nPtBins_, ptBinBounds_);  h_temp_->Sumw2();
    // int maxX = 660;
    // int binWidth = 20;
    // h_temp_ = new TH1F("h_temp","", maxX/binWidth, 0, maxX);  h_temp_->Sumw2();

  // Attempt to open input file and output file.
    cout << "  Opening file: " << fn_input_ << "..." << endl;
    f_input_  = TFile::Open(fn_input_ .c_str());
    cout << "  Output to file: " << fn_output_ << "...\n" << endl;
    f_output_ = TFile::Open(fn_output_.c_str(), "RECREATE"); f_output_->cd();

  // Set up output histograms
    array<char,3>   flavors = {'l','c','b'};
    const vector<TString> channels = {"Zuu", "Zee"};
    const vector<TString> tags     = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS", "ChmL", "ChmM", "ChmT"};
    const vector<TString> svTypes  = {"noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
    const vector<TString> uncVar   = {"central", "sfHFp", "sfHFm", "sfLp", "sfLm"};

    for( const TString& channel : channels )
        for(      char&       f : flavors  )
    { // Set up properly binned storage histos.
        cout << "Retrieving plots for " << f << "...";
        h_nEvts [channel][f] = (TH1F*) h_temp_ ->Clone(TString::Format("h%s_nZ%cEvents"                                          , channel.Data(), f));
        hr_nEvts[channel][f] = (TH1F*) f_input_->Get(  TString::Format("raw_event_eff_plots/dy/%s_Z%c/nZfEvents_NoHFnoSV_central", channel.Data(), f))
                                               ->Clone(TString::Format("n%s_Z%cEvents_original"                                  , channel.Data(), f));
        cout << " done." << endl;
      // Rebin plots
        transferContents(hr_nEvts[channel][f], h_nEvts[channel][f]);
        for(         const TString & tag : tags    )
            for(     const TString &  sv : svTypes )
                for( const TString & unc : uncVar  )
        {   cout << "\tCreating Eff. for conditions: " << f << "," << tag << "," << sv << "," << unc << "...";
            h_nTaggedEvts [channel][f][tag][sv][unc]
                = (TH1F*) h_temp_ ->Clone(TString::Format("h%s_nt_Z%cEvts_%s%s_%s"                         ,channel.Data(),f,tag.Data(),sv.Data(),unc.Data()));
            hr_nTaggedEvts[channel][f][tag][sv][unc]
                = (TH1F*) f_input_->Get(  TString::Format("raw_event_eff_plots/dy/%s_Z%c/nZfEvents_%s%s_%s",channel.Data(),f,tag.Data(),sv.Data(),unc.Data()))
                                  ->Clone(TString::Format("h%s_nt_Z%cEvts_%s%s_%s_original"                ,channel.Data(),f,tag.Data(),sv.Data(),unc.Data()));
          // Rebin plots
            cout << " done. Transferring contents." << endl;
            transferContents(hr_nTaggedEvts[channel][f][tag][sv][unc], h_nTaggedEvts[channel][f][tag][sv][unc]);
          // Make eff plot
            h_EvtTagEff[channel][f][tag][sv][unc]
                = makeEffPlots( TString::Format( "h%s_Z%cEvtTagEff_%s%s_%s",channel.Data(),f,tag.Data(),sv.Data(),unc.Data()) ,
                                h_nTaggedEvts[channel][f][tag][sv][unc],
                                h_nEvts[channel][f]
                              );
            //printPlotValues(h_EvtTagEff[channel][f][tag][sv][unc]);
        }
    }

    // Set up histograms from data.
    TString dsName = "";
    cout << "Retrieving plots from data..." << endl;
    for(             const TString& channel : channels )
        for(         const TString&     tag : tags     )
            for(     const TString&      sv : svTypes  )
    {   cout << "\tExtracting Results from Data for conditions: " << channel << "," << tag << "," << sv << "...";
        if(channel == "Zuu") dsName = "muon";
        if(channel == "Zee") dsName = "elec";
        h_nTaggedEvts_Data[channel][tag][sv]
            = (TH1F*) h_temp_ ->Clone(TString::Format("h%s_nt_ZHFEvts_%s%s",          channel.Data(),tag.Data(),sv.Data()));
        hr_nTaggedEvts_Data[channel][tag][sv]
            = (TH1F*) f_input_->Get(  TString::Format("raw_event_eff_plots/%s/%s/nZfEvents_%s%s_central",
                                                                        dsName.Data(),channel.Data(),tag.Data(),sv.Data()))
                              ->Clone(TString::Format("h%s_nt_ZHFEvts_%s%s_original", channel.Data(),tag.Data(),sv.Data()));
        cout << " done. Rebinning contents." << endl;
      // Rebin plots
        transferContents(hr_nTaggedEvts_Data[channel][tag][sv], h_nTaggedEvts_Data[channel][tag][sv]);
    }

  // For each tag/sv, Calculate the average efficiency for each flavor, then make presentable efficiency plots.
    for(             const TString& channel : channels )
        for(         const TString&     tag : tags     )
            for(     const TString&      sv : svTypes  )
            {   for( const TString&     unc : uncVar   )
                    for( char& f : flavors) avgEff[channel][f][tag][sv][unc] = calculateAvgEff(h_EvtTagEff[channel][f][tag][sv][unc], h_nTaggedEvts_Data[channel][tag][sv]);
                c_combinedEffPlot[channel][tag][sv] = makeCombinedEffPlots(channel, tag, sv, h_EvtTagEff[channel]['b'][tag][sv], h_EvtTagEff[channel]['c'][tag][sv], h_EvtTagEff[channel]['l'][tag][sv]);
            }

  // Write histograms, canvases to file
    TDirectory* individualEffPlotDir = f_output_->mkdir("IndividualEffPlots");
    individualEffPlotDir->cd();
    for(                 const TString& channel : channels )
        for(                      char&       f : flavors  )
            for(         const TString&     tag : tags     )
                for(     const TString&      sv : svTypes  )
                    for( const TString&     unc : uncVar   )
                    h_EvtTagEff[channel][f][tag][sv][unc]->Write();
    TDirectory* combinedEffPlotDir = f_output_->mkdir("CombinedEffPlots");
    combinedEffPlotDir->cd();
    for(         const TString& channel : channels )
        for(     const TString&     tag : tags     )
            for( const TString&      sv : svTypes  )
                c_combinedEffPlot[channel][tag][sv]->Write();


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
}

bool EventEffPlotAndCalc::processCommandLineInput(int argc, char* argv[])
{ // Process command line input.
    if(argc != 3)
    {   cout << "Please specify two arguments: ./EventEffPlotAndCalc/bin/EventEffPlotAndCalc <INPUT FILE> <OUTPUT FILE>" << endl;
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
    // TString bLegLabel = TString::Format("Z+b (avg: %0.5f#pm%0.5f)", avgEff['b'][tag][sv].first, avgEff['b'][tag][sv].second);
    // TString cLegLabel = TString::Format("Z+c (avg: %0.5f#pm%0.5f)", avgEff['c'][tag][sv].first, avgEff['c'][tag][sv].second);
    // TString lLegLabel = TString::Format("Z+l (avg: %0.5f#pm%0.5f)", avgEff['l'][tag][sv].first, avgEff['l'][tag][sv].second);

  // The legend continues
    TLegend *leg = new TLegend(0.5,0.15,0.95,0.35);
    leg->SetHeader(TString::Format("Z+jet Event Selection Eff. for %s/%s", tag.Data(), sv.Data()));
    // leg->AddEntry(h_beff, bLegLabel, "L");
    // leg->AddEntry(h_ceff, cLegLabel, "L");
    // leg->AddEntry(h_leff, lLegLabel, "L");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

    return plot;
}


TCanvas* EventEffPlotAndCalc::makeCombinedEffPlots(TString channel, TString tag, TString sv, std::map<TString, TH1F*>& ho_b, std::map<TString, TH1F*>& ho_c, std::map<TString, TH1F*>& ho_l)
{
  // Takes three efficiency plots and a data distribution of the differential
  // variable. Plots the three plots on a canvas.
    // cout << "Drawing combined efficiency plot for tag/sv: " << tag << "/" << sv << endl;
    TCanvas* plot = new TCanvas(TString("effPlot_")+channel+"_"+tag+sv, TString("effPlot_")+channel+"_"+tag+sv);
    plot->cd();
    TH1F* h_beff = (TH1F*) ho_b["central"]->Clone("h_beff");
    TH1F* h_ceff = (TH1F*) ho_c["central"]->Clone("h_ceff");
    TH1F* h_leff = (TH1F*) ho_l["central"]->Clone("h_leff");
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

  // Calculate systematics
    double central_b      = avgEff[channel]['b'][tag][sv]["central"].first;
    double sysUnc_b_sfHFp = avgEff[channel]['b'][tag][sv]["sfHFp"  ].first - central_b;
    double sysUnc_b_sfHFm = avgEff[channel]['b'][tag][sv]["sfHFm"  ].first - central_b;
    double sysUnc_b_sfLp  = avgEff[channel]['b'][tag][sv]["sfLp"   ].first - central_b;
    double sysUnc_b_sfLm  = avgEff[channel]['b'][tag][sv]["sfLm"   ].first - central_b;
    double sysUnc_b_sfHF  = (sysUnc_b_sfHFp - sysUnc_b_sfHFm)/2;
    double sysUnc_b_sfL   = (sysUnc_b_sfLp  - sysUnc_b_sfLm )/2;
    double sysUnc_b       = sqrt(sysUnc_b_sfL*sysUnc_b_sfL + sysUnc_b_sfHF*sysUnc_b_sfHF);

    double central_c      = avgEff[channel]['c'][tag][sv]["central"].first;
    double sysUnc_c_sfHFp = avgEff[channel]['c'][tag][sv]["sfHFp"  ].first - central_c;
    double sysUnc_c_sfHFm = avgEff[channel]['c'][tag][sv]["sfHFm"  ].first - central_c;
    double sysUnc_c_sfLp  = avgEff[channel]['c'][tag][sv]["sfLp"   ].first - central_c;
    double sysUnc_c_sfLm  = avgEff[channel]['c'][tag][sv]["sfLm"   ].first - central_c;
    double sysUnc_c_sfHF  = (sysUnc_c_sfHFp - sysUnc_c_sfHFm)/2;
    double sysUnc_c_sfL   = (sysUnc_c_sfLp  - sysUnc_c_sfLm )/2;
    double sysUnc_c       = sqrt(sysUnc_c_sfL*sysUnc_c_sfL + sysUnc_c_sfHF*sysUnc_c_sfHF);

    double central_l      = avgEff[channel]['l'][tag][sv]["central"].first;
    double sysUnc_l_sfHFp = avgEff[channel]['l'][tag][sv]["sfHFp"  ].first - central_l;
    double sysUnc_l_sfHFm = avgEff[channel]['l'][tag][sv]["sfHFm"  ].first - central_l;
    double sysUnc_l_sfLp  = avgEff[channel]['l'][tag][sv]["sfLp"   ].first - central_l;
    double sysUnc_l_sfLm  = avgEff[channel]['l'][tag][sv]["sfLm"   ].first - central_l;
    double sysUnc_l_sfHF  = (sysUnc_l_sfHFp - sysUnc_l_sfHFm)/2;
    double sysUnc_l_sfL   = (sysUnc_l_sfLp  - sysUnc_l_sfLm )/2;
    double sysUnc_l       = sqrt(sysUnc_l_sfL*sysUnc_l_sfL + sysUnc_l_sfHF*sysUnc_l_sfHF);


  // Set up legend labels
    TString unc = "central";
    TString bLegLabel = TString::Format("Z+b (avg: %0.5f #pm %0.5f (st) #pm %0.5f (sy))", avgEff[channel]['b'][tag][sv][unc].first, avgEff[channel]['b'][tag][sv][unc].second, sysUnc_b);
    TString cLegLabel = TString::Format("Z+c (avg: %0.5f #pm %0.5f (st) #pm %0.5f (sy))", avgEff[channel]['c'][tag][sv][unc].first, avgEff[channel]['c'][tag][sv][unc].second, sysUnc_c);
    TString lLegLabel = TString::Format("Z+l (avg: %0.5f #pm %0.5f (st) #pm %0.5f (sy))", avgEff[channel]['l'][tag][sv][unc].first, avgEff[channel]['l'][tag][sv][unc].second, sysUnc_l);

  // The legend continues
    TLegend *leg = new TLegend(0.26,0.12,0.91,0.34);
    leg->SetHeader(TString::Format("%s+jet Event Selection Eff. for %s/%s", channel.Data(), tag.Data(), sv.Data()));
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
