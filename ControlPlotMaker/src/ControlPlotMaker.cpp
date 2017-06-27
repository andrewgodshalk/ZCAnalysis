/*------------------------------------------------------------------------------
ControlPlotMaker.cpp

 Created : 2015-11-23  godshalk
 Modified: 2017-03-04  godshalk

------------------------------------------------------------------------------*/

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <list>
#include <TCanvas.h>
#include <TFile.h>
#include <THStack.h>
#include <TImage.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TRegexp.h>
#include "../interface/ControlPlotMaker.h"

using std::atoi;   using std::setw  ;
using std::cout;   using std::string;
using std::endl;   using std::vector;
using std::map ;   using std::list  ;

ControlPlotMaker::ControlPlotMaker(TString fnac, TString fni, TString fno, TString o, bool log, bool flv, bool tau, bool eps)
 : anCfg(fnac), fnInput(fni), fnOutput(fno), options(o), usingLogScale(log), splitDYByFlavor(flv), splitTauDecayFromDY(tau), outputAsEPS(eps), usingLegStats(true), rebinOpt(2), integralInLegend(false)
{
  // TEST output
    cout << "  ControlPlotMaker::ControlPlotMaker(): Created.\n"
            "    Options:                       " << options   << "\n"
         << endl;

  // Get HF rebin option from command line option.
    if(options.Index(TRegexp("[rR][eE][bB][iI][nN][0-9]")) != -1)
        rebinOpt = atoi(options(5,1).Data());

  // Set up lists of datasets
  //   //bkgdDatasets = {"zz", "wz", "ww", "ttlep", "ttsemi", "tthad", "t_s", "t_t", "t_tw", "tbar_s", "tbar_t", "tbar_tw"};
    bkgdDatasets = {"zz", "wz", "ww", "tt"};
    //if(splitTauDecayFromDY) bkgdDatasets.push_back("dy_tautau");
    if(splitDYByFlavor) sigDatasets = {"dy_Zl", "dy_Zc", "dy_Zb"};
    else                sigDatasets = {"dy"};
    dataDatasets = {"muon", "elec"};
    allDatasets = {""};  // Leave first listing open for the data dataset. Then fill w/ signal and BG MC.
    allDatasets.insert(allDatasets.end(),  sigDatasets.begin(),  sigDatasets.end() );
    allDatasets.insert(allDatasets.end(), bkgdDatasets.begin(), bkgdDatasets.end() );

  // Set up color and style maps.
    vector<string> dsNameVec  = { "muon", "elec",        "dy",       "dy_tautau",       "dy_Zl",     "dy_Zc",    "dy_Zb",      "zz",     "wz",     "ww",       "tt",        "tt_lep",         "ttsemi",         "tthad",          "t_s",          "t_t",          "t_tw",             "tbar_s",             "tbar_t",             "tbar_tw" };
    Color_t dsColorVec[]      = { kBlack, kBlack,      kRed+1,           kYellow,        kRed+1,      kRed+1,     kRed+1,  kGreen-1, kGreen-2, kGreen-3,    kBlue-0,         kBlue-0,          kBlue-2,         kBlue-3,           kRed,           kRed,       kOrange+3,                 kRed,                 kRed,             kOrange+3 };
    Style_t dsStyleVec[]      = {      0,      0,        1001,              1001,          1001,        3001,       3002,      1001,     1001,     1001,       1001,            1001,             1001,            1001,           1001,           1001,            1001,                 1001,                 1001,                  3001 };
    string  dsLegLabel[]      = { "Data", "Data", "Drell-Yan", "DY (Z->#tau#tau", "DY (Z+udsg)",  "DY (Z+c)", "DY (Z+c)",      "ZZ",     "WZ",     "WW", "t#bar{t}", "t#bar{t}(lep)", "t#bar{t}(semi)", "t#bar{t}(had)", "t(s-channel)", "t(t-channel)", "t(tW-channel)", "#bar{t}(s-channel)", "#bar{t}(t-channel)", "#bar{t}(tW-channel)" };

    for(unsigned int i=0; i<dsNameVec.size(); i++)
    {   dsColor[dsNameVec[i]] = dsColorVec[i];
        dsStyle[dsNameVec[i]] = dsStyleVec[i];
    }

  // Set up input and outut files, taking into account that the two could be the same file.
    map<TString, TFile*> fileList;
    fileList[fnInput] = fileList[fnOutput] = NULL;
    // for(auto& kv : fileList)
    // {   cout << "  ControlPlotMaker::ControlPlotMaker(): Opening file: " << kv.first << endl;
    //     kv.second = TFile::Open(kv.first, "UPDATE");
    // }

    inputFile  = fileList[fnInput ] = TFile::Open(fnInput, "UPDATE");
    if(fnInput == fnOutput)
        outputFile = fileList[fnOutput] = inputFile;
    else
        outputFile = fileList[fnOutput] = TFile::Open(fnOutput, "RECREATE");

  // For the Zuu, then Zee plots...
    //vector<string> decayChain = {"Zuu"};
    vector<string> decayChain = {"Zuu", "Zee"};
    for(int ds_i=0; ds_i<2; ds_i++)
    {   cout << "  ControlPlotMaker::ControlPlotMaker(): Processing decay chain: " << decayChain[ds_i] << endl;
      // Set up output directory in output file
        TString newOutDir = TString("control_plots/combined_plots/")+decayChain[ds_i];
        outputFile->cd();
        outputFile->mkdir(newOutDir);
        TDirectory* outDir = outputFile->GetDirectory(newOutDir);
        outDir->cd();

      // Set the proper data dataset for this decay chain.
        allDatasets[0] = dataDatasets[ds_i];

      // Set the directory location of each dataset
        for(string& ds : allDatasets) setDatasetDir(ds, decayChain[ds_i]);

      // Get list of selection sub directories (from data directory, arbitrarily. Should have same in each directory).
        TList* selectionSubDirList = dsDirectory[allDatasets[0]]->GetListOfKeys();
        // selectionSubDirList->Print();

      // Iterate through sub directories.
        TIter nextSubDir(selectionSubDirList);
        TDirectory* templateSubDir  = 0;
        TDirectory* selectionSubDir = 0;
        TString selectionDirName = "";
        while( (templateSubDir = (TDirectory*) nextSubDir()) )
        { // Set up the output for saving the stacked histograms.
            selectionDirName = templateSubDir->GetName();
            selectionSubDir  = outDir->mkdir(selectionDirName);
            selectionSubDir->cd();
            cout << "  ControlPlotMaker::ControlPlotMaker(): Processing selection directory: " << templateSubDir->GetName()
                 << " (" << templateSubDir->ClassName() << ")"<< endl;
            templateSubDir = dsDirectory[allDatasets[0]]->GetDirectory(selectionDirName);

          // Get list of histgrams (from data directory, arbitrarily. Should have same in each directory).
            // TList* histos = dsDirectory[allDatasets[0]]->GetListOfKeys();
            TList* histos = templateSubDir->GetListOfKeys();
            // cout << " Histos retrieved." << endl;

          // Iterate through histograms.
            TIter nextHist(histos); TH1 *hist = 0;
            while((hist=(TH1*)nextHist()))
            {
                TString histToStack = hist->GetName();
                // if(histToStack!="zhfmet_CSVT_hfjet_ld_msv") continue;

                cout << "    ControlPlotMaker::ControlPlotMaker(): Processing histo: " << hist->GetName() << endl;
              // Iterate through each dataset and:
                for(string& ds : allDatasets) dsHist[ds] = createStackHisto(ds, selectionDirName, histToStack);

              // Order the datasets by dominant contribution
                map<string, float> dsIntegral;
                list<string> dsOrder;
                for(auto& ds_h : dsHist) dsIntegral[ds_h.first] = ds_h.second->Integral();
              //  for(auto& ds_n : dsIntegral) cout << "    ControlPlotMaker::ControlPlotMaker(): " << ds_n.first << " has integral of " << ds_n.second << endl;
              //  for(auto& ds : allDatasets)  cout << "    ControlPlotMaker::ControlPlotMaker(): " << setw(12) << ds << " has integral of " << dsIntegral[ds] << " (" << dsHist[ds]->Integral() << "," << dsHist[ds]->GetEntries() << ")" << endl;
              //  for(auto& ds : allDatasets)  cout << "  " << setw(12) << ds << setw(10) << dsIntegral[ds] << setw(10) << dsHist[ds]->Integral() << setw(10) << dsHist[ds]->GetEntries() << endl;
                for(auto& ds_n : dsIntegral)
                {
                    if(ds_n.first=="muon" || ds_n.first=="elec" || ds_n.second < 10) continue;
                    list<string>::iterator it = dsOrder.begin();
                    while(it != dsOrder.end() && ds_n.second > dsIntegral[*it]) it++;
                    dsOrder.insert(it, string(ds_n.first));
                }
                //cout << "    ControlPlotMaker::ControlPlotMaker(): Dataset order: ";
                //for(auto& ds : dsOrder) cout << ds << "(" << dsIntegral[ds] << "), ";
                //cout << endl;

              // Create histo for ratio plot denominator.
                TH1F* mcHist = new TH1F("totalSim", dsHist["tt"]->GetTitle(), dsHist["tt"]->GetNbinsX(), dsHist["tt"]->GetXaxis()->GetXmin(), dsHist["tt"]->GetXaxis()->GetXmax());
                mcHist->Sumw2();
              //  TH1F *mcHist = (TH1F*) dsHist["tt"]->Clone("totalSim");
              //  for(int i=0; i<mcHist->GetNbinsX(); i++)
              //  {   mcHist->SetBinContent(0,0);
              //      mcHist->SetBinError(0,0);
              //  }

              // Create a stack for signal and background histograms based on population.
                TString stackName = TString("h_stack_")+histToStack;
                THStack *simStack = new THStack(stackName, stackName);
                for(auto& ds: dsOrder)
                {   simStack->Add(dsHist[ds]);
                    mcHist  ->Add(dsHist[ds]);
                }

              // Create canvases
                TString cName  = decayChain[ds_i]+"_"+histToStack;
                TString cTitle = dsHist[allDatasets[0]]->GetTitle();
                TCanvas *plot  = new TCanvas(cName,cTitle, 600, 600);
                plot->SetHighLightColor(2);
                plot->Range(0,0,1,1);
                plot->SetFillColor(0);
                plot->SetFillStyle(4000);
                plot->SetBorderMode(0);
                plot->SetBorderSize(2);
                plot->SetFrameFillStyle(1000);
                plot->SetFrameBorderMode(0);

                plot->cd();
                TPad * stackPad = new TPad("stackPad", "stackPad", 0, 0.3, 1, 1);
                stackPad->Draw();
                stackPad->cd();
                stackPad->Range(3.951682,-3,208.9113,-0.4156831);
                stackPad->SetFillColor(0);
                stackPad->SetBorderMode(0);
                stackPad->SetBorderSize(2);
                stackPad->SetLogy();
                stackPad->SetLeftMargin(0.12709);
                stackPad->SetRightMargin(0.0434783);
                stackPad->SetBottomMargin(0.005);
                stackPad->SetFrameBorderMode(0);
                stackPad->SetFrameBorderMode(0);

                plot->cd();
                TPad * ratioPad = new TPad("ratioPad", "ratioPad", 0, 0, 1, 0.3);
                ratioPad->Draw();
                ratioPad->cd();
                ratioPad->Range(3.951682,-0.2885246,208.9113,1.678689);
                ratioPad->SetFillColor(0);
                ratioPad->SetBorderMode(0);
                ratioPad->SetBorderSize(2);
                ratioPad->SetTickx(1);
                ratioPad->SetTicky(1);
                ratioPad->SetLeftMargin(0.12709);
                ratioPad->SetRightMargin(0.0434783);
                ratioPad->SetTopMargin(0.04);
                ratioPad->SetBottomMargin(0.35);
                ratioPad->SetFrameBorderMode(0);
                ratioPad->SetFrameBorderMode(0);

              // Plot the data and stack on the same histogram.
                stackPad->cd();
                if(usingLogScale) stackPad->SetLogy();   // If log variable asked for, set log axis.
                TH1* dataHist = dsHist[allDatasets[0]];
                // Modify y-range based on type of plot. Set max to 110% max value of either data or mc-stack.
                float simMax  = simStack->GetMaximum();
                float dataMax = dataHist->GetMaximum();
                float plotMax = dataMax>simMax ? dataMax : simMax;
                float plotMin = usingLogScale ? 1 : 0;
                plotMax      *= usingLogScale ? 5 : 1.1;
                //dataHisto->SetAxisRange(plotMin, plotMax, "Y");    // Doesn't work for log plots.
                dataHist->SetMinimum(plotMin);
                dataHist->SetMaximum(plotMax);
                dataHist->SetStats(false);
                //dataHist->SetMarkerStyle(8);
                //dataHist->SetMarkerSize(0.8);
                dataHist->Draw();
                simStack->Draw("hist same");
                dataHist->DrawClone("EP same");
                dataHist->Draw("axis same");

              // Set up legend
                TLegend *leg = NULL;
                if(integralInLegend) leg = new TLegend(0.65,0.60,0.90,0.89);
                else                 leg = new TLegend(0.72,0.50,0.98,0.89);
                if(decayChain[ds_i] == "Zuu") leg->SetHeader("Z\\rightarrow\\mu\\mu Channel");
                if(decayChain[ds_i] == "Zee") leg->SetHeader("Z\\rightarrow ee Channel");

              // Data histogram listing
                TString legLabel = "";
                if(integralInLegend) legLabel = Form("%s (%.1f)", dsLegLabel[0].c_str(), dataHist->Integral());
                else                 legLabel = Form("%s",        dsLegLabel[0].c_str()                      );
                leg->AddEntry(dataHist, legLabel, "p");
              // Bkgd histo listings
                for(list<string>::reverse_iterator ds = dsOrder.rbegin(); ds!=dsOrder.rend(); ds++)
                {   TH1* hist = dsHist[*ds];
                    unsigned int k; for(k=0; k<dsNameVec.size(); k++) if(dsNameVec[k]==*ds) break;
                    if(k==dsNameVec.size()) cout << " ERROR: DS NAME NOT FOUND IN LIST: " << *ds << endl;
                    // else cout << " Legend label found: " << *ds << " found at " << k << " (" << dsLegLabel[k] << ")" << endl;
                    if(integralInLegend) legLabel = Form("%s (%.1f)", dsLegLabel[k].c_str(), hist->Integral());
                    else                 legLabel = Form("%s",        dsLegLabel[k].c_str()                  );
                    leg->AddEntry(hist, legLabel, "f");
                }

                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->Draw();

                stackPad->cd();
                TLatex* tex = new TLatex(0.45,0.92,"CMS Internal #sqrt{s} = 13 TeV, 35.9 fb^{-1}");
                tex->SetNDC();
                tex->SetTextFont(42);
                tex->SetLineWidth(2);
                tex->Draw();

              // Plot the ratio plot.
                ratioPad->cd();
                ratioPad->SetLogy(0);   // Set ratio pad to linear scale
                ratioPad->SetGrid();
                // ratioPad->SetTopMargin(0);
                //Make MC denominator plot
                //TH1F* mcHist    = createSimSumPlot(simStack);

                TH1F* ratioHist = createRatioPlot((TH1F*) dataHist, mcHist);
                ratioHist->DrawCopy("ep");

              // Save canvas to file.
                selectionSubDir->cd();
                // outDir->cd();
                plot->Update();
                plot->Write(histToStack, TObject::kOverwrite);

              // If necessary, save canvas to EPS
                if(eps)
                {   TImage *img = TImage::Create();
                    img->FromPad(plot);
                    img->WriteImage(TString("ControlPlotMaker/plots/")+histToStack+".png");
                    plot->SaveAs(TString("ControlPlotMaker/epsplots/")+histToStack+".eps");
                    delete img;
                }

              // Clean up
                delete plot;
                for(string& ds : allDatasets) dsHist[ds]->Delete();
                dsHist.clear();
                delete simStack;
                delete mcHist;
                delete ratioHist;

                //////////////////UNCOMMENT HERE TO TEST ONLY ONE PLOT
                // break;
            }
        }
    }

  // Close all files.
    for(auto& kv : fileList)
    {   cout << "  ControlPlotMaker::ControlPlotMaker(): Closing file: " << kv.first << endl;
        kv.second->Close();
    }
}


void ControlPlotMaker::setDatasetDir(string ds, string decayChain)
{
    cout << "    ControlPlotMaker::setDatasetDir(): Setting location of dataset " << ds << " for decay chain " << decayChain << endl;

  // Set the location of the dataset's directory for this decay chain
    // Special cases: "dy_tautau" "dy_Zl" "dy_Zc" "dy_Zb"
    TString dirStr = TString("control_plots/") + ds + "/" + decayChain;
    if(ds=="dy_tautau") dirStr = TString("control_plots/dy/")+decayChain+"_Ztautau";
    if(ds=="dy_Zl"    ) dirStr = TString("control_plots/dy/")+decayChain+"_Zl";
    if(ds=="dy_Zc"    ) dirStr = TString("control_plots/dy/")+decayChain+"_Zc";
    if(ds=="dy_Zb"    ) dirStr = TString("control_plots/dy/")+decayChain+"_Zb";

    // cout << "    - Retrieving directory: " <<  dirStr << endl;
    dsDirectory[ds] = inputFile->GetDirectory(dirStr);
    // cout << "    - Retrieved directory: " <<  dsDirectory[ds]->GetTitle() << endl;
}


TH1* ControlPlotMaker::createStackHisto(string& ds, TString& selectionName, TString& histName)
{ // Sets up a histogram from file given the proper dataset and histogram name.
  // Create a clone to the appropriate histogram,
    // cout << "      Attempting to retrieve from " << ds << ": " << histName << "..." << endl;
    TH1* h = (TH1*)(dsDirectory[ds]->GetDirectory(selectionName)->Get(histName)->Clone(ds+"_"+histName));
    h->SetTitle("");
    // h->SetXTitle("");
    // h->SetLabel()

  // If hist is at the zhf level, rebin to make it a bit more reasonable.
    //if(histName(0,3)=="zhf" && !histName.Contains("mult")) h->Rebin(3);
    if(histName(0,3)=="zhf" && !histName.Contains("mult")) h->Rebin(rebinOpt);

  // Set color/fill/etc. Scale each clone by set weight if ds is a sim set.
    if(ds=="muon" || ds=="elec")
    {
        h->SetMarkerStyle(8);
        h->SetMarkerSize(0.8);
    }
    else
    {
        h->SetFillColor(dsColor[ds]);
        h->SetFillStyle(dsStyle[ds]);
        h->SetLineColor(kGray+3    );
        h->SetLineWidth(1          );
        if(ds=="dy_tautau" || ds=="dy_Zl" || ds=="dy_Zc" || ds=="dy_Zb" ) h->Scale(anCfg.setWeight["dy"]);
        else                                                              h->Scale(anCfg.setWeight[ ds ]);
    }
    h->SetTitleSize( 0.05, "x" );
    return h;
}


TH1F* ControlPlotMaker::createRatioPlot(TH1F* num, TH1F* denom)
{
    cout << "  Creating ratio for " << num->GetName() << endl;
    TH1F * h_ratio = (TH1F*) num->Clone("ratioHist");
    h_ratio->SetTitle("");
    h_ratio->GetYaxis()->SetTitle("Data/MC");
    // h_ratio->GetXaxis()->SetTitle("");
    h_ratio->SetTitleSize(   0.1 , "x" );
    h_ratio->SetTitleSize(   0.12, "y" );
    h_ratio->SetTitleOffset( 0.35, "y" );
    h_ratio->SetLabelSize(   0.08, "x" );
    h_ratio->SetLabelSize(   0.06, "y" );
    h_ratio->Divide(denom);
    h_ratio->SetMinimum(0.5);
    h_ratio->SetMaximum(1.5);

    return h_ratio;
}

// Creates a histogram that is sum of all
TH1F* ControlPlotMaker::createSimSumPlot(THStack* simStack)
{
    cout << " stack name: " << simStack->GetName();
    TList* histList   = simStack->GetHists()    ;   if(!histList  ) return NULL;
    TH1F*  sumSimHist = (TH1F*) histList->Last();   if(!sumSimHist) return NULL;
    sumSimHist = (TH1F*) sumSimHist->Clone("sumSimHist");

  // Clear histogram
    for(int i=0; i<sumSimHist->GetNbinsX(); i++)
    {
        sumSimHist->SetBinContent(0,0);
        sumSimHist->SetBinError(0,0);
    }
  // Add up all histograms
    TIter next(histList);
    TH1F* h_temp;
    while((h_temp = (TH1F*) next())) sumSimHist->Add(h_temp);
    return sumSimHist;

  /*
   TList *histos = hs->GetHists();
   TIter next(histos);
   TH1F *sum = new TH1F("sum","sum of histograms",100,-4,4);
   TH1F *hist;
   while ((hist =(TH1F*)next())) {
      cout << "Adding " << hist->GetName() << endl;
      sum->Add(hist);
   }
    return
  */
}
