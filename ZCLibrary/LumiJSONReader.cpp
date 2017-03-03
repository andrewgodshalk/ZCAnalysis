// LumiJSONReader.cpp
//
//
// 2017-01-19 godshalk
//

#include <cctype>
#include <fstream>
#include "LumiJSONReader.h"

using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::map;
using std::string;
using std::ifstream;
using std::getline;
using std::stoi;
using std::isdigit;

typedef string::const_iterator str_iter;

LumiJSONReader::LumiJSONReader(string fn_json)
  : json_file(fn_json), inputSucessful(false)
{}

void LumiJSONReader::setFile(string fn)
{
    json_file = fn;
  // Read in file contents
    ifstream input_file;
    input_file.open(json_file);
    if(!input_file.is_open()) {cout << " ERROR: Unable to open json file: " << json_file << endl; return;}
    string rawJSON = "";
    string line;
    while(getline(input_file,line)) rawJSON += line;
    //cout << rawJSON << endl;

  // Pull information from contents.
    str_iter sIter = rawJSON.begin();
    str_iter sEnd  = rawJSON.end();
    if( *(sIter) != '{' || *(sEnd-1) != '}' ) {cout << " ERROR: File format incorrect. Needs to be bound by brackets {}." << endl; return;}
  // Find next run number: "######": [
    while( ++sIter != sEnd )
        if(*sIter == '"') extractRunAndLumis(sIter);

    cout << " TEST: LUMIREADER IS A GO WITH JSON FILE NAME: " << json_file << '\n' << endl;
    inputSucessful = true;
}

bool LumiJSONReader::isInJSON(int nRun, int nLumi)
{
    //cout << " TEST: LUMIREADER IS CHECKING " << nRun << "," << nLumi << endl;
    if(runsAndLumis.find(nRun) == runsAndLumis.end()) return false;  // If the run number isn't in the list, return false.
    for( auto& min_max : runsAndLumis[nRun] )  // For each range for this run number...
        if( min_max.first <= nLumi && nLumi <= min_max.second ) return true;  // If the lumi number is in range, return true.
    return false;
}

void LumiJSONReader::extractRunAndLumis(str_iter& sIter)
{ // Starts on the " before the run number.
    // Find next run number: "######": [[],[]],
    if(*(sIter) != '"') {cout << " ERROR: extractRunAndLumi called on Iter that doesn't begin with \"." << endl; return;}
    findNextDigit(sIter);
    int runNumber = getNumberFromString(sIter, '"');
    runsAndLumis[runNumber] = vector<std::pair<int,int>>();
    while(*sIter != '[') sIter++;
    addLumiRangesToList(runNumber, sIter);
}

void LumiJSONReader::addLumiRangesToList(int runNumber, str_iter& sIter)
{ // Iter begins at first [ of list of lumis
    sIter++;   // Move past the first [
    while(*sIter!=']') // While the list of ranges has not ended...
    {   while(*sIter!='[') sIter++; // Find the beginning of the next list.
        addLumiRangeToList(runNumber, sIter);
    }
}

void LumiJSONReader::addLumiRangeToList(int runNumber, str_iter& sIter)
{ // Iter starts at first '[' of range.
     sIter++;  // Move to the first number (should be right after '[')
     int rangeBegin = getNumberFromString(sIter, ',');
     findNextDigit(sIter);  // Find the second number in the range.
     int rangeEnd   = getNumberFromString(sIter, ']');
     runsAndLumis[runNumber].push_back({rangeBegin, rangeEnd});
}


int LumiJSONReader::getNumberFromString(str_iter& sIter, char delim)
{ // Returns sIter at character AFTER delim.
    str_iter runStrBegin = sIter;
    while (*sIter!=delim) sIter++;
    sIter++;
    return stoi(string(runStrBegin, sIter));
}
