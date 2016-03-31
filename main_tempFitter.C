// main_tempFitter.C
// ROOT macro. From a given data file, plots b/c/l efficiency plots using makeEffPlot()

// 2015-02-09 - used for closure test

{
    #include <sstream>
    #include <string>
    #include <fstream>


///////////////////////////////////////////////////
// PLACE TO MODIFY
///////////////////////////////////////////////////
    int rebin = 1;

    // S: py8 B: py6 FULL
    TString l_sample    = "dy_py8";
    TString l_templates = "dy";

//     S: dy_lpc_odd B: dy_lpc_even
//    TString l_sample    = "dy_lpc_odd";
//    TString l_templates = "dy_lpc_even";

    // S: dy_lpc_even B: dy_lpc_odd
//    TString l_sample    = "dy_lpc_even";
//    TString l_templates = "dy_lpc_odd";

    // S: py6  B: py6
//    TString l_sample    = "dy_lpc";
//    TString l_templates = "dy_lpc";

    // S: py6_top  B: py6_bottom
//    TString l_sample    = "dy_lpc_top"   ;
//    TString l_templates = "dy_lpc_bottom";

    // S: py6_bottom  B: py6_top
//    TString l_sample    = "dy_lpc_bottom";
//    TString l_templates = "dy_lpc_top"   ;

    // S: py6 B: py8
//    TString l_sample    = "dy_lpc";
//    TString l_templates = "dy_py8";

    // S: py8 B: py6
//    TString l_sample    = "dy_py8";
//    TString l_templates = "dy_lpc";

    // S: py8 B: py6
//    TString l_sample    = "dy_lpc";
//    TString l_templates = "dy1j";


    TString fn_sample_pre = TString::Format("bcl_combo_samples/zcBCLComb_%s_", l_sample   .Data());      // Only the prefix. The rest (percentages and suffix) will be added in loop.
    TString fn_templates  = TString::Format("templates/zc_templates_%s.root" , l_templates.Data());

//////////////////////////////////////////////////
//////////////////////////////////////////////////


    gStyle->SetOptStat("");
    gROOT->ProcessLine(".L templateFitter.C");

    TH1F *h_b, *h_c, *h_l;

  // Get templates from input
    getTemplatesFromFile(fn_templates, h_b, h_c, h_l);
    //getTemplatesFromOldFile(h_b, h_c, h_l);

  // Input percentages from file
    float ipB, ipC, ipL;      // input percentages for each flavor
    ifstream percentFile("closureTestPercentages.txt");
    stringstream mainLog("");

    while(percentFile >> ipB >> ipC >> ipL)
    {
        TString l_ratio = TString::Format("%02.0f-%02.0f-%02.0f", ipB, ipC, ipL);
        TString fn_sample = fn_sample_pre + l_ratio + ".root";
        TH1F* h_sample = getSampleFromFile(fn_sample);
        //TH1F* h_sample = getSampleFromOldFile();

      // Create output lable
        TString l_output = TString::Format("fit_TEMPS_%s_ON_%s_WITH_RATIOS_%s_REBIN_%d", l_templates.Data(), l_sample.Data(), l_ratio.Data(), rebin);

      // TEMPLATE TEST
        //cout << "  TEMPLATE TEST: " << h_sample->GetTitle() << " " << h_b->GetTitle() << " " << h_c->GetTitle() << " " << h_l->GetTitle() << endl;

        mainLog << "l_sample:  " << l_ratio   << endl;
        mainLog << "fn_sample: " << fn_sample << endl;
        mainLog << templateFitter(l_output , h_sample, h_b, h_c, h_l, rebin);
        cout << mainLog.str();

    }

    TString l_logname = TString::Format("fit_TEMPS_%s_ON_%s_REBIN_%d.log", l_templates.Data(), l_sample.Data(), rebin);

  // Ouptut log to screen and file.
    string logFileName = "fits/" + fileTimeStamp() + "_" + l_logname.Data();
    ofstream logFile(logFileName.c_str());
    logFile << mainLog.str();
    logFile.close();

    cout << "\n  CombineBCLTemplates.cpp ---- COMPLETE \n" << endl;

}
