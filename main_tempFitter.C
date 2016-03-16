// main_tempFitter.C
// ROOT macro. From a given data file, plots b/c/l efficiency plots using makeEffPlot()

// 2015-02-09 - used for closure test

{
    #include <sstream>
    #include <string>

    gStyle->SetOptStat("");
    gROOT->ProcessLine(".L templateFitter.C");

    stringstream mainLog("");
//    mainLog << templateFitter("fit_temp_svt_csvt_45-45-10", new TFile("svt_csvt_lnl_DYCombo-45-45-10.root"));
//    mainLog << templateFitter("fit_temp_svt_csvt_60-30-10", new TFile("svt_csvt_lnl_DYCombo-60-30-10.root"));
//    mainLog << templateFitter("fit_temp_svt_csvt_70-20-10", new TFile("svt_csvt_lnl_DYCombo-70-20-10.root"));
    mainLog << templateFitter("fit_temp_svt_csvt_85-10-5" , new TFile("svt_csvt_lnl_DYCombo-85-10- 5.root" ));

    cout << mainLog.str();
    
  // Ouptut log to screen and file.
    string logFileName = "fits/" + fileTimeStamp() + "_templateFitting.log";
    ofstream logFile(logFileName.c_str());
    logFile << mainLog.str();
    logFile.close();

}