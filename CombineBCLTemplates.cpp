/*-------------------------------------------------------------------

  2016-03-15 CombineBCLTemplates.cpp
    - Adapted from code from 2015-02

  Code that takes Z+c flavor templates extracted from DY by the NTupleProcessor's TemplateExtractor
  and creates a sample that is comprised of input percentages of each flavor template.
  - Percentages are input from file.
  - Also saves templates in a more streamlined file.

    TO DO: Make this a class.

./CombineBCLTemplates.exe templates/zc_analysis_dy1j.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_local_bottom.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_local.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_local_top.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_lpc_bottom.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_lpc.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_lpc_top.root
./CombineBCLTemplates.exe templates/zc_analysis_dy_py8.root


*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <TFile.h>
#include <TH1F.h>
#include <TString.h>

using std::cout;         using std::endl;
using std::cin;          using std::ifstream;
using std::setprecision; using std::setw;

bool addTo100(int, int, int);
void combineTemplates(TString, TH1F*, TH1F*, TH1F*, float, float, float);
void extractTemplates(TFile*, TH1F*&, TH1F*&, TH1F*&);
void writeTemplatesToFile(TString, TH1F*, TH1F*, TH1F*);
TString getFileLabel(TString);

int main(int argc, char* argv[])
{
    cout << "\n  CombineBCLTemplates.cpp ---- BEGIN \n" << endl;

  // Input variables
    int ipB, ipC, ipL;

  // Input file locations
    if(argc<=1){ cout << "    Enter a file, damnit!\n" << endl; return 1;}
    TString fn_input(argv[1]);
    TString inputLabel = getFileLabel(fn_input);

  // Open input file
    TFile *inputFile = TFile::Open(fn_input);
    if(!inputFile){ cout << "    That's not a file, dummy!\n" << endl; return 1;}
    cout << "    Opening file: " << fn_input << endl;

  // Extract histograms from input
    TH1F *h_b, *h_c, *h_l;
    extractTemplates(inputFile, h_b, h_c, h_l);

  // Write the templates you extracted to file
    writeTemplatesToFile(inputLabel, h_b, h_c, h_l);

  // Input percentages from file
    ifstream percentFile("closureTestPercentages.txt");
    while(percentFile >> ipB >> ipC >> ipL)
    {
        if(addTo100(ipB,ipC,ipL)) combineTemplates(inputLabel, h_b, h_c, h_l, ipB, ipC, ipL);
        else cout << "ERROR: Bad input percentages: " << ipB << " " << ipC << " "  << ipL << "\n" << endl;
    }
    cout << "\n  CombineBCLTemplates.cpp ---- COMPLETE \n" << endl;

  // Clean up
    delete h_b;
    delete h_c;
    delete h_l;
    inputFile->Close();

return 0;
}


bool addTo100(int b, int c, int l) {return b+c+l == 100;}


void combineTemplates(  TString inputLabel,
                        TH1F* h_b, TH1F* h_c, TH1F* h_l,
                        float scaleB, float scaleC, float scaleL
                     )
{
  // DATA SCALE VALUE
    float dataScale = 50000;

  // Create output filename from input values.
    TString outFileName = TString::Format("bcl_combo_samples/zcBCLComb_%s_%02.0f-%02.0f-%02.0f.root", inputLabel.Data(), scaleB, scaleC, scaleL);

  // Change percentages to decimals.
    scaleB /= 100; scaleC /= 100; scaleL /= 100;

  // Test output
    cout << setprecision(2) << setw(6) << scaleB << " " << setw(6) << scaleC << " " << setw(6) << scaleL << " --> " << outFileName << endl;

  // Further process scales, scaling to approx. 50,000 events
    scaleB *= 51152 / h_b->Integral();
    scaleC *= 51152 / h_c->Integral();
    scaleL *= 51152 / h_l->Integral();

  // Set up output file.
    TFile *outputFile = TFile::Open(outFileName, "RECREATE");

  // Combine histograms into one large histogram, scaled properly.
    TH1F* h_sample = (TH1F*) h_b->Clone("h_sample");
    h_sample->Scale(scaleB);
    h_sample->Add(h_c, scaleC);
    h_sample->Add(h_l, scaleL);

  // Write to file and clean up.
    outputFile->cd();
    h_sample->Write();
    delete h_sample;
    outputFile->Close();
}


TString getFileLabel(TString fn_input)
{ // Assuming file has format: templates/zc_analysis_<label>.root
    int labelSize = fn_input.Length() - 27;
    TString fnLabel(fn_input(22, labelSize));
return fnLabel;
}


void extractTemplates(TFile* inputFile, TH1F*& h_b, TH1F*& h_c, TH1F*& h_l)
{
  // Temporary histogram storage
    TH1F *h_b_Zee, *h_c_Zee, *h_l_Zee, *h_b_Zuu, *h_c_Zuu, *h_l_Zuu;

  // Set up input path (specifically, which dataset is used, dy or dy1j
    TString path = "templates/";
    TString fn_input = inputFile->GetName();
    path+= (fn_input.Contains("dy1j") ? "dy1j/" : "dy/");

  // Get templates from file
    inputFile->GetObject(path+"Zee_Zb/template_CSVT", h_b_Zee); h_b_Zee->Sumw2();
    inputFile->GetObject(path+"Zee_Zc/template_CSVT", h_c_Zee); h_c_Zee->Sumw2();
    inputFile->GetObject(path+"Zee_Zl/template_CSVT", h_l_Zee); h_l_Zee->Sumw2();
    inputFile->GetObject(path+"Zuu_Zb/template_CSVT", h_b_Zuu); h_b_Zuu->Sumw2();
    inputFile->GetObject(path+"Zuu_Zc/template_CSVT", h_c_Zuu); h_c_Zuu->Sumw2();
    inputFile->GetObject(path+"Zuu_Zl/template_CSVT", h_l_Zuu); h_l_Zuu->Sumw2();

  // Add templates together to get final templates
    h_b = (TH1F*) h_b_Zee->Clone("h_b");   h_b->Add(h_b_Zuu);
    h_c = (TH1F*) h_c_Zee->Clone("h_c");   h_c->Add(h_c_Zuu);
    h_l = (TH1F*) h_l_Zee->Clone("h_l");   h_l->Add(h_l_Zuu);

  // Clean up
    delete h_b_Zee;  delete h_c_Zee;  delete h_l_Zee;
    delete h_b_Zuu;  delete h_c_Zuu;  delete h_l_Zuu;
}


void writeTemplatesToFile(TString inputLabel, TH1F* h_b, TH1F* h_c, TH1F* h_l)
{
  // Set up output file
    TString fn_output = TString::Format("templates/zc_templates_%s.root", inputLabel.Data());
    TFile* f_output = TFile::Open(fn_output, "RECREATE");  f_output->cd();
  // Write the histograms to file, then close.
//    h_b->Scale(1.0/h_b->Integral());
//    h_c->Scale(1.0/h_c->Integral());
//    h_l->Scale(1.0/h_l->Integral());
    h_b->Write();
    h_c->Write();
    h_l->Write();
    f_output->Close();
}
