// Macro that mixes Z+b/c/light jet templates with a given proportionality.
// Output used to test the template fitting's ability to extract a fraction from a sample.

{
  // Scale factors applied to templates to get the desired fraction of b/c/light jets.
    float scaleB = 45;
    float scaleC = 45;
    float scaleL = 10;

  // Input file locations
    TFile *inputTemplates = new TFile("svt_csvt_lnl.root");
    //TFile *outputFile     = new TFile("svt_csvt_lnl_DYCombo-15-45-40.root", "RECREATE");
    TString outTitle = TString::Format("svt_csvt_lnl_DYCombo-%2.0f-%2.0f-%2.0f.root", scaleB, scaleC, scaleL);
    cout << outTitle << endl;
    TFile *outputFile     = new TFile(outTitle, "RECREATE");

  // Change scale factors to decimals
    scaleB /= 100.0;   scaleC /= 100.0;   scaleL /= 100.0;

  // Extract input templates
    TH1F *inputB    = (TH1F*)inputTemplates->Get("bjets");
    TH1F *inputC    = (TH1F*)inputTemplates->Get("cjets");
    TH1F *inputL    = (TH1F*)inputTemplates->Get("ljets");
    TH1F *inputData = (TH1F*)inputTemplates->Get("muon");

  // Scale histograms to given fraction * integral of data plot in same file.
    scaleB *= inputData->Integral()/inputB->Integral();
    scaleC *= inputData->Integral()/inputC->Integral();
    scaleL *= inputData->Integral()/inputL->Integral();
    inputB->Scale(scaleB);
    inputC->Scale(scaleC);
    inputL->Scale(scaleL);

  // Add histograms together into an output histogram
    TH1F *output = (TH1F*)inputB->Clone("muon");
    output->Add(inputC);  output->Add(inputL);

  // Save to file
    output->Write();
    inputB->Write();
    inputC->Write();
    inputL->Write();

}