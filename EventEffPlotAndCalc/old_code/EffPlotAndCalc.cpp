/*-------------------------------------------------------------------

  2016-02-17 - Extracts from NtupleExtractor EffPlotExtractor

  2016-03-04 - MODIFIED TO WORK PER JET, NOT PER EVENT.
              See older versions for per-event.
              Modify to deal with both.


  g++ EffPlotAndCalc.cpp -o EffPlotAndCalc.exe -lboost_program_options `root-config --cflags --glibs`
  ./JetEffPlotAndCalc.exe

TO DO - implement an event selection profile *INSTEAD* of having a 4-layer map/vector/whatever.

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
using std::max;

enum errorFactor {CENTRAL = 0, MINUS = -1, PLUS = +1};

TH1F* getDataWeightPlot ( TFile*, TString&, TString&                        );
TH1F* getDenominatorPlot( TFile*, TString&, TString&                        );
TH1F* getNumeratorPlot  ( TFile*, TString&, TString&, TString&, errorFactor );
TH1F* makeEfficiencyPlot( TH1F* , TH1F*   , TString&                        );
pair<float,float> calculateEfficiency(TH1F*, TH1F*                          );

int main(int argc, char* argv[])
{

    cout << "\n  EffPlotAndCalc.cpp ---- BEGIN \n" << endl;

    float n_num = 0;
    float n_den = 0;
    float n_eff = 0;

  // Retrieve input file suffix from command line (or use default).
  //  TString fn_input  = ( argc > 1 ? argv[1] : "zc_eff_raw_plots"     ".root");
  //  TString fn_output = ( argc > 2 ? argv[2] : "zc_eff_jet_comb_plots"".root");
    TString    fn_label =  "_"    ;
    if(argc>1) fn_label += argv[1];
    else       fn_label =  ""     ;

  //  TString fn_input  = TString("zc_eff_raw_plots"     )+fn_label+".root";
    TString fn_input  = "zc_eff_raw_plots.root";
    TString fn_output = TString("zc_eff_evt_comb_plots")+fn_label+".root";


  // Open inputs, outputs.
    TFile *f_input  = TFile::Open(fn_input             );
    TFile *f_output = TFile::Open(fn_output, "RECREATE");


    vector<TString> HFTag   = {"NoHF", "SVT", "CSVL", "CSVM", "CSVT", "CSVS"};
    vector<TString> channel = {"Zuu", "Zee", "Zll"};
    vector<TString> flavor  = {"Zb", "Zc", "Zl"};
  //  vector<TString> flavor  = {"b", "c", "l"};
    vector<errorFactor> errorFactorList = {MINUS, CENTRAL, PLUS};

    map<TString, map<TString, TH1F*> >                                                 h_dtwt    ; // [channel][hftag]
    map<TString, map<TString, TH1F*> >                                                 h_den     ; // [channel][flavor]
    map<TString, map<TString, map<TString, map<errorFactor, TH1F*              > > > > h_num     ; // [channel][hftag][flavor][errfactor]
    map<TString, map<TString, map<TString, map<errorFactor, TH1F*              > > > > h_eff     ; // [channel][hftag][flavor][errfactor]
    map<TString, map<TString, map<TString, map<errorFactor, pair<float, float> > > > > efficiency; // [channel][hftag][flavor][errfactor](central value, error)
    map<TString, map<TString, map<TString, float> > >                                  systErr   ; // [channel][hftag][flavor](error)


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
              // For each error factor...
                for(errorFactor& ef : errorFactorList)
                {
                  // Extract Num (raw_eff_plots/dy/Zee_Zf/n_tagged_ZfEvents_CSV#)
                  // Extract Num (raw_eff_plots/dy1j/Zee_Zf/n_tagged_ZfEvents_CSV#)
                    cout << "    Making Num Plot: " << chLabel << " - " << hfLabel << " - " << flLabel << endl;
                    h_num[chLabel][hfLabel][flLabel][ef] = getNumeratorPlot(f_input, chLabel, hfLabel, flLabel, ef );
                    h_num[chLabel][hfLabel][flLabel][ef]->Write();

                  // Make Eff plot (Num & Den)
                    cout << "    Making Eff Plot: " << chLabel << " - " << hfLabel << " - " << flLabel << endl;
                    h_eff[chLabel][hfLabel][flLabel][ef] = makeEfficiencyPlot(h_num[chLabel][hfLabel][flLabel][ef], h_den[chLabel][flLabel], flLabel);
                    h_eff[chLabel][hfLabel][flLabel][ef]->Write();

                  // Calculate simple efficiency
                    n_num = h_num[chLabel][hfLabel][flLabel][ef]->Integral();
                    n_den = h_den[chLabel][flLabel]             ->Integral();
                    n_eff = n_num/n_den;

                  // Calculate Weighted Eff (Eff, DWts)
                    efficiency[chLabel][hfLabel][flLabel][ef] = calculateEfficiency(h_eff[chLabel][hfLabel][flLabel][ef], h_dtwt[chLabel][hfLabel]);
                    cout << "    Calculating Eff: " << chLabel << " - " << hfLabel << " - " << flLabel << " - "
                         << (ef != CENTRAL ? (ef != MINUS ? "PLUS   " : "MINUS  " ) : "CENTRAL" ) << " ==> "
                         << efficiency[chLabel][hfLabel][flLabel][ef].first << " +- " << efficiency[chLabel][hfLabel][flLabel][ef].second
                         << " (flat eff = " << n_eff << ")" << endl;
                }
              // Calculate systematic efficiency.
                float central = efficiency[chLabel][hfLabel][flLabel][CENTRAL].first;
                float plus    = efficiency[chLabel][hfLabel][flLabel][PLUS   ].first;
                float minus   = efficiency[chLabel][hfLabel][flLabel][MINUS  ].first;
                float systerr = max(central-minus, plus-central);
                cout << "      Syst err for " << chLabel << "-" << hfLabel << "-" << "flLabel: " << systerr << endl;
                systErr[chLabel][hfLabel][flLabel] = systerr;

            }
          // Make Canvas
            TCanvas *plot = new TCanvas("effPlot", "effPlot");
            //plot->cd();
            TH1F* h_beff = (TH1F*) h_eff[chLabel][hfLabel]["Zb"][CENTRAL]->Clone("h_beff");
            TH1F* h_ceff = (TH1F*) h_eff[chLabel][hfLabel]["Zc"][CENTRAL]->Clone("h_ceff");
            TH1F* h_leff = (TH1F*) h_eff[chLabel][hfLabel]["Zl"][CENTRAL]->Clone("h_leff");

          // Reset titles so that they are printed properly
            h_beff->SetTitle("Eff. vs. Lead Tagged Jet p_{T};Jet p_{T} (GeV);Efficiency");

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
            bLabel << "Z+b (avg: " << setprecision(3) <<  efficiency[chLabel][hfLabel]["Zb"][CENTRAL].first << "#pm" << efficiency[chLabel][hfLabel]["Zb"][CENTRAL].second << ")";
            cLabel << "Z+c (avg: " << setprecision(3) <<  efficiency[chLabel][hfLabel]["Zc"][CENTRAL].first << "#pm" << efficiency[chLabel][hfLabel]["Zc"][CENTRAL].second << ")";
            lLabel << "Z+l (avg: " << setprecision(3) <<  efficiency[chLabel][hfLabel]["Zl"][CENTRAL].first << "#pm" << efficiency[chLabel][hfLabel]["Zl"][CENTRAL].second << ")";

          // The legend continues
            TLegend *leg = new TLegend(0.5,0.15,0.95,0.35);
            if(hfLabel=="SVT") leg->SetHeader(hfLabel+    " Selection Eff.");
            else               leg->SetHeader(hfLabel+"+SVT Selection Eff.");
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

  //  map<TString, TH1F*> h_num;
  //  map<TString, TH1F*> h_dWt;


    cout << "  EffPlotAndCalc.cpp ---- COMPLETE \n" << endl;

    return 0;
}


TH1F* getDataWeightPlot(TFile* f, TString& channel, TString& hftag)
{  // Extract DataWeights (raw_eff_plots/<CHANNEL>/<CHANNEL>/n_tagged_<HFTag>)
    TH1F* h_dtwt;
    TString path = "raw_eff_plots/";

    vector<TString> muDataSets = {"muon_c", "muon_d_1of6", "muon_d_2of6", "muon_d_3of6", "muon_d_4of6", "muon_d_5of6", "muon_d_6of6" };
    vector<TString> elDataSets = {"elec_c", "elec_d_1of8", "elec_d_2of8", "elec_d_3of8", "elec_d_4of8", "elec_d_5of8", "elec_d_6of8", "elec_d_7of8", "elec_d_8of8"};

    if(channel == "Zuu")
    {
        h_dtwt = (TH1F*) f->Get(path+muDataSets[0]+"/Zuu/ntEvt_"+hftag)->Clone("h_dtwt");
        for( int i=1; i<muDataSets.size(); i++ )
        {
            TH1F* h_temp = (TH1F*) f->Get(path+muDataSets[i]+"/Zuu/ntEvt_"+hftag)->Clone("h_temp");
            h_dtwt->Add(h_temp);
            delete h_temp;
        }
    }
    if(channel == "Zee")// h_dtwt = (TH1F*) f->Get(path+"elec/Zee/ntEvt_"+hftag)->Clone("h_dtwt");
    {
        h_dtwt = (TH1F*) f->Get(path+elDataSets[0]+"/Zee/ntEvt_"+hftag)->Clone("h_dtwt");
        for( int i=1; i<elDataSets.size(); i++ )
        {
            TH1F* h_temp = (TH1F*) f->Get(path+elDataSets[i]+"/Zee/ntEvt_"+hftag)->Clone("h_temp");
            h_dtwt->Add(h_temp);
            delete h_temp;
        }
    }
    if(channel == "Zll")
    {
        h_dtwt = (TH1F*) f->Get(path+muDataSets[0]+"/Zuu/ntEvt_"+hftag)->Clone("h_dtwt");
        for( int i=1; i<muDataSets.size(); i++ )
        {
            TH1F* h_temp = (TH1F*) f->Get(path+muDataSets[i]+"/Zuu/ntEvt_"+hftag)->Clone("h_temp");
            h_dtwt->Add(h_temp);
            delete h_temp;
        }
        for( int i=0; i<elDataSets.size(); i++ )
        {
            TH1F* h_temp = (TH1F*) f->Get(path+elDataSets[i]+"/Zee/ntEvt_"+hftag)->Clone("h_temp");
            h_dtwt->Add(h_temp);
            delete h_temp;
        }
    }
    cout << "   getDataWeightPlot: Extracted histo: " << h_dtwt->GetTitle() << endl;
    return h_dtwt;
}

TH1F* getDenominatorPlot(TFile* f, TString& channel, TString& flavor)
{ // Extract Den (raw_eff_plots/dy/Zee_Zf/n_ZfEvents)
  // channel = Zee,Zuu,Zll   flavor = Zb, Zc, Zl
    cout << "      getDenominatorPlot(" << f->GetName() << ", " << channel << ", " << flavor << ")" << endl;
    TH1F *h_den, *h_temp;
    TString hName = TString("h_")+flavor+"Den";
    TString path   = "raw_eff_plots/dy/";
    TString path1j = "raw_eff_plots/dy1j_";
    if(channel == "Zuu" || channel == "Zee")
    {
        cout << path+channel+"_"+flavor+"/n_Z"+flavor[1]+"Events" << endl;
        h_den  = (TH1F*) f->Get(path+channel+"_"+flavor+"/n_Z"+flavor[1]+"Events")->Clone(hName);
        for( char i='1', n='7'; i<=n; i++)
        {
            h_temp = (TH1F*) f->Get(path1j+i+"of"+n+'/'+channel+"_"+flavor+"/n_Z"+flavor[1]+"Events")->Clone("h_temp");
            h_den->Add(h_temp);
            delete h_temp;
        }
    }
    if(channel == "Zll")
    {   h_den  = (TH1F*) f->Get(path+"Zuu_"+flavor+"/n_Z"+flavor[1]+"Events")->Clone(hName);
        h_temp = (TH1F*) f->Get(path+"Zee_"+flavor+"/n_Z"+flavor[1]+"Events")->Clone("h_temp");
        h_den->Add(h_temp);
        delete h_temp;
        for( char i='1', n='7'; i<=n; i++)
        {
            h_temp = (TH1F*) f->Get(path1j+i+"of"+n+'/'+"Zuu_"+flavor+"/n_Z"+flavor[1]+"Events")->Clone("h_temp");
            h_den->Add(h_temp);
            delete h_temp;
            h_temp = (TH1F*) f->Get(path1j+i+"of"+n+'/'+"Zee_"+flavor+"/n_Z"+flavor[1]+"Events")->Clone("h_temp");
            h_den->Add(h_temp);
            delete h_temp;
        }
    }
    return h_den;
}

TH1F* getNumeratorPlot(TFile* f, TString& channel, TString& hftag, TString& flavor, errorFactor errFactor)
{ // Extract Num (raw_eff_plots/dy/Zee_Zf/n_tagged_ZfEvents_CSV#)
    TH1F *h_num, *h_temp;
    TString hName   = TString("h_")+flavor[1]+"Num";
    hName += (errFactor!=CENTRAL ? ( errFactor!=PLUS ? "_minus" : "_plus") : "" );
    TString path   = "raw_eff_plots/dy/";
    TString path1j = "raw_eff_plots/dy1j_";
    TString errSuffix = (errFactor!=CENTRAL ? ( errFactor!=PLUS ? "_mErr" : "_pErr") : "" );
    if(channel == "Zuu" || channel == "Zee")
    {
        h_num  = (TH1F*) f->Get(path  +channel+"_"+flavor+"/nt_Z"+flavor[1]+"Events_"+hftag+errSuffix)->Clone(hName);
        for( char i='1', n='7'; i<=n; i++)
        {
            h_temp = (TH1F*) f->Get(path1j+i+"of"+n+'/'+channel+"_"+flavor+"/nt_Z"+flavor[1]+"Events_"+hftag+errSuffix)->Clone("h_temp");
            h_num->Add(h_temp);
            delete h_temp;
        }
    }
    if(channel == "Zll")
    {   h_num  = (TH1F*) f->Get(path  +"Zuu_"+flavor+"/nt_Z"+flavor[1]+"Events_"+hftag+errSuffix)->Clone(hName);
        h_temp = (TH1F*) f->Get(path  +"Zee_"+flavor+"/nt_Z"+flavor[1]+"Events_"+hftag+errSuffix)->Clone("h_temp");
        h_num->Add(h_temp);
        delete h_temp;
        for( char i='1', n='7'; i<=n; i++)
        {
            h_temp = (TH1F*) f->Get(path1j+i+"of"+n+"/Zuu_"+flavor+"/nt_Z"+flavor[1]+"Events_"+hftag+errSuffix)->Clone("h_temp");
            h_num->Add(h_temp);
            delete h_temp;
            h_temp = (TH1F*) f->Get(path1j+i+"of"+n+"/Zee_"+flavor+"/nt_Z"+flavor[1]+"Events_"+hftag+errSuffix)->Clone("h_temp");
            h_num->Add(h_temp);
            delete h_temp;
        }
    }
    //cout << "   getDataWeightPlot: Extracted histo: " << h_dtwt->GetTitle() << endl;

    return h_num;
}

TH1F* makeEfficiencyPlot( TH1F* h_num, TH1F* h_den, TString &flavor)
{
  //  TString hName = TString("h_")+flavor[1]+"EventEff";
    TString hName = TString::Format("h_%cEventEff", flavor[1]);
    TH1F* h_eff = (TH1F*) h_num->Clone(hName);
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
