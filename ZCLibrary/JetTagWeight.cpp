/*------------------------------------------------------------------------------
JetTagWeight.cpp

 Created : 2017-02-03  godshalk
 Modified: 2017-02-03  godshalk

------------------------------------------------------------------------------*/

#include "JetTagWeight.h"   // Class header
#include <iostream>
#include <string>
#include <iostream>
#include "TFile.h"
#include "TString.h"

using std::string;
using std::cout;   using std::endl;

JetTagWeight::JetTagWeight()
: fn_eff_(""), fn_csv_sf_(""), fn_chm_sf_(""), effLoaded_(false),
  effPtMin_(-1), effPtMax_(-1), effEtaMin_(-1), effEtaMax_(-1),
  csvSFLoaded_(false), chmSFLoaded_(false),
  histHolder_(new TDirectory("JetTagWeight_histHolder_", "JetTagWeight Histogram Holder"))
{ // Set up maps from my labels to BTV's enums.
    opPtMap_   = { {"NoHF", BTagEntry::OP_LOOSE},
                   {"CSVL", BTagEntry::OP_LOOSE}, {"CSVM", BTagEntry::OP_MEDIUM},
                   {"CSVT", BTagEntry::OP_TIGHT}, {"CSVS", BTagEntry::OP_TIGHT },
                   {"ChmL", BTagEntry::OP_LOOSE}, {"ChmM", BTagEntry::OP_MEDIUM},
                   {"ChmT", BTagEntry::OP_TIGHT}
                 };
    flvMap_    = { {'b',BTagEntry::FLAV_B}, {'c', BTagEntry::FLAV_C}, {'l', BTagEntry::FLAV_UDSG} };
    flvSetCSV_ = { {'b',"comb"}, {'c',"comb"}, {'l',"incl"} };
    flvSetChm_ = { {'b',"TnP"} , {'c',"comb"}, {'l',"incl"} };
    flavor_    = { 'b', 'c', 'l' };
    opPt_      = { "NoHF", "CSVL", "CSVM", "CSVT", "CSVS", "ChmL", "ChmM", "ChmT",
                   "BCLL","BCLM","BCLT","BCML","BCMM","BCMT","BCTL","BCTM","BCTT"  // combined b/c tagger
                 };
    svTypes_   = { "noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
}


bool JetTagWeight::setEffFile(const string& fn)
{
    cout << "  JetTagWeight: Loading Efficiency plots from file: " << fn << "..." << endl;
    fn_eff_ = fn;
    TFile* effFile = TFile::Open(fn_eff_.c_str());
    histHolder_->cd();
    for( char& f : flavor_)
        for( string& op : opPt_)
            for( string& sv : svTypes_)
    {   TString plot_name = TString::Format("dy_Zll_zpjmet_%s%s_%cJetEff", op.c_str(), sv.c_str(), f);
        cout << "    Getting eff plot: " << plot_name;
        TH2F* temp = (TH2F*) effFile->Get(plot_name.Data());
        if(temp != NULL)
        {   jetTagEff_[f][op][sv] = (TH2F*) temp->Clone((plot_name+"_clone").Data());
            cout << " --> " << jetTagEff_[f][op][sv]->GetName() << endl;
        }
        else cout << " --> PLOT NOT FOUND, ASSUMING EFF. = 1.0" << endl;
    }
    effFile->Close();
    // for( char& f : flavor_)
    //     for( string& op : opPt_)
    //         for( string& sv : svTypes_)
    //             cout << "JetTagWeight::setEffFile(" << fn << "): Loaded from file: " << jetTagEff_[f][op][sv]->GetName() << endl;
    effLoaded_ = true;
    effPtMin_  = jetTagEff_['l']["CSVT"]["pfISV"]->GetXaxis()->GetXmin();
    effPtMax_  = jetTagEff_['l']["CSVT"]["pfISV"]->GetXaxis()->GetXmax();
    effEtaMin_ = jetTagEff_['l']["CSVT"]["pfISV"]->GetYaxis()->GetXmin();
    effEtaMax_ = jetTagEff_['l']["CSVT"]["pfISV"]->GetYaxis()->GetXmax();
    // cout << TString::Format("    (Loaded dimensions: {pt}x{eta} = {%f,%f}x{%f,%f})", effPtMin_, effPtMax_, effEtaMin_, effEtaMax_) << endl;
    // cout << "  Complete!" << endl;
    return true;
}


bool JetTagWeight::setCSVSFFile( const string& fn)
{
    fn_csv_sf_ = fn;
    string tagger = "CSVv2";
    csvCalib_ = BTagCalibration(tagger, fn_csv_sf_);
    for( string& op : opPt_) if( op.substr(0,3) == "CSV" || op == "NoHF")
    {   btagCalibReader_[op] = BTagCalibrationReader(opPtMap_[op], "central", {"up","down"});
        for( char& f : flavor_)
            btagCalibReader_[op].load(csvCalib_, flvMap_[f], flvSetCSV_[f]);
    }
    csvSFLoaded_ = true;
    return true;
}


bool JetTagWeight::setChmSFFile( const string& fn)
{
    fn_chm_sf_ = fn;
    string tagger = "cTag";
    chmCalib_ = BTagCalibration(tagger, fn_chm_sf_);
    for( string& op : opPt_) if( op.substr(0,3) == "Chm")
    {
        btagCalibReader_[op] = BTagCalibrationReader(opPtMap_[op], "central", {"up","down"});
        for( char& f : flavor_)
            btagCalibReader_[op].load(chmCalib_, flvMap_[f], flvSetChm_[f]);
    }
    chmSFLoaded_ = true;
    return true;
}


float JetTagWeight::getJetEff(char flv, string opPt, string svType, float pt, float eta)
{
    // cout << TString::Format("JetTagWeight::getJetEff(%c, %s, %s, %f, %f)", flv, opPt.c_str(), svType.c_str(), pt, eta) << endl;
    float eff = 1.0;
    if(effLoaded_ && jetTagEff_[flv][opPt][svType] != NULL)
    { // cout << "    ...loading eff from " << jetTagEff_[flv][opPt][svType]->GetTitle() << "..." << endl;
      // Check to see that pt, eta are within bounds of eff bins. If not, use last viable bin.
        int globalBinNum = jetTagEff_[flv][opPt][svType]->FindBin(pt, eta);
        if( pt  < effPtMin_ ) { pt  = effPtMin_ +0.001; globalBinNum = jetTagEff_[flv][opPt][svType]->FindBin(pt, eta); }
        if( pt  > effPtMax_ ) { pt  = effPtMax_ -0.001; globalBinNum = jetTagEff_[flv][opPt][svType]->FindBin(pt, eta); }
        if( eta < effEtaMin_) { eta = effEtaMin_+0.001; globalBinNum = jetTagEff_[flv][opPt][svType]->FindBin(pt, eta); }
        if( eta > effEtaMax_) { eta = effEtaMax_-0.001; globalBinNum = jetTagEff_[flv][opPt][svType]->FindBin(pt, eta); }
      // Load efficiency
        eff = jetTagEff_[flv][opPt][svType]->GetBinContent(globalBinNum);
        // cout << "    ...from file: " << eff << endl;
    }
    return eff;
}

float JetTagWeight::getJetSF(char flv, string opPt, float pt, float eta, string type)
{
    float sf = 1.0;

  // Evaluate the input type.
    if(type != "central" )
    {   if(     type == "sfHFp") (flv == 'b' || flv == 'c' ? type = "up"   : type = "central");
        else if(type == "sfHFm") (flv == 'b' || flv == 'c' ? type = "down" : type = "central");
        else if(type == "sfLp" ) (              flv == 'l' ? type = "up"   : type = "central");
        else if(type == "sfLm" ) (              flv == 'l' ? type = "down" : type = "central");
        else
        {   cout << "  INVALID TYPE SPECIFIED: " << type << " --> Using default type \"central\"." << endl;
            type = "central";
        }
    }

  // Check if scale factors are loaded and the OP requires a SF.
    if(csvSFLoaded_ && chmSFLoaded_ && opPt != "SVT" && opPt != "NoHF")
    {   if(opPt.substr(0,2) == "BC")  // If combining taggers, combine SFs
        {   string btag = "NoHF";
            string ctag = "NoHF";
            if(opPt[2] == 'L') btag = "CSVL";
            if(opPt[2] == 'M') btag = "CSVM";
            if(opPt[2] == 'T') btag = "CSVT";
            if(opPt[3] == 'L') ctag = "ChmL";
            if(opPt[3] == 'M') ctag = "ChmM";
            if(opPt[3] == 'T') ctag = "ChmT";
            sf = getJetSF(flv,btag,pt,eta,type) * getJetSF(flv,ctag,pt,eta,type);
        }
        else  // Get value from btag calibration reader directly
            sf = btagCalibReader_[opPt].eval_auto_bounds(type, flvMap_[flv], eta, pt);
    }

    return sf;
}
