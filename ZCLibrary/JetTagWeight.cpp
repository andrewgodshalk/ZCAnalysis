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
: fn_eff_(""), fn_btag_sf_(""), effLoaded_(false), effPtMin_(-1), effPtMax_(-1), effEtaMin_(-1), effEtaMax_(-1), sfLoaded_(false),
  histHolder_(new TDirectory("JetTagWeight_histHolder_", "JetTagWeight Histogram Holder"))
{ // Set up maps from my labels to BTV's enums.
    opPtMap_ =  { {"NoHF", BTagEntry::OP_LOOSE}, //{"SVT",  BTagEntry::OP_LOOSE },
                  {"CSVL", BTagEntry::OP_LOOSE}, {"CSVM", BTagEntry::OP_MEDIUM},
                  {"CSVT", BTagEntry::OP_TIGHT}, {"CSVS", BTagEntry::OP_TIGHT }
                };
    flvMap_  =  { {'b',BTagEntry::FLAV_B}, {'c', BTagEntry::FLAV_C}, {'l', BTagEntry::FLAV_UDSG} };
    flvSet_  =  { {'b',"comb"}, {'c',"comb"}, {'l',"incl"} };
    flavor_  =  { 'b', 'c', 'l' };
    opPt_    =  { "NoHF", "CSVL", "CSVM", "CSVT", "CSVS" };
    svTypes_ =  { "noSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
    // svTypes_ =  { "noSV", "oldSV", "pfSV", "pfISV", "qcSV", "cISV", "cISVf", "cISVp"};
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
        // TString plot_name = TString::Format("h_%cJetTagEff_%s%s", f, op.c_str(), sv.c_str());
        jetTagEff_[f][op][sv] = (TH2F*) effFile->Get(plot_name.Data())->Clone((plot_name+"_clone").Data());
        // cout << "    " << jetTagEff_[f][op][sv]->GetName() << endl;
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
    cout << TString::Format("    (Loaded dimensions: {pt}x{eta} = {%f,%f}x{%f,%f})", effPtMin_, effPtMax_, effEtaMin_, effEtaMax_) << endl;
    cout << "  Complete!" << endl;
    return true;
}


bool JetTagWeight::setSFFile( const string& fn)
{
    fn_btag_sf_ = fn;
    string tagger = "CSVv2";
    btagCalib_ = BTagCalibration(tagger, fn_btag_sf_);
    for( string& op : opPt_)
    {   btagCalibReader_[op] = BTagCalibrationReader(opPtMap_[op], "central", {"up","down"});
        for( char& f : flavor_)
            btagCalibReader_[op].load(btagCalib_, flvMap_[f], flvSet_[f]);
    }
    sfLoaded_ = true;
    return true;
}


float JetTagWeight::getJetEff(char flv, string opPt, string svType, float pt, float eta)
{
    // cout << TString::Format("JetTagWeight::getJetEff(%c, %s, %s, %f, %f)", flv, opPt.c_str(), svType.c_str(), pt, eta) << endl;
    float eff = 1.0;
    if(effLoaded_)
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
    //cout << "  Looking for jet sf: flv=" << flv << ", opPt=" << opPt << ", pt=" << pt << ", eta=" << eta << endl;
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

  // Get value from btag calibration reader
    if(sfLoaded_ && opPt != "SVT" && opPt != "NoHF")
    {   sf = btagCalibReader_[opPt].eval_auto_bounds(type, flvMap_[flv], eta, pt);
        //cout << "    ...from file: " << sf << endl;
    }

    return sf;
}
