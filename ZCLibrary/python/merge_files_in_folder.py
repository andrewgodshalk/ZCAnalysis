#!/bin/python

# Created: 2017-03-02 godshalk
#
# Combines all root files in the given folder into a zc_rawcp_<>.root file.

import sys, os

def merge_file_in_folder(folder) :
    print "==="
  # Handle input
    # Remove slashes from end of folder name.
    while folder[-1] == '/' : folder = folder[:-1]
    # Check that the directory actually exists
    if not os.path.exists(folder) :
        print '  ERROR: Input directory does not exist:', folder
        return "INVALID INPUT DIRECTORY"

  # Create file name from folder name. Remove if it already exists.
    fn_combo = folder + '/zc_rawcp_' + folder.split('/')[-1]+ '.root'
    print "  Creating", fn_combo
    if os.path.isfile(fn_combo) :
        print "  - Combined plot file already exists. Recreating."
        os.system("rm {}".format(fn_combo))

  # Create new combo file from all files in folder.
    os.system("hadd {} {}/*.root".format(fn_combo, folder))

# === MAIN DEF ============================================================
if __name__ == "__main__" :
    if len(sys.argv) != 2 : print "ERROR: Please specify a directory"
    else :
        print sys.argv[1]
        merge_file_in_folder(sys.argv[1])
