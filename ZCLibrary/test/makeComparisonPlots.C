// ROOT SCRIPT THAT TAKES TWO


void makeComparisonPlots()
{
  // TFile* outputFile = TFile::Open("PlotComparison.root", "RECREATE");
  // vector<TFile*> files = {
  //     TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/raw_cp_merges/zc_rawcp_2017-03-30_septmg00.root"),
  //     TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/raw_cp_merges/zc_rawcp_2017-03-30_septmg01.root"),
  //     TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/raw_cp_merges/zc_rawcp_2017-03-30_septmg02.root"),
  //     TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/raw_cp_merges/zc_rawcp_2017-03-30_septmg03.root"),
  //     TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/raw_cp_merges/zc_rawcp_2017-03-16_SeptRRMG.root"),
  // };
  // vector<TString> labels = {
  //     " 00 Old   ",
  //     " 01 Trig  ",
  //     " 02 Iso   ",
  //     " 03 SF    ",
  //     "-01 Older ",
  // };
  // vector<TString> plots = {
  //     "control_plots/elec/Zee/vldll/vldll_dilepton_mass",
  //     "control_plots/muon/Zuu/vldll/vldll_dilepton_mass",
  //     "control_plots/dy/Zuu/vldll/vldll_dilepton_mass",
  //     "control_plots/dy/Zee/vldll/vldll_dilepton_mass",
  //     "control_plots/tt/Zee/vldll/vldll_dilepton_mass",
  //     "control_plots/ww/Zee/vldll/vldll_dilepton_mass",
  //     "control_plots/zz/Zee/vldll/vldll_dilepton_mass",
  // };

    TFile* outputFile = TFile::Open("FixComparisons.root", "RECREATE");
    vector<TFile*> files = {
        TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/ZCLibrary/test/zc_out_febRR_double_elec_G_1of3_wofix.root"),
        TFile::Open("/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/ZCLibrary/test/zc_out_febRR_double_elec_G_1of3_wfix.root" ),
    };
    vector<TString> labels = {
        "Analyzer Fix",
        "  Ntuple Fix",
    };

    vector<TString> plots = {
        "control_plots/elec/Zee/vldll/vldll_dilepton_mass",
    };

    // black red blue green magenta
    vector<int> colors = {1, 2, 4, 8, 6};
    int lineWidth = 2;

    outputFile->cd();
    for(  TString& plot : plots )
    {
        TCanvas* c = new TCanvas(plot, "", 800, 800);
        TLegend *leg = new TLegend(0.55,0.60,0.95,0.89);
        TString drawOption  = "hist";
        TString ratioOption = "ep"  ;
        float plotMax = 1.0;

      //
        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1->SetBottomMargin(0); // Upper and lower plot are joined
        pad1->Draw();
        c->cd();
        TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
        pad2->SetGridy();         // Vertical grid
        pad2->SetTopMargin(0);
        pad2->SetBottomMargin(0.2);
        pad2->Draw();

        TH1F* comparisonPlot = 0;

        for(int i=0; i<files.size(); i++)
        { // Add plot to upper plot.
            pad1->cd();
            TH1F* tempPlot = (TH1F*) files[i]->Get(plot);
            tempPlot->SetLineColor(colors[i]);
            tempPlot->SetLineWidth(lineWidth);
            tempPlot->SetStats(0);
            tempPlot->Draw(drawOption);
            // if(plotMax < tempPlot->GetMaximum()) plotMax = tempPlot->GetMaximum();
            if(drawOption == "hist") drawOption = "histSame";  // Draw on same canvas after first draw.
            TString label = labels[i] + TString::Format("E:%.1f I:%.1f", tempPlot->GetEntries(), tempPlot->Integral());
            leg->AddEntry(tempPlot, label, "L");

          // Add ratio to plot 2, or set up
            pad2->cd();       // pad2 becomes the current pad
            // If comparisonPlot doesn't exist, create it. Otherwise, use it for ratio.
            if(!comparisonPlot) comparisonPlot = (TH1F*) tempPlot->Clone(TString(tempPlot->GetName())+"_ratio");
            else
            {   TH1F* ratioPlot = (TH1F*) tempPlot->Clone(TString(tempPlot->GetName())+"_ratio");
                // ratioPlot->Sumw2();
                if(ratioOption == "ep") ratioOption = "epSame";  // Draw on same canvas after first draw.
                // ratioPlot->SetMarkerStyle(21);
                ratioPlot->SetMarkerColor(colors[i]);
                ratioPlot->SetStats(0);
                ratioPlot->SetMinimum(0.5);
                ratioPlot->SetMaximum(1.5);
                ratioPlot->Divide(comparisonPlot);
                ratioPlot->Draw(ratioOption);
            }
        }

        pad1->cd();
        leg->SetBorderSize(0);
        leg->SetFillStyle(0) ;
        leg->Draw()          ;
        c->SetLogy()         ;
        c->Write()           ;
    }
    for(TFile* file : files ) file->Close();
    outputFile->Close();
}
