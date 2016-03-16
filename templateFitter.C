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



string templateFitter(TString plotName, TFile* source, int rebin = 2)
{
  // Set up the environment
    gStyle->SetOptStat("");     // Gets rid of the stats panel
    //gStyle->SetErrorX(0);       // Removes error bars in the x-direction.

  // Extract histograms from file
    TH1F *data  = (TH1F*)source->Get("muon");
    TH1F *btemp = (TH1F*)source->Get("bjets");
    TH1F *ctemp = (TH1F*)source->Get("cjets");
    TH1F *ltemp = (TH1F*)source->Get("ljets");

  // Extract integral from each template, then normalize.
    float nData = data ->Integral();  //data ->Scale(1.0/nData);
    float nBjet = btemp->Integral();  //btemp->Scale(1.0/nBjet);
    float nCjet = ctemp->Integral();  //ctemp->Scale(1.0/nCjet);
    float nLjet = ltemp->Integral();  //ltemp->Scale(1.0/nLjet);

  // Reset titles so that they are printed properly
    data->SetTitle("M_{SV};M_{SV};");

  // Rebin the histograms
    data ->Rebin(rebin);
    btemp->Rebin(rebin);
    ctemp->Rebin(rebin);
    ltemp->Rebin(rebin);

  // Pick a color, any color.
    data->SetMarkerColor(kBlack);  data->SetMarkerStyle(20);  data->SetMarkerSize(1);

    data ->SetLineColor(kBlack  );  data ->SetLineWidth(2.0);
    btemp->SetLineColor(kRed    );  btemp->SetLineWidth(2.0);
    ctemp->SetLineColor(kBlue   );  ctemp->SetLineWidth(2.0);
    ltemp->SetLineColor(kGreen+2);  ltemp->SetLineWidth(2.0);

  // Draw individual templates in separate plots
    TCanvas *seperate_plots = new TCanvas(plotName+"_individual",plotName+"_individual");
    seperate_plots->Divide(2,2);
    seperate_plots->cd(1); gPad->SetLogy(); data ->DrawNormalized("eb");
    seperate_plots->cd(2); gPad->SetLogy(); btemp->DrawNormalized("hist e");
    seperate_plots->cd(3); gPad->SetLogy(); ctemp->DrawNormalized("hist e");
    seperate_plots->cd(4); gPad->SetLogy(); ltemp->DrawNormalized("hist e");
    seperate_plots->Update();

  // Set up slot for fitter variables
    double frac[3], err[3];  int status;
    TObjArray *temps = new TObjArray(3);  // Array for template histograms
    temps->Add(btemp); temps->Add(ctemp); temps->Add(ltemp);

  // Set up your Fraction Fitter
    TFractionFitter *fit = new TFractionFitter(data, temps);
    fit->Constrain(0, 0.00, 1.00);
    fit->Constrain(1, 0.00, 0.99);
    fit->Constrain(2, 0.00, 0.99);

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
    std::stringstream log("");
    log << "\n  ==TEMPLATEFITTER== \n"
            "    " << plotName   << "\n"
//            "    " << timeStamp() << "\n"
            "    Rebin: " << rebin << "\n";

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
    btemp->Scale(frac[0]*nData/nBjet);
    ctemp->Scale(frac[1]*nData/nCjet);
    ltemp->Scale(frac[2]*nData/nLjet);

  // Draw them histos.
    data ->Draw("hist PE sames");
    btemp->Draw("hist E sames");
    ctemp->Draw("hist E sames");
    ltemp->Draw("hist E sames");
    result->Draw("esame");

  // Set Log Y
    resultPlot->SetLogy();

  // Make labels for histogram legend
    TString dataLegLabel = TString("Data (") + Form("%.1f",nData) + ")";
    TString bjetLegLabel = TString("Z+b (") + Form("%.3f",frac[0]) + "#pm" + Form("%.3f",err[0]) + ")";
    TString cjetLegLabel = TString("Z+c (") + Form("%.3f",frac[1]) + "#pm" + Form("%.3f",err[1]) + ")";
    TString ljetLegLabel = TString("Z+dusg (") + Form("%.3f",frac[2]) + "#pm" + Form("%.3f",err[2]) + ")";

  // The legend continues
    TLegend *leg = new TLegend(0.63,0.62,0.98,0.88);
    leg->SetHeader("M_{SV} Template Fit");
    leg->AddEntry(data,   dataLegLabel, "P");
    leg->AddEntry(btemp,  bjetLegLabel, "L");
    leg->AddEntry(ctemp,  cjetLegLabel, "L");
    leg->AddEntry(ltemp,  ljetLegLabel, "L");
    leg->AddEntry(result, "Fit",        "L");
    //leg->SetBorderSize(0);  leg->SetFillStyle(0);
    leg->Draw();

  // Same thing, but with your scaled templates stacked.
    THStack *stackedTemps = new THStack("temp_stack", "temp_stack");
    TCanvas *resultPlotStacked = new TCanvas(resultPlotName+"Stacked", resultPlotName+"Stacked");
    result->Draw("e");

  // Create new, filled versions of the templates for the stack
    TH1F *btempFilled = (TH1F*) btemp->Clone("btemp_filled"); /*btempFilled->SetLineWidth(0);*/ btempFilled->SetFillColor(46);  stackedTemps->Add(btempFilled);
    TH1F *ctempFilled = (TH1F*) ctemp->Clone("ctemp_filled"); /*ctempFilled->SetLineWidth(0);*/ ctempFilled->SetFillColor(38);  stackedTemps->Add(ctempFilled);
    TH1F *ltempFilled = (TH1F*) ltemp->Clone("ltemp_filled"); /*ltempFilled->SetLineWidth(0);*/ ltempFilled->SetFillColor(30);  stackedTemps->Add(ltempFilled);
    stackedTemps->Draw("hist E sames");
    data->Draw("hist E sames");  result->Draw("esame");  leg->Draw();
  // Give plot a log axis
//    resultPlotStacked->SetLogy();

  // Output the fractions from each template
    log << "    Fractions: \n"
           "      Z+b: " << frac[0] << "\t +- " << err[0] << "\n"
           "      Z+c: " << frac[1] << "\t +- " << err[1] << "\n"
           "      Z+l: " << frac[2] << "\t +- " << err[2] << "\n"
           "      (Check: Total = " << frac[0]+frac[1]+frac[2] << ")\n"
           "\n";

  // Output plots to file
    TFile plotOutput(TString("fits/") + plotName + ".root", "RECREATE");
    seperate_plots   ->Write();
    resultPlot       ->Write();
    resultPlotStacked->Write();
    plotOutput.Close();

    delete fit;
    delete temps;
    return log.str();

}