// LumiJSONReader
// 
// Quick class put together to read JSON file of good lumis.
// Class is then able to tell whether a given run and lumi are in a "good" section.
// 
//
// 2017-01-19
//
//

#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class LumiJSONReader 
{
  public:
    LumiJSONReader(std::string fn_json="");
    ~LumiJSONReader(){};
    void setFile(std::string);
    bool isInJSON(int, int);  // Function that takes a run and lumi number and checks if they fall in good lumi sections.
    bool isValid(){return inputSucessful;}

  private:
    std::string json_file;	// Location of JSON file.
    std::map<int, std::vector<std::pair<int,int> > > runsAndLumis;
    bool inputSucessful;
    void extractRunAndLumis(std::string::const_iterator&);
    void addLumiRangesToList(int, std::string::const_iterator&);
    void addLumiRangeToList( int, std::string::const_iterator&);
    int getNumberFromString(std::string::const_iterator&, char delim ='"');
    void findNextDigit(std::string::const_iterator& i){ while(!std::isdigit(*i)) i++; }
};
