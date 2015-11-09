#ifndef GUARD_ConfigReader_h
#define GUARD_ConfigReader_h
/*-----------------------------------------------------------------------------
 *  ConfigReader class
 *
 *  Created by Andrew Godshalk, 2015-10-28
 *
 *  Virtual class with basic functions used in inputing an ini-format
 *  configuration file.
 *
 *
-----------------------------------------------------------------------------*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <TString.h>

using std::cout; using std::endl;

class ConfigReader {
  public:
    virtual ~ConfigReader(){}
    ConfigReader(TString fnc): fn_config(fnc)
    {
      // Open and read in config file
        boost::property_tree::ini_parser::read_ini(fn_config.Data(), pt);
    }

  protected: 
    TString fn_config;
      // Location of the input configuration file.
    boost::property_tree::ptree pt;
      // Property tree read from file.

    template <typename T>
      void getListFromString(std::string& str, std::vector<T>& list)
    { // Simple function that extracts numbers from a string.
      // Feeds string into a stringstream and, while there is still something to
      //   read out, ouputs the entry from the stream into an variable and
      //   pushes the variable into the output vector.
        list.clear();
        std::stringstream strm(str);
        while(true) {
            T n;
            strm >> n;
            if(!strm) break;
            list.push_back(n);
        }
    }
};



#endif
