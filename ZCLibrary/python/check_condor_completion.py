#
#  check_condor_completion.py
#
#  created:  2017-03-02 godshalk
#  modified: 2017-03-02 godshalk
#
#  Combines plot files by ntuple numbering.
#
#  Takes an input of the condor directory after submission and the dsList that
#  was used in submission. Looks at files in directory for a result from each
#  dsList listing. Dumps missing listings into a new file of the same with
#  RESUBMIT label at the end.
# 
#  Written to be called from VHbbAnalysis/Heppy/test

import os, sys, re


# === GLOBALS =================================================================
RESUBMIT_SUFFIX = '_RESUBMIT'
CONDOR_OUTPUT_PREFIX = 'zc_out_'

# === FUNCTION ============================================================
def check_condor_completion( condor_dir, ds_list ) :
  # Initial output
    print '\n===== check_condor_completion("{}, {}")'.format(condor_dir, ds_list)

  # Handle input
    # Remove slashes from end of directory name if present
    while condor_dir[-1] == '/' : condor_dir = condor_dir[:-1]
  # Check if both inputs exist.
    if not os.path.exists(condor_dir) :
        print '  ERROR: Condor directory does not exist:', condor_dir
        return "INVALID INPUT DIRECTORY"
    if not os.path.exists(ds_list) :
        print '  ERROR: Dataset list does not exist:', ds_list
        return "INVALID INPUT DIRECTORY"
  # Split fn_ds_list into its pieces
    #fn_ds_list = ds_list.split('/')[-1]
    #dir_ds_list = ds_list[:-1*len(fn_ds_list)]
    # print ds_list, '->', dir_ds_list, fn_ds_list

  # Read in list of root files in condor directory.
    os.system("ls -1 {} | grep {} | grep .root > tmp.txt".format(condor_dir, CONDOR_OUTPUT_PREFIX))
    # Open file and get lines.
    f_ls_completed_files = open("tmp.txt")
    #print f_root_file_list.read()
    completed_file_list = f_ls_completed_files.readlines()
    f_ls_completed_files.close()
    print "  Completed File Count in {}:".format(condor_dir), len(completed_file_list)
    #print completed_file_list

  # Read in ds_list
    f_ds_list = open(ds_list) 
    dataset_list = f_ds_list.readlines()
    f_ds_list.close()
    print "  Dataset listings in {}:".format(ds_list), len(dataset_list)
    #print dataset_list

  # For each listing in ds_list...
    ds_resubmit_list = []
    for ds_listing in dataset_list : 
      # Get the output name from the ntuple label (first listing on ds_list line) 
        ds_label = CONDOR_OUTPUT_PREFIX + ds_listing.split(" ")[0] + ".root\n"
      # If there is no output file for this listing:
        if ds_label not in completed_file_list :
          # Add ds line to resubmit list.
            ds_resubmit_list.append(ds_listing)

  # Save resubmit list to new file with RESUBMIT_SUFFIX in same place as input dsList.
    fn_resubmit_file = ds_list[:-4] + RESUBMIT_SUFFIX + ds_list[-4:]
    f_resubmit_file = open(fn_resubmit_file, 'w')
    for ds_listing in ds_resubmit_list : f_resubmit_file.write(ds_listing)
    f_resubmit_file.close()
    print "  Resubmit listings in {}".format(fn_resubmit_file), len(ds_resubmit_list)
    #print ds_resubmit_list

  # Final output
    print '\n====='


# === MAIN DEF ============================================================
if __name__ == "__main__" :
    if len(sys.argv) != 3 : print "ERROR: Please specify a directory and a dataset list."
    else : 
        check_condor_completion(sys.argv[1], sys.argv[2])
