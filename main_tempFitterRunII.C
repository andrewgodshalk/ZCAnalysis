// main_tempFitter.C
// ROOT macro. From a given data file, plots b/c/l efficiency plots using makeEffPlot()

// 2015-02-09 - used for closure test
// 2016-06-30 - Modified to pull templates, data, etc from control plots file.
// 2016-09-01 - Modified to pull templates based on channel.

{
    #include <sstream>
    #include <string>
    #include <fstream>


///////////////////////////////////////////////////
// PLACE TO MODIFY
///////////////////////////////////////////////////
    int rebin = 3;

//    TString fn_input = "zc_cp_2016-06-15_w_trig.root";
//    TString fn_sample = "zc_cp_2016-06-15_w_trig.root";
    TString fn_input =  "zc_cp_2016-07-07_wElec.root";
    TString fn_sample = "zc_cp_2016-07-07_wElec.root";

//////////////////////////////////////////////////
//////////////////////////////////////////////////

    gStyle->SetOptStat("");
    gROOT->ProcessLine(".L templateFitter.C");

    stringstream mainLog("");

    TH1F *h_b_Zuu, *h_c_Zuu, *h_l_Zuu;
    TH1F *h_b_Zee, *h_c_Zee, *h_l_Zee;
    TH1F *h_b_Zll, *h_c_Zll, *h_l_Zll;

  // Get templates from input
    getTemplatesFromRunIIFile(fn_input, h_b_Zuu, h_c_Zuu, h_l_Zuu, "Zuu");
    cout << "TEST: Zuu Temps Extracted" << endl;
    getTemplatesFromRunIIFile(fn_input, h_b_Zee, h_c_Zee, h_l_Zee, "Zee");
    cout << "TEST: Zee Temps Extracted" << endl;
    getTemplatesFromRunIIFile(fn_input, h_b_Zll, h_c_Zll, h_l_Zll, "Zll");
    cout << "TEST: Zll Temps Extracted" << endl;

    TH1F* h_sample_Zuu = getRunIISampleFromFile(fn_sample, "Zuu");
    cout << "TEST: Zuu Sample Extracted" << endl;
    TH1F* h_sample_Zee = getRunIISampleFromFile(fn_sample, "Zee");
    cout << "TEST: Zee Sample Extracted" << endl;
    TH1F* h_sample_Zll = getRunIISampleFromFile(fn_sample, "Zll");
    cout << "TEST: Zll Sample Extracted" << endl;

  // Create output lable
    TString l_output = TString::Format("fit_REBIN_%d", rebin);

  // TEMPLATE TEST
//    cout << "  TEMPLATE TEST: " << h_sample->GetTitle() << " " << h_b->GetTitle() << " " << h_c->GetTitle() << " " << h_l->GetTitle() << endl;
//    mainLog << templateFitter(l_output+"_Zuu"         , h_sample_Zuu, h_b_Zuu, h_c_Zuu, h_l_Zuu, rebin);
//    mainLog << templateFitter(l_output+"_Zee"         , h_sample_Zee, h_b_Zee, h_c_Zee, h_l_Zee, rebin);
//    mainLog << templateFitter(l_output+"_Zll"         , h_sample_Zll, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZuuDBC_ZllL" , h_sample_Zuu, h_b_Zuu, h_c_Zuu, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZeeDBC_ZllL" , h_sample_Zee, h_b_Zee, h_c_Zee, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZuuD_ZllBCL" , h_sample_Zuu, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZeeD_ZllBCL" , h_sample_Zee, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
//    cout << mainLog.str();

    TString l_logname = TString::Format("fit_REBIN_%d.log", rebin);

  // Ouptut log to screen and file.
    string logFileName = "fits/" + fileTimeStamp() + "_" + l_logname.Data();
    ofstream logFile(logFileName.c_str());
    logFile << mainLog.str();
    logFile.close();

    cout << "\n  CombineBCLTemplates.cpp ---- COMPLETE \n" << endl;

}
