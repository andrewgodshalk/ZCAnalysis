#
#  combine_plot_files.py
#
#  created:  2017-03-01 godshalk
#  modified: 2017-03-01 godshalk
#
#  Combines plot files by ntuple numbering.
#
#  Takes an input of the condor directory after the Analyzer has successfully
#  run over all ntuples. Combines plot files (zc_out_*.root) with a XXofYY 
#  suffix into a single plot file. Stores new files in a directory
#  <OUTPUT_PATH>/raw_cp_<CONDOR_SUFFIX>,
# 
#  Written to be called from VHbbAnalysis/Heppy/test

import os, sys, re


# === GLOBALS =================================================================
OUTPUT_PATH = "raw_cp/"
PLOT_FILE_PREFIX = 'zc_out_'

# === FUNCTION ============================================================
def combine_plot_files( plot_file_dir ) :
  # Initial output
    print '===== combine_plot_files("{}")'.format(plot_file_dir)

  # Handle input
    # Remove slashes from end of directory name if present
    while plot_file_dir[-1] == '/' : plot_file_dir = plot_file_dir[:-1]
    # Check that the directory actually exists
    if not os.path.exists(plot_file_dir) :
        print '  ERROR: Input directory does not exist:', plot_file_dir
        return "INVALID INPUT DIRECTORY"

  # Set up output directory
    # Check to see if plot file directory meets naming standards
    # (should be something like "condor_<SUFFIX>") 
    output_dir_name = OUTPUT_PATH + "raw_cp_"
    suffix = plot_file_dir
    if plot_file_dir[:7] != "condor_" :
        print '  WARNING: Input directory does not match naming convention ("condor_<SUFFIX>"). Using full directory name as suffix.'
    else : suffix = plot_file_dir[7:]
    output_dir_name += suffix

    # Check if the output directory already exists. Add to suffix if so, then create.
    extra_sfx = ""
    sfx_num = 0
    while os.path.exists(output_dir_name+extra_sfx):
        sfx_num += 1
        extra_sfx = "_{}".format(sfx_num)
    output_dir_name += extra_sfx + "/"
    print '  Output directory: ', output_dir_name
    os.makedirs(output_dir_name)
    
  # Get list of plot files from input directory. Plot files have format zc_out_<ntuple_identifier>[_XXofYY].root
    # Create a list of root files. Store in condor directory.
    fn_root_file_list = plot_file_dir+"/plot_file_list.txt"
    if os.path.isfile(fn_root_file_list) :
        print "  Plot file list already exists. Recreating."
        os.system("rm {}".format(fn_root_file_list))
    os.system("ls -1 {} | grep zc_out_ | grep .root > {}".format(plot_file_dir, fn_root_file_list))
    # Open file and get lines.
    f_root_file_list = open(fn_root_file_list)
    #print f_root_file_list.read()
    file_list = f_root_file_list.readlines()
    f_root_file_list.close()

  # Sort list into lists, grouping files that are part of a set.
  # Each set has two parts: A string of the final combined file name, and a list of files that will be combined into that file.
    file_sets = {}
    file_set_complete = {}   # Bool for each set. Initially false. Set to true if set gets completed.
    numbered_fn_pattern = r'^'+PLOT_FILE_PREFIX+r'(?P<pfx>.*)_(?P<num>\d{1,2})of(?P<ttl>\d{1,2}).root$'
    single_fn_pattern   = r'^'+PLOT_FILE_PREFIX+r'(?P<pfx>.*).root$'
    for fn in file_list : 
        fn = fn.replace('\n','')   # Get rid of that pesky return character.

      # Check if the the file name is part of a number set.
        fn_match = re.match(numbered_fn_pattern, fn)
        if fn_match :
            pfx = fn_match.group('pfx')
            num = fn_match.group('num')
            ttl = fn_match.group('ttl')
          # If the set does not yet exist, create it and its completion.
            if pfx not in file_sets :
                file_sets[pfx] = [fn]
                file_set_complete[pfx] = False
          # Otherwise, add to the current set.
            else: file_sets[pfx].append(fn)
          # If the current file is last in the set and there are enough files in the list, mark set as complete.
            if num == ttl and len(file_sets[pfx]) == int(ttl) : file_set_complete[pfx] = True
            #print fn, '->', pfx, num, ttl, len(file_sets[pfx])

      # If file is singular, add to the file set list by its lonesome.
        else :
            fn_match = re.match(single_fn_pattern, fn)
            if fn_match : 
                pfx = fn_match.group('pfx') 
                #print fn, '->', pfx
                file_sets[pfx] = [fn]
            else : print "  WARNING: Invalid file found in file list:", fn

  # Check that all the lists are complete
    for fset, complete in file_set_complete.items() : 
        if not complete : print "  WARNING: File set {} missing a file.".format(fset)
    #print file_set_complete, '\n'
    #print file_sets

  # For each list, combine and move the file to the output directory.
    for set_name, files in file_sets.items() :   # Just copy the file if there its not part of a list.
        if len(files) == 1 : os.system( "cp {}/{} {}{}.root".format(plot_file_dir, files[0], output_dir_name, set_name) ) 
        else : 
            hadd_cmd = "hadd {}{}.root".format(output_dir_name, set_name)
            for fn in files : hadd_cmd += " " + plot_file_dir + "/" + fn
            os.system(hadd_cmd)
            #print hadd_cmd

  # Final output
    print '\n====='


# === MAIN DEF ============================================================
if __name__ == "__main__" :
    if len(sys.argv) != 2 : print "ERROR: Please specify a directory"
    else : 
        combine_plot_files(sys.argv[1])
