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
    int rebin     = 1;
    int tempScale = 1;

    TString fn_input  = "raw_cp_merges/2017-01-19_singleMuG_Trig_noSF_fileJSON/zc_rawcp_2017-01-19_singleMuG_Trig_noSF_fileJSON.root";
    TString fn_sample = "raw_cp_merges/2017-01-19_singleMuG_Trig_noSF_fileJSON/zc_rawcp_2017-01-19_singleMuG_Trig_noSF_fileJSON.root";

//////////////////////////////////////////////////
//////////////////////////////////////////////////

    gStyle->SetOptStat("");
    gROOT->ProcessLine(".L templateFitter.C");

    stringstream mainLog("");

    TH1F *h_b_Zuu, *h_c_Zuu, *h_l_Zuu;
    TH1F *h_b_Zee, *h_c_Zee, *h_l_Zee;
    TH1F *h_b_Zll, *h_c_Zll, *h_l_Zll;

  // Get templates from input
    getTemplatesFromRunIIFile(fn_input, h_b_Zuu, h_c_Zuu, h_l_Zuu, "Zuu", tempScale);
    getTemplatesFromRunIIFile(fn_input, h_b_Zee, h_c_Zee, h_l_Zee, "Zee", tempScale);
    getTemplatesFromRunIIFile(fn_input, h_b_Zll, h_c_Zll, h_l_Zll, "Zll", tempScale);

  // Get data sample from input
    TH1F* h_sample_Zuu = getRunIISampleFromFile(fn_sample, "Zuu");
    TH1F* h_sample_Zee = getRunIISampleFromFile(fn_sample, "Zee");
    TH1F* h_sample_Zll = getRunIISampleFromFile(fn_sample, "Zll");

  // Create output lable
    TString l_output = TString::Format("fit_REBIN_%d_TEMPSCL_%d", rebin, tempScale);

  // TEMPLATE TEST
    mainLog << templateFitter(l_output+"_Zuu"         , h_sample_Zuu, h_b_Zuu, h_c_Zuu, h_l_Zuu, rebin);
//    mainLog << templateFitter(l_output+"_Zee"         , h_sample_Zee, h_b_Zee, h_c_Zee, h_l_Zee, rebin);
//    mainLog << templateFitter(l_output+"_Zll"         , h_sample_Zll, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZuuDBC_ZllL" , h_sample_Zuu, h_b_Zuu, h_c_Zuu, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZeeDBC_ZllL" , h_sample_Zee, h_b_Zee, h_c_Zee, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZuuD_ZuuBCL" , h_sample_Zuu, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
//    mainLog << templateFitter(l_output+"_ZeeD_ZllBCL" , h_sample_Zee, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
    cout << mainLog.str();

    TString l_logname = TString::Format("fit_REBIN_%d_TEMPSCL_%d.log", rebin, tempScale);

  // Ouptut log to screen and file.
    string logFileName = "fits/" + fileTimeStamp() + "_" + l_logname.Data();
    ofstream logFile(logFileName.c_str());
    logFile << mainLog.str();
    logFile.close();

//    cout << "\n  CombineBCLTemplates.cpp ---- COMPLETE \n" << endl;

}
