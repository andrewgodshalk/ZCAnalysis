/*-------------------------------------------------------------------

  2016-03-15 CombineBCLTemplates.cpp
    - Adapted from code from 2015-02

    TO DO: Fix scale factor.
    TO DO: Make this a class.

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
void extractTemplates(TFile*, TH1F*, TH1F*, TH1F*);
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
    cout << "WEEEE" << endl;

    cout << "    h_b title = " << h_b->GetTitle() << endl;


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
  // Create output filename from input values.
    TString outFileName = TString::Format("bcl_combo_samples/zcBCLComb_%s_%02.0f-%02.0f-%02.0f.root", inputLabel.Data(), scaleB, scaleC, scaleL);

  // Change percentages to decimals.
    scaleB /= 100; scaleC /= 100; scaleL /= 100;

  // Test output
    cout << setprecision(2) << setw(6) << scaleB << " " << setw(6) << scaleC << " " << setw(6) << scaleL << " --> " << outFileName << endl;

  // Further process scales, scaling to approx. 50,000 events
    scaleB *= 50000 / h_b->Integral();
    scaleC *= 50000 / h_c->Integral();
    scaleL *= 50000 / h_l->Integral();

  // Set up output file.
    TFile *outputFile = TFile::Open(outFileName, "RECREATE");
    outputFile->cd();

  // Combine histograms into one large histogram, scaled properly.
    TH1F* h_sample = (TH1F*) h_b->Clone("h_sample");
    h_sample->Scale(scaleB);
    h_sample->Add(h_c, scaleC);
    h_sample->Add(h_l, scaleL);

  // Write to file and clean up.
    h_sample->Write();
    delete h_sample;
    outputFile->Close();
}


TString getFileLabel(TString fn_input)
{ // Assuming file has format: templates/zc_temps_<label>.root
    int labelSize = fn_input.Length() - 24;
    TString fnLabel(fn_input(19, labelSize));
return fnLabel;
}

void extractTemplates(TFile* inputFile, TH1F* h_b, TH1F* h_c, TH1F* h_l)
{
  // Temporary histogram storage
    TH1F *h_b_Zee, *h_c_Zee, *h_l_Zee, *h_b_Zuu, *h_c_Zuu, *h_l_Zuu;

  // Set up input path (specifically, which dataset is used, dy or dy1j
    TString path = "templates/";
    TString fn_input = inputFile->GetName();
    path+= (fn_input.Contains("dy1j") ? "dy1j/" : "dy/");

  // Get templates from file
    inputFile->GetObject(path+"Zee_Zb/template_CSVT", h_b_Zee);
    inputFile->GetObject(path+"Zee_Zc/template_CSVT", h_c_Zee);
    inputFile->GetObject(path+"Zee_Zl/template_CSVT", h_l_Zee);
    inputFile->GetObject(path+"Zuu_Zb/template_CSVT", h_b_Zuu);
    inputFile->GetObject(path+"Zuu_Zc/template_CSVT", h_c_Zuu);
    inputFile->GetObject(path+"Zuu_Zl/template_CSVT", h_l_Zuu);

  // Add templates together to get final templates
    h_b = (TH1F*) h_b_Zee->Clone("h_b");   h_b->Add(h_b_Zuu);
    h_c = (TH1F*) h_c_Zee->Clone("h_c");   h_c->Add(h_c_Zuu);
    h_l = (TH1F*) h_l_Zee->Clone("h_l");   h_l->Add(h_l_Zuu);

  // Clean up
    delete h_b_Zee;  delete h_c_Zee;  delete h_l_Zee;
    delete h_b_Zuu;  delete h_c_Zuu;  delete h_l_Zuu;
}
