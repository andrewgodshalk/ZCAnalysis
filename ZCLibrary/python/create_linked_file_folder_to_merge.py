#
#  create_linked_file_folder_to_merge.py
#
#  created:  2017-03-17 godshalk
#  modified: 2017-03-17 godshalk
#
#  Takes two folders: A first folder with files, for which links are created in the second folder.
#  Hardcoded to take a folder
#  
#  Written to be called from VHbbAnalysis/Heppy/test

import sys, os

# === GLOBALS =================================================================
OUTPUT_PATH = "raw_cp_merges/"
TEMP_LIST_FILE = "tmp.txt"

# === FUNCTION ================================================================

def create_linked_file_folder_to_merge(folder_with_files, folder_for_links="", sfx_to_copy=[]) :
  # sfx_to_copy - if not empty, only make links to files with this suffix.

  # Handle input
    # Remove slashes from end of directory name if present
    while folder_with_files[-1] == '/' : folder_with_files = folder_with_files[:-1]
    # Check that the directory actually exists
    if not os.path.exists(folder_with_files) :
        print '  ERROR: Input directory does not exist:', plot_file_dir
        return "INVALID INPUT DIRECTORY"

  # If output directory specified, generate a name using default output path.
    if folder_for_links == "" :
        folder_for_links = folder_with_files.split('/')[-1]
        if folder_for_links.startswith("raw_cp_") : folder_for_links = folder_for_links[7:]
        folder_for_links = OUTPUT_PATH+folder_for_links
    else :
        while folder_for_links[-1] == '/' : folder_for_links = folder_for_links[:-1]

  # If the output directory doesn't exist, create it.
    if not os.path.exists(folder_for_links) :
        print " Creating output directory", folder_for_links
        os.system("mkdir {}".format(folder_for_links))

  # Get list of files from input directory.
    if os.path.exists(TEMP_LIST_FILE) : os.system("rm {}".format(TEMP_LIST_FILE))
    input_list_command = "ls -1 {}".format(folder_with_files)
    for sfx in sfx_to_copy : input_list_command += " | grep {}".format(sfx)
    input_list_command += " > {}".format(TEMP_LIST_FILE)
    os.system(input_list_command)
    f_file_list = open(TEMP_LIST_FILE)
    file_list = f_file_list.readlines()
    f_file_list.close()

  # Make link for each input file in the output directory.
    for filename in file_list :
        filename = filename.replace('\n','')
        os.system("ln -s ../../{0}/{2} {1}/{2}".format(folder_with_files, folder_for_links, filename))
        #print "ln -s {0}/{2} {1}/{2}".format(folder_with_files, folder_for_links, filename)


# === MAIN DEF ================================================================
if __name__ == "__main__" :
    if len(sys.argv) < 2 : print "ERROR: Please specify a directory with files to link."
    else :
        output_dir = "" if len(sys.argv)<3 else sys.argv[2]
        create_linked_file_folder_to_merge(sys.argv[1], output_dir, [".root"])
