#!/bin/bash
# Source directory containing .log files
source_path="/home/trinity/Programs/Trinity/control_software/fcutils/test/LOGS/"
echo "$source_path"
# Destination directory
destination="/home/trinity/Documents/Data/$(date +'%m%d%Y')/LOGS"
echo "$destination"
# Create a LOGS directory in the newest directory if it doesn't exist
mkdir -p "$destination"
# Move .log files from path1 to the LOGS directory in the newest directory
#mv "$source_path/Client_LVPS.log" "$destination/LOGS/"
mv "$source_path/rc.log" "$destination"


echo "Moved .log files from $source_path to $destination_dir/LOGS"
# Move all the .log files from path1 and put them in \Documents\Data\ determin the most recent folder (newest) and 
# and copy all the files to mv path1/*.log \Documents\Data\newest\LOGS
