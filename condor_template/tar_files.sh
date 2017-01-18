#!/bin/bash

# Give input config files generic names
cp $1 zcAC.ini
cp $2 zcNPC.ini

# Compress files
tar czv --file=zcfiles.tgz --files-from=files_to_tar.txt

# Clean up previous files.
rm zcAC.ini zcNPC.ini
