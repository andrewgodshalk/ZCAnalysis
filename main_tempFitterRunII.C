// main_tempFitter.C
// ROOT macro. From a given data file, plots b/c/l efficiency plots using makeEffPlot()

// 2015-02-09 - used for closure test
// 2016-06-30 - Modified to pull templates, data, etc from control plots file.
// 2016-09-01 - Modified to pull templates based on channel.
// 2017-02-09 - Works w/ RunII PR ntuple samples.

{
    #include <sstream>
    #include <string>
    #include <fstream>


  ///////////////////////////////////////////////////
  // PLACE TO MODIFY
  ///////////////////////////////////////////////////
    int rebin     = 4;
    int tempScale = 1;

    TString fn_input, fn_sample;
    TString fn_2016RRG = "/home/godshalk/Work/2016-12-07_ZCAnalysis2016_OldCode/raw_cp_merges/2017-02-15_ReReco_dMuG_mm_for_jetEff/zc_rawcp_2017-02-15_ReReco_dMuG_mm.root";
    // TString fn_2016BCDamc = "raw_cp_merges/2017-02-04_HadFlv_BTagEffAMC_dMuBCD_amc""/zc_rawcp_2017-02-04_HadFlv_BTagEffAMC_dMuBCD_amc"".root";
    // TString fn_2016BCDmm  = "raw_cp_merges/2017-02-04_HadFlv_BTagEffMM_dMuBCD_mm"  "/zc_rawcp_2017-02-04_HadFlv_BTagEffMM_dMuBCD_mm"  ".root";
    // TString fn_2016Gamc   = "raw_cp_merges/2017-02-04_HadFlv_BTagEffAMC_dMuG_amc"  "/zc_rawcp_2017-02-04_HadFlv_BTagEffAMC_dMuG_amc"  ".root";
    // TString fn_2016Gmm    = "raw_cp_merges/2017-02-04_HadFlv_BTagEffMM_dMuG_mm"    "/zc_rawcp_2017-02-04_HadFlv_BTagEffMM_dMuG_mm"    ".root";
    fn_input = fn_sample = fn_2016RRG;
    // fn_input = fn_sample = fn_2016BCDmm ;
    // fn_input = fn_sample = fn_2016Gamc  ;
    // fn_input = fn_sample = fn_2016Gmm   ;
  // NOTE: Need to change dataset weights in templateFitter.C for different datasets (getRunIISampleFromFile(), Line 405)

  //////////////////////////////////////////////////
  //////////////////////////////////////////////////

    gStyle->SetOptStat("");
    gROOT->ProcessLine(".L templateFitter.C");

    stringstream mainLog("");


  // Get templates from input
    cout << "  Getting templates from input file: " << fn_input << endl;
    TH1F *h_b_Zuu, *h_c_Zuu, *h_l_Zuu;
    TH1F *h_b_Zee, *h_c_Zee, *h_l_Zee;
    TH1F *h_b_Zll, *h_c_Zll, *h_l_Zll;
    getTemplatesFromRunIIFile(fn_input, h_b_Zuu, h_c_Zuu, h_l_Zuu, "Zuu", tempScale);
    // getTemplatesFromRunIIFile(fn_input, h_b_Zee, h_c_Zee, h_l_Zee, "Zee", tempScale);
    // getTemplatesFromRunIIFile(fn_input, h_b_Zll, h_c_Zll, h_l_Zll, "Zll", tempScale);

  // Get data sample from input
    cout << "  Getting data sample from file: " << fn_sample << endl;
    TH1F* h_sample_Zuu = getRunIISampleFromFile(fn_sample, "Zuu");
    // TH1F* h_sample_Zee = getRunIISampleFromFile(fn_sample, "Zee");
    // TH1F* h_sample_Zll = getRunIISampleFromFile(fn_sample, "Zll");

  // Create output lable
    TString l_output = TString::Format("fit_REBIN_%d_TEMPSCL_%d", rebin, tempScale);

  // TEMPLATE TEST
    cout << "  Fitting templates, saving to output: " << l_output << endl;
    mainLog << templateFitter(l_output+"_Zuu"         , h_sample_Zuu, h_b_Zuu, h_c_Zuu, h_l_Zuu, rebin);
    // mainLog << templateFitter(l_output+"_Zee"         , h_sample_Zee, h_b_Zee, h_c_Zee, h_l_Zee, rebin);
    // mainLog << templateFitter(l_output+"_Zll"         , h_sample_Zll, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
    // mainLog << templateFitter(l_output+"_ZuuDBC_ZllL" , h_sample_Zuu, h_b_Zuu, h_c_Zuu, h_l_Zll, rebin);
    // mainLog << templateFitter(l_output+"_ZeeDBC_ZllL" , h_sample_Zee, h_b_Zee, h_c_Zee, h_l_Zll, rebin);
    // mainLog << templateFitter(l_output+"_ZuuD_ZuuBCL" , h_sample_Zuu, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
    // mainLog << templateFitter(l_output+"_ZeeD_ZllBCL" , h_sample_Zee, h_b_Zll, h_c_Zll, h_l_Zll, rebin);
    cout << mainLog.str();

    TString l_logname = TString::Format("fit_REBIN_%d_TEMPSCL_%d.log", rebin, tempScale);

  // Ouptut log to screen and file.
    string logFileName = "fits/" + fileTimeStamp() + "_" + l_logname.Data();
    ofstream logFile(logFileName.c_str());
    logFile << mainLog.str();
    logFile.close();

    cout << "\n  CombineBCLTemplates.cpp ---- COMPLETE \n" << endl;

}
