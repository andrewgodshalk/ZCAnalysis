/*------------------------------------------------------------------------------
JetTagWeight.cpp

 Created : 2017-02-03  godshalk
 Modified: 2017-02-03  godshalk

------------------------------------------------------------------------------*/

#include "JetTagWeight.h"   // Class header
#include <iostream>
#include <string>
#include <TFile.h>

using std::string;
using std::cout; using std::endl;

JetTagWeight::JetTagWeight()
: fn_eff_(""), fn_btag_sf_(""), effLoaded_(false), sfLoaded_(false)
{ // Set up maps from my labels to BTV's enums.
    opPtMap_ =  { {"NoHF", BTagEntry::OP_LOOSE}, {"SVT",  BTagEntry::OP_LOOSE },
                  {"CSVL", BTagEntry::OP_LOOSE}, {"CSVM", BTagEntry::OP_MEDIUM},
                  {"CSVT", BTagEntry::OP_TIGHT}, {"CSVS", BTagEntry::OP_TIGHT }
                };
    flvMap_  =  { {'b',BTagEntry::FLAV_B}, {'c', BTagEntry::FLAV_C}, {'l', BTagEntry::FLAV_UDSG} };
    flvSet_  =  { {'b',"comb"}, {'c',"comb"}, {'l',"incl"} };
    flavor_  =  { 'b', 'c', 'l' };
    opPt_    =  { "NoHF", "SVT", "CSVL", "CSVM", "CSVT", "CSVS" };
}


bool JetTagWeight::setEffFile(const string& fn)
{
    cout << "  JetTagWeight: << Loading Efficiency plots from file: " << fn << "...";
    fn_eff_ = fn;
    TFile* effFile = TFile::Open(fn_eff_.c_str());
    for( char& f : flavor_)
        for( string& op : opPt_)
        {   TString plot_name = TString::Format("h_%cJetTagEff_%s", f, op.c_str());
            jetTagEff_[f][op] = (TH2F*) effFile->Get(plot_name.Data())->Clone((plot_name+"_clone").Data());
            //cout << "    " << jetTagEff_[f][op]->GetName() << endl;
        }
    //cout << jetTagEff_['l']["CSVT"]->GetName() << endl;
    //effFile->Close();
    //cout << jetTagEff_['l']["CSVT"]->GetName() << endl;
    effLoaded_ = true;
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


float JetTagWeight::getJetEff(char flv, string opPt, float pt, float eta)
{
    //cout << "  Looking for jet eff: flv=" << flv << ", opPt=" << opPt << ", pt=" << pt << ", eta=" << eta << endl;
    float eff = 1.0;
    if(effLoaded_)
    {   int globalBinNum = jetTagEff_[flv][opPt]->FindBin(pt, eta);
        eff = jetTagEff_[flv][opPt]->GetBinContent(globalBinNum);
        //cout << "    ...from file: " << eff << endl;
    }
    return eff;
}

float JetTagWeight::getJetSF(char flv, string opPt, float pt, float eta, string type)
{
    //cout << "  Looking for jet sf: flv=" << flv << ", opPt=" << opPt << ", pt=" << pt << ", eta=" << eta << endl;
    float sf = 1.0;
    if(sfLoaded_ && opPt != "SVT" && opPt != "NoHF")
    {
        sf = btagCalibReader_[opPt].eval_auto_bounds(type, flvMap_[flv], eta, pt);
        //cout << "    ...from file: " << sf << endl;
    }

    return sf;
}
