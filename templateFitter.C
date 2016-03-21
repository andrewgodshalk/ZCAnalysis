// templateFitter()
// Fits templates to data, extracts the fraction of b/c/l from fit, and shows a plot.

#include <string>
#include <sstream>


// Spits out date and time, by default in YYYY-MM-DD HH:MM:SS
string timeStamp(char d = '-', char b = ' ', char t = ':');
// Spits out date and time, by default in YYYY-MM-DD_HH_MM_SS, for use in filenames
string fileTimeStamp();

string timeStamp( char d, char b, char t ) {
  // print_date_time - takes characters to separate date & time.
  // if b and/or t are equal to '0', only prints date.
    stringstream output;
    TDatime now;
    int date = now.GetDate();
    int time = now.GetTime();
    if( time < 0 )
    {
        time-= 240000;
        date+= 1;
    }
  
    output << date/10000 << d << (date%10000)/1000 << (date%1000)/100 << d << (date%100)/10 << date%10;
    if( b!='0' && t!='0' ) output << b << time/10000 << t << (time%10000)/1000 << (time%1000)/100 << t << (time%100)/10 << time%10;
return output.str();
}


// Spits out date and time, by default in YYYY-MM-DD_HHMMSS, for use in filenames
string fileTimeStamp() {
  stringstream output;
  TDatime now;
  int date = now.GetDate();
  int time = now.GetTime();
  if( time < 0 )
  {
    time-= 240000;
    date+= 1;
  }

  output << date/100000 << (date%100000)/10000 << '-' << (date%10000)/1000 << (date%1000)/100 << '-' << (date%100)/10 << date%10;
  output << '_' << time/100000 << (time%100000)/10000 << (time%10000)/1000 << (time%1000)/100 << (time%100)/10 << time%10;
return output.str();
}


void inflateError(TH1F* h)
{
    float fudgeFactor = 1.;
    for(int i=1; i<=h->GetNbinsX(); i++) h->SetBinError(i, fudgeFactor*h->GetBinError(i));
}



string templateFitter(TString plotName, TH1F* h_sample, TH1F* h_b, TH1F* h_c, TH1F* h_l, int rebin = 1)
{
    std::stringstream log("");
    log << "\n  ==TEMPLATEFITTER== \n"
            "    " << plotName   << "\n"
//            "    " << timeStamp() << "\n"
            "    Rebin: " << rebin << "\n";


  // Set up the environment
    gStyle->SetOptStat("");     // Gets rid of the stats panel
    //gStyle->SetErrorX(0);       // Removes error bars in the x-direction.

    TH1F* data  = (TH1F*) h_sample->Clone("data" );
    TH1F* btemp = (TH1F*) h_b     ->Clone("btemp");
    TH1F* ctemp = (TH1F*) h_c     ->Clone("ctemp");
    TH1F* ltemp = (TH1F*) h_l     ->Clone("ltemp");

  // Extract integral from each template, then normalize.
    float nData = data ->Integral();  //data ->Scale(1.0/nData);
    float nBjet = btemp->Integral();  //btemp->Scale(1.0/nBjet);
    float nCjet = ctemp->Integral();  //ctemp->Scale(1.0/nCjet);
    float nLjet = ltemp->Integral();  //ltemp->Scale(1.0/nLjet);

  // Reset titles so that they are printed properly
    data->SetTitle("M_{SV};M_{SV};");

  // Rebin the histograms
    data ->Rebin(rebin);
    btemp->Rebin(rebin);  //inflateError(btemp);
    ctemp->Rebin(rebin);  //inflateError(ctemp);
    ltemp->Rebin(rebin);  //inflateError(ltemp);

  // Pick a color, any color.
    data->SetMarkerColor(kBlack);  data->SetMarkerStyle(20);  data->SetMarkerSize(1);

    data ->SetLineColor(kBlack  );  data ->SetLineWidth(2.0);
    btemp->SetLineColor(kRed    );  btemp->SetLineWidth(2.0);
    ctemp->SetLineColor(kBlue   );  ctemp->SetLineWidth(2.0);
    ltemp->SetLineColor(kGreen+2);  ltemp->SetLineWidth(2.0);

  // Draw individual templates in separate plots
    TCanvas *seperate_plots = new TCanvas(plotName+"_individual",plotName+"_individual");
    seperate_plots->Divide(2,2);
    seperate_plots->cd(1); data ->DrawNormalized("eb");     //gPad->SetLogy();
    seperate_plots->cd(2); btemp->DrawNormalized("hist e"); //gPad->SetLogy();
    seperate_plots->cd(3); ctemp->DrawNormalized("hist e"); //gPad->SetLogy();
    seperate_plots->cd(4); ltemp->DrawNormalized("hist e"); //gPad->SetLogy();
    seperate_plots->Update();

  // Set up slot for fitter variables
    double frac[3], err[3];  int status;
    TObjArray *temps = new TObjArray(3);  // Array for template histograms
    temps->Add(btemp); temps->Add(ctemp); temps->Add(ltemp);

  // Set up your Fraction Fitter
    TFractionFitter *fit = new TFractionFitter(data, temps);
    fit->Constrain(0, 0.00001, 0.99999);
    fit->Constrain(1, 0.00001, 0.99999);
    fit->Constrain(2, 0.00001, 0.99999);
    //if     (plotName == "fit_TEMPS_dy_py8_ON_dy_lpc_WITH_RATIOS_45-45-10")
    //{
    //    fit->Constrain(2, 0.04671697,  0.05328263);
    //    log << " BLERG!\n";
    //}
    //else if(plotName == "fit_TEMPS_dy_py8_ON_dy_lpc_WITH_RATIOS_60-30-10") fit->Constrain(2, 0.09631067,  0.10368293);
    //else if(plotName == "fit_TEMPS_dy_py8_ON_dy_lpc_WITH_RATIOS_70-20-10") fit->Constrain(2, 0.09607847,  0.10391653);
    //else if(plotName == "fit_TEMPS_dy_py8_ON_dy_lpc_WITH_RATIOS_85-10-05") fit->Constrain(2, 0.095511865, 0.104480735);
    //else fit->Constrain(2, 0.00001, 0.99999);

  // Extract the number of bins in data, then Set the range of the fit to exclude first bin.
    int nBins = data->GetNbinsX();
    fit->SetRangeX(1,nBins);

  // DO THE DEED!!
    status = fit->Fit();
// ?????????????????????????? //
    float _FUP = 0.54;        // set UP with 70% C.L. for 2 parameters  
    fit->ErrorAnalysis(_FUP); //

// ?????????????????????????? //
    cout << "fit status:" << status << endl;

  // Start delivering the goods.
  // If the fit returns a non-zero status, fudge the numbers and kill the program.
    if (status != 0) {
        frac[0] = -1.0;  err[0] = 0.0;
        frac[1] =  0.0;  err[1] = 0.0;
        frac[2] =  0.0;  err[2] = 0.0;
        log << "  Error: " << plotName
            << " has encountered an error in fitting. Status: "
            << Form("%d", status) << "\n\n";
        delete fit;
        delete temps;
        return log.str();
    }

  // Extract fit results
    for(int i=0; i<3; i++) fit->GetResult(i, frac[i], err[i]);
    TH1F* result = (TH1F*) fit->GetPlot();

  // Draw something pretty
    TString resultPlotName = plotName + "_result";
    TCanvas *resultPlot = new TCanvas(resultPlotName, resultPlotName);
    result->SetLineColor(kMagenta);  result->SetLineStyle(2);
    result->SetLineWidth(3.0);       result->Draw("e");

  // Scale templates for plot
    btemp->Scale(frac[0]*nData/btemp->Integral());
    ctemp->Scale(frac[1]*nData/ctemp->Integral());
    ltemp->Scale(frac[2]*nData/ltemp->Integral());

  // Draw them histos.
    data ->Draw("hist PE sames");
    btemp->Draw("hist E sames");
    ctemp->Draw("hist E sames");
    ltemp->Draw("hist E sames");
    result->Draw("esame");

  // Set Log Y
    resultPlot->SetLogy();

  // Make labels for histogram legend
    float chi2val = fit->GetChisquare()/fit->GetNDF();
    TString dataLegLabel = TString("Data (") + Form("%.1f",nData) + ")";
    TString bjetLegLabel = TString("Z+b (") + Form("%.3f",frac[0]) + "#pm" + Form("%.3f",err[0]) + ")";
    TString cjetLegLabel = TString("Z+c (") + Form("%.3f",frac[1]) + "#pm" + Form("%.3f",err[1]) + ")";
    TString ljetLegLabel = TString("Z+dusg (") + Form("%.3f",frac[2]) + "#pm" + Form("%.3f",err[2]) + ")";
    TString  fitLegLabel = TString("Fit (#chi^2/NDOF = ") + Form("%.3f", chi2val) + ")";

  // The legend continues
    TLegend *leg = new TLegend(0.63,0.62,0.98,0.88);
    leg->SetHeader("M_{SV} Template Fit");
    leg->AddEntry(data,   dataLegLabel, "P");
    leg->AddEntry(btemp,  bjetLegLabel, "L");
    leg->AddEntry(ctemp,  cjetLegLabel, "L");
    leg->AddEntry(ltemp,  ljetLegLabel, "L");
    leg->AddEntry(result,  fitLegLabel, "L");
    //leg->SetBorderSize(0);  leg->SetFillStyle(0);
    leg->Draw();

  // Same thing, but with your scaled templates stacked.
    THStack *stackedTemps = new THStack("temp_stack", "temp_stack");
    TCanvas *resultPlotStacked = new TCanvas(resultPlotName+"Stacked", resultPlotName+"Stacked");
    result->Draw("e");

  // Create new, filled versions of the templates for the stack
    TH1F *btempFilled = (TH1F*) btemp->Clone("btemp_filled"); btempFilled->SetFillColor(46);  stackedTemps->Add(btempFilled); //btempFilled->SetLineWidth(0);
    TH1F *ctempFilled = (TH1F*) ctemp->Clone("ctemp_filled"); ctempFilled->SetFillColor(38);  stackedTemps->Add(ctempFilled); //ctempFilled->SetLineWidth(0);
    TH1F *ltempFilled = (TH1F*) ltemp->Clone("ltemp_filled"); ltempFilled->SetFillColor(30);  stackedTemps->Add(ltempFilled); //ltempFilled->SetLineWidth(0);
    stackedTemps->Draw("hist E sames");
    data->Draw("hist E sames");  result->Draw("esame");  leg->Draw();
  // Give plot a log axis
//    resultPlotStacked->SetLogy();

////////////////////// CONTROL PLOT
  // Same thing, but with false-scaled templates
    THStack *stackedTemps_control = new THStack("temp_stack_control", "temp_stack_control");
    TCanvas *resultPlotStacked_control = new TCanvas(resultPlotName+"Stacked_control", resultPlotName+"Stacked_control");
    result->Draw("e");

  // Set scale factors
    float sB, sC, sL;
    if     (plotName.Contains("85-10-05")) { sB = 0.85; sC = 0.10; sL = 0.05; }
    else if(plotName.Contains("70-20-10")) { sB = 0.70; sC = 0.20; sL = 0.10; }
    else if(plotName.Contains("60-30-10")) { sB = 0.60; sC = 0.30; sL = 0.10; }
    else if(plotName.Contains("45-45-10")) { sB = 0.45; sC = 0.45; sL = 0.10; }
    else  { sB = 0; sC = 0; sL = 0; }

  // Create new, filled versions of the templates for the stack
    TH1F *btempFilled_control = (TH1F*) btemp->Clone("btemp_filled_control"); btempFilled_control->Scale(nData*sB/btempFilled_control->Integral()); btempFilled_control->SetFillColor(46);  stackedTemps_control->Add(btempFilled_control); //btempFilled->SetLineWidth(0);
    TH1F *ctempFilled_control = (TH1F*) ctemp->Clone("ctemp_filled_control"); ctempFilled_control->Scale(nData*sC/ctempFilled_control->Integral()); ctempFilled_control->SetFillColor(38);  stackedTemps_control->Add(ctempFilled_control); //ctempFilled->SetLineWidth(0);
    TH1F *ltempFilled_control = (TH1F*) ltemp->Clone("ltemp_filled_control"); ltempFilled_control->Scale(nData*sL/ltempFilled_control->Integral()); ltempFilled_control->SetFillColor(30);  stackedTemps_control->Add(ltempFilled_control); //ltempFilled->SetLineWidth(0);
    stackedTemps_control->Draw("hist E sames");
    data->Draw("hist E sames");  result->Draw("esame");  leg->Draw();


  // Output the fractions from each template
    log << "    Fractions: \n"
           "      Z+b: " << frac[0] << "\t +- " << err[0] << "\n"
           "      Z+c: " << frac[1] << "\t +- " << err[1] << "\n"
           "      Z+l: " << frac[2] << "\t +- " << err[2] << "\n"
           "      (Check: Total = " << frac[0]+frac[1]+frac[2] << ")\n"
           "      Chi2/NDOF = " << chi2val << "\n"
           "\n";
/////////////////////////////////////////////

  // Output plots to file
    TFile plotOutput(TString("fits/") + plotName + ".root", "RECREATE");
    seperate_plots   ->Write();
    resultPlot       ->Write();
    resultPlotStacked->Write();
    resultPlotStacked_control->Write();
    plotOutput.Close();

    delete fit;
    delete temps;
    return log.str();
}

TH1F* getSampleFromFile(TString fn)
{
    TFile* inputFile = TFile::Open(fn);

  // Pointers to file histograms
    TH1F *h_sample, *hf_sample;  // Pointer to new histogram and histogram in file.

  // Get templates from file
    inputFile->GetObject("h_sample", hf_sample);
    cout << hf_sample->GetTitle() << endl;

  // Add templates together to get final templates
    gROOT->cd();
    h_sample = (TH1F*) hf_sample->Clone("data");
    cout << h_sample->GetTitle() << endl;

  // Clean up
    inputFile->Close();
    cout << h_sample->GetTitle() << endl;
    return h_sample;
}

void getTemplatesFromFile(TString fn, TH1F*& h_b, TH1F*& h_c, TH1F*& h_l)
{
    TFile* inputFile = TFile::Open(fn);

  // Pointers to file histograms
    TH1F *hf_b, *hf_c, *hf_l;

  // Get templates from file
    inputFile->GetObject("h_b", hf_b);
    inputFile->GetObject("h_c", hf_c);
    inputFile->GetObject("h_l", hf_l);

  // Add templates together to get final templates
    h_b = (TH1F*) hf_b->Clone("btemp");
    h_c = (TH1F*) hf_c->Clone("ctemp");
    h_l = (TH1F*) hf_l->Clone("ltemp");

  // Clean up
    delete hf_b;  delete hf_c;  delete hf_l;

}


void getTemplatesFromOldFile(TH1F*& h_b, TH1F*& h_c, TH1F*& h_l)
{
    TFile* inputFile = TFile::Open("/home/godshalk/Work/2016-03-15_ZC_closureTestRedux/svt_csvt_lnl.root");

  // Pointers to file histograms
    TH1F *hf_b, *hf_c, *hf_l;

  // Get templates from file
    inputFile->GetObject("bjets", hf_b);
    inputFile->GetObject("cjets", hf_c);
    inputFile->GetObject("ljets", hf_l);

  // Add templates together to get final templates
    h_b = (TH1F*) hf_b->Clone("btemp");
    h_c = (TH1F*) hf_c->Clone("ctemp");
    h_l = (TH1F*) hf_l->Clone("ltemp");

  // Clean up
    delete hf_b;  delete hf_c;  delete hf_l;

}


TH1F* getSampleFromOldFile()
{
    TFile* inputFile = TFile::Open("/home/godshalk/Work/2016-03-15_ZC_closureTestRedux/svt_csvt_lnl_DYCombo-70-20-10.root");

  // Pointers to file histograms
    TH1F *h_sample, *hf_sample;  // Pointer to new histogram and histogram in file.

  // Get templates from file
    inputFile->GetObject("muon", hf_sample);
    cout << hf_sample->GetTitle() << endl;

  // Add templates together to get final templates
    gROOT->cd();
    h_sample = (TH1F*) hf_sample->Clone("data");
    cout << h_sample->GetTitle() << endl;

  // Clean up
    inputFile->Close();
    cout << h_sample->GetTitle() << endl;
    return h_sample;
}