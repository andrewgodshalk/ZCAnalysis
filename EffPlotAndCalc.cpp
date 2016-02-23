/*-------------------------------------------------------------------

  2016-02-17 - Extracts from NtupleExtractor EffPlotExtractor


  g++ -o EffPlotAndCalc.exe -lboost_program_options `root-config --cflags --glibs` EffPlotAndCalc.cpp
  ./EffPlotAndCalc.exe

*/

#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>
#include <sstream>
#include <vector>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TString.h>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::pow;
using std::setprecision;
using std::sqrt;
using std::stringstream;
using std::vector;

TH1F* getDataWeightPlot ( TFile*, TString&, TString&           );
TH1F* getDenominatorPlot( TFile*, TString&, TString&           );
TH1F* getNumeratorPlot  ( TFile*, TString&, TString&, TString& );
TH1F* makeEfficiencyPlot( TH1F*, TH1F*, TString&               );
pair<float,float> calculateEfficiency(TH1F*, TH1F*);

int main()
{
//    PSEUDO:
//    For each channel {Zuu, Zee, Zll}
//      For each HFTag {NoHF, CSVL, ... , CSVS}
//        Extract DataWeights (raw_eff_plots/<CHANNEL>/<CHANNEL>/n_tagged_<HFTag>)
//      For each flavor (Z+b, Z+c, Z+l)
//        Extract Den (raw_eff_plots/dy/Zee_Zf/n_ZfEvents)
//      For each HFTag {NoHF, CSVL, ... , CSVS}
//        For each flavor (Z+b, Z+c, Z+l)
//          Extract Num (raw_eff_plots/dy/Zee_Zf/n_tagged_ZfEvents_CSV#)
//          Make Eff plot (Num & Den)
//          Calculate Weighted Eff (Eff, DWts)
//        Make Plot

    cout << "\n  EffPlotAndCalc.cpp ---- BEGIN \n" << endl;


  // Open inputs, outputs.
    TFile *f_input  = TFile::Open("zc_eff_raw_plots.root"             );
    TFile *f_output = TFile::Open("zc_eff_comb_plots.root", "RECREATE");


    vector<TString> HFTag   = {"NoHF", "CSVL", "CSVM", "CSVT", "CSVS"};
    vector<TString> channel = {"Zuu", "Zee", "Zll"};
    vector<TString> flavor  = {"Zb", "Zc", "Zl"};

    map<TString, map<TString, TH1F*> >                              h_dtwt    ; // [channel][hftag]
    map<TString, map<TString, TH1F*> >                              h_den     ; // [channel][flavor]
    map<TString, map<TString, map<TString, TH1F*> > >               h_num     ; // [channel][hftag][flavor]
    map<TString, map<TString, map<TString, TH1F*> > >               h_eff     ; // [channel][hftag][flavor]
    map<TString, map<TString, map<TString, pair<float, float> > > > efficiency; // [channel][hftag][flavor](central value, error)


  // For each channel...
    for(TString& chLabel : channel)
    {
      // Make a new directory for this channel.
        TDirectory *d = f_output->mkdir(chLabel);
        d->cd();

      // For each flavor...
        d->cd();
        for(TString& flLabel : flavor)
        {
          // Extract data weight plot.
            cout << "    Making Denominator Plot: " << chLabel << " - " << flLabel << endl;
            h_den[chLabel][flLabel] = getDenominatorPlot(f_input, chLabel, flLabel);
            h_den[chLabel][flLabel]->Write();
        }
      //For each HFTag {NoHF, CSVL, ... , CSVS}
        for(TString& hfLabel : HFTag)
        {
          // Make a directory for this HFTag
            TDirectory* d_hf = d->mkdir(hfLabel);
            d_hf->cd();

          // Extract data weight plot.
            cout << "    Making DtWt Plot: " << chLabel << " - " << hfLabel << endl;
            h_dtwt[chLabel][hfLabel] = getDataWeightPlot(f_input, chLabel, hfLabel);
            h_dtwt[chLabel][hfLabel]->Write();

          // For each flavor (Z+b, Z+c, Z+l)
            for(TString& flLabel : flavor)
            {
              // Extract Num (raw_eff_plots/dy/Zee_Zf/n_tagged_ZfEvents_CSV#)
                cout << "    Making Num Plot: " << chLabel << " - " << hfLabel << " - " << flLabel << endl;
                h_num[chLabel][hfLabel][flLabel] = getNumeratorPlot(f_input, chLabel, hfLabel, flLabel);
                h_num[chLabel][hfLabel][flLabel]->Write();

              // Make Eff plot (Num & Den)
                cout << "    Making Eff Plot: " << chLabel << " - " << hfLabel << " - " << flLabel << endl;
                h_eff[chLabel][hfLabel][flLabel] = makeEfficiencyPlot(h_num[chLabel][hfLabel][flLabel], h_den[chLabel][flLabel], flLabel);
                h_eff[chLabel][hfLabel][flLabel]->Write();

              // Calculate Weighted Eff (Eff, DWts)
                efficiency[chLabel][hfLabel][flLabel] = calculateEfficiency(h_eff[chLabel][hfLabel][flLabel], h_dtwt[chLabel][hfLabel]);
                cout << "    Calculating Eff: " << chLabel << " - " << hfLabel << " - " << flLabel << " ==> "
                     << efficiency[chLabel][hfLabel][flLabel].first << " +- " << efficiency[chLabel][hfLabel][flLabel].second << endl;

            }
          // Make Canvas
            TCanvas *plot = new TCanvas("effPlot", "effPlot");
            //plot->cd();
            TH1F* h_beff = (TH1F*) h_eff[chLabel][hfLabel]["Zb"]->Clone("h_beff");
            TH1F* h_ceff = (TH1F*) h_eff[chLabel][hfLabel]["Zc"]->Clone("h_ceff");
            TH1F* h_leff = (TH1F*) h_eff[chLabel][hfLabel]["Zl"]->Clone("h_leff");

          // Reset titles so that they are printed properly
            h_beff->SetTitle("Eff. vs. Jet p_{T};Jet p_{T} (GeV);Efficiency");

          // Pick a color, any color.
            h_beff->SetLineColor(kRed    );  h_beff->SetLineWidth(2.5);  //beff->SetMarkerColor(kRed);  beff->SetMarkerStyle(20);  beff->SetMarkerSize(1);
            h_ceff->SetLineColor(kBlue   );  h_ceff->SetLineWidth(2.5);
            h_leff->SetLineColor(kGreen+2);  h_leff->SetLineWidth(2.5);

          // Set Y-axis to (0.0001, 1)
            h_beff->SetAxisRange(0.0001, 1.0, "Y");

          // Draw them histos.
            h_beff->Draw("hist PE");
            h_ceff->Draw("hist PE sames");
            h_leff->Draw("hist PE sames");

          // Give plot a log axis
            plot->SetLogy();

          // Create legend labels
            stringstream bLabel, cLabel, lLabel;
            bLabel << "Z+b    (avg: " << setprecision(3) <<  efficiency[chLabel][hfLabel]["Zb"].first << "#pm" << efficiency[chLabel][hfLabel]["Zb"].second << ")";
            cLabel << "Z+c    (avg: " << setprecision(3) <<  efficiency[chLabel][hfLabel]["Zc"].first << "#pm" << efficiency[chLabel][hfLabel]["Zc"].second << ")";
            lLabel << "Z+dusg (avg: " << setprecision(3) <<  efficiency[chLabel][hfLabel]["Zl"].first << "#pm" << efficiency[chLabel][hfLabel]["Zl"].second << ")";

          // The legend continues
            TLegend *leg = new TLegend(0.5,0.15,0.95,0.35);
            leg->SetHeader(hfLabel+"+SVT Selection Efficiency");
            leg->AddEntry(h_beff, bLabel.str().c_str(), "L");
            leg->AddEntry(h_ceff, cLabel.str().c_str(), "L");
            leg->AddEntry(h_leff, lLabel.str().c_str(), "L");
            leg->SetBorderSize(0);
            leg->SetFillStyle(0);
            leg->Draw();

            plot->Write();
            delete plot;
            delete h_beff;
            delete h_ceff;
            delete h_leff;
        }
        cout << endl;
    }

//    map<TString, TH1F*> h_num;
//    map<TString, TH1F*> h_dWt;


    cout << "  EffPlotAndCalc.cpp ---- COMPLETE \n" << endl;

return 0;
}


TH1F* getDataWeightPlot(TFile* f, TString& channel, TString& hftag)
{  // Extract DataWeights (raw_eff_plots/<CHANNEL>/<CHANNEL>/n_tagged_<HFTag>)
    TH1F* h_dtwt;
    TString path = "raw_eff_plots/";
    if(channel == "Zuu") h_dtwt = (TH1F*) f->Get(path+"muon/Zuu/n_tagged_"+hftag)->Clone("h_dtwt");
    if(channel == "Zee") h_dtwt = (TH1F*) f->Get(path+"elec/Zee/n_tagged_"+hftag)->Clone("h_dtwt");
    if(channel == "Zll")
    {   h_dtwt       = (TH1F*) f->Get(path+"muon/Zuu/n_tagged_"+hftag)->Clone("h_dtwt");
        TH1F* h_temp = (TH1F*) f->Get(path+"muon/Zuu/n_tagged_"+hftag)->Clone("h_temp");
        h_dtwt->Add(h_temp);
        delete h_temp;
    }
    //cout << "   getDataWeightPlot: Extracted histo: " << h_dtwt->GetTitle() << endl;
    return h_dtwt;
}

TH1F* getDenominatorPlot(TFile* f, TString& channel, TString& flavor)
{  // Extract Den (raw_eff_plots/dy/Zee_Zf/n_ZfEvents)
    TH1F* h_den;
    TString hName = TString("h_")+flavor+"Den";
    TString path = "raw_eff_plots/dy/";
    if(channel == "Zuu") h_den = (TH1F*) f->Get(path+channel+"_"+flavor+"/n_"+flavor+"Events")->Clone(hName);
    if(channel == "Zee") h_den = (TH1F*) f->Get(path+channel+"_"+flavor+"/n_"+flavor+"Events")->Clone(hName);
    if(channel == "Zll")
    {   h_den        = (TH1F*) f->Get(path+"Zuu_"+flavor+"/n_"+flavor+"Events")->Clone(hName);
        TH1F* h_temp = (TH1F*) f->Get(path+"Zee_"+flavor+"/n_"+flavor+"Events")->Clone("h_temp");
        h_den->Add(h_temp);
        delete h_temp;
    }
    //cout << "   getDataWeightPlot: Extracted histo: " << h_dtwt->GetTitle() << endl;
    return h_den;
}

TH1F* getNumeratorPlot(TFile* f, TString& channel, TString& hftag, TString& flavor)
{ // Extract Num (raw_eff_plots/dy/Zee_Zf/n_tagged_ZfEvents_CSV#)
    TH1F* h_num;
    TString hName = TString("h_")+flavor+"Num";
    TString path = "raw_eff_plots/dy/";
    if(channel == "Zuu") h_num = (TH1F*) f->Get(path+channel+"_"+flavor+"/n_tagged_"+flavor+"Events_"+hftag)->Clone(hName);
    if(channel == "Zee") h_num = (TH1F*) f->Get(path+channel+"_"+flavor+"/n_tagged_"+flavor+"Events_"+hftag)->Clone(hName);
    if(channel == "Zll")
    {   h_num        = (TH1F*) f->Get(path+"Zuu_"+flavor+"/n_tagged_"+flavor+"Events_"+hftag)->Clone(hName);
        TH1F* h_temp = (TH1F*) f->Get(path+"Zee_"+flavor+"/n_tagged_"+flavor+"Events_"+hftag)->Clone("h_temp");
        h_num->Add(h_temp);
        delete h_temp;
    }
    //cout << "   getDataWeightPlot: Extracted histo: " << h_dtwt->GetTitle() << endl;
  // Scale numerator by CSV Scale factors.
//    if(hftag == "NoHF") h_num->Scale(1.00);
//    if(hftag == "CSVS") h_num->Scale(0.90);
//    if(hftag == "CSVT") h_num->Scale(0.90);
//    if(hftag == "CSVM") h_num->Scale(0.95);
//    if(hftag == "CSVL") h_num->Scale(0.98);

    return h_num;
}

TH1F* makeEfficiencyPlot( TH1F* h_num, TH1F* h_den, TString &flavor)
{
    TString hName = TString("h_")+flavor+"Eff";
    TH1F* h_eff = (TH1F*) h_num->Clone(hName); //h_eff->Sumw2();
    h_eff->Sumw2();
    h_eff->Divide(h_den);

  // Set error by bin by bin.
    int bins = h_num->GetNbinsX();
    float n_val, n_err, d_val, d_err, e_val, e_err;
    for(int i=1; i<=bins; i++)
    {
        n_val = h_num->GetBinContent(i);
        n_err = h_num->GetBinError  (i);
        d_val = h_den->GetBinContent(i);
        d_err = h_den->GetBinError  (i);
        e_val = h_eff->GetBinContent(i);
        if(n_val == 0 || d_val == 0) e_err = 0;
        else e_err = sqrt( pow(n_err/d_val, 2) + pow(n_val*d_err/d_val/d_val, 2) );
        h_eff->SetBinError(i, e_err);
        //cout << "    makeEfficiencyPlot: Calculated eff in bin " << i << ": " << e_val << " +- " << e_err << endl;
    }

    return h_eff;
}

pair<float,float> calculateEfficiency(TH1F* h_eff, TH1F* h_dtwt)
{
  // Calculate efficiency and error
  // Efficiency = sum(eff_i*wt_i) over i bins
  // Error = sqrt( (e_eff*wt)^2 + (e_wt*eff)^2 + (eff*wt*sqrtN/N^2)^2)

  // Set up storage for the efficiency and its error.
    float eff  = 0;
    float err  = 0;
    float err1 = 0;
    float err2 = 0;
    float err3 = 0;
    float bin_eff, bin_err, bin_wt, bin_wterr;
    float numDataEvents = h_dtwt->Integral();

  // Clone the data histogram and normalize to 1.
    TH1F* h_wt = (TH1F*) h_dtwt->Clone("h_wt");
    h_wt->Scale(1.0/numDataEvents);

  // Iterate through bins, adding the appropriate values to each term.
    int bins = h_wt->GetNbinsX();
    for(int i=1; i<=bins; i++)
    {
        bin_eff   = h_eff->GetBinContent(i);
        bin_err   = h_eff->GetBinError  (i);
        bin_wt    = h_wt ->GetBinContent(i);
        bin_wterr = h_wt ->GetBinError  (i)/numDataEvents;
        eff  += bin_eff*bin_wt   ;
        err1 += bin_err*bin_wt   ;
        err2 += bin_eff*bin_wterr;
        err3 += bin_eff*bin_wt*sqrt(numDataEvents)/pow(numDataEvents, 2.0);
        //cout << "       err2 calc: bin_eff = " << bin_err << " bin_wterr = " << bin_wterr << endl;
    }
    err = sqrt( err1*err1 + err2*err2 + err3*err3);
    //cout << "       " << setprecision(4) << eff << " " << err1 << " " << err2 << " " << err3 << " " << err << endl;
    delete h_wt;
return { eff, err };
}


