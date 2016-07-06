// main_tempFitter.C
// ROOT macro. From a given data file, plots b/c/l efficiency plots using makeEffPlot()

// 2015-02-09 - used for closure test
// 2016-06-30 - Modified to pull templates, data, etc from control plots file.

{
    #include <sstream>
    #include <string>
    #include <fstream>


///////////////////////////////////////////////////
// PLACE TO MODIFY
///////////////////////////////////////////////////
    int rebin = 4;

    TString fn_input = "zc_cp_2016-06-15_w_trig.root";

//////////////////////////////////////////////////
//////////////////////////////////////////////////

    gStyle->SetOptStat("");
    gROOT->ProcessLine(".L templateFitter.C");

    TH1F *h_b, *h_c, *h_l;

  // Get templates from input
    getTemplatesFromRunIIFile(fn_input, h_b, h_c, h_l);

    stringstream mainLog("");

    TString fn_sample = "zc_cp_2016-06-15_w_trig.root";
    TH1F* h_sample = getRunIISampleFromFile(fn_sample);

  // Create output lable
    TString l_output = TString::Format("fit_REBIN_%d", rebin);

  // TEMPLATE TEST
    cout << "  TEMPLATE TEST: " << h_sample->GetTitle() << " " << h_b->GetTitle() << " " << h_c->GetTitle() << " " << h_l->GetTitle() << endl;
    mainLog << templateFitter(l_output , h_sample, h_b, h_c, h_l, rebin);
    cout << mainLog.str();

    TString l_logname = TString::Format("fit_REBIN_%d.log", rebin);

  // Ouptut log to screen and file.
    string logFileName = "fits/" + fileTimeStamp() + "_" + l_logname.Data();
    ofstream logFile(logFileName.c_str());
    logFile << mainLog.str();
    logFile.close();

    cout << "\n  CombineBCLTemplates.cpp ---- COMPLETE \n" << endl;

}
