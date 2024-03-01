#!/bin/bash

DATEVAR=$(/bin/date +%Y%m%d)

/usr/bin/python3 /home/mpotts32/cams/upload_vid.py --file="/home/mpotts32/cams/VIDS/OUT_$DATEVAR.mp4" --title="OUT $DATEVAR" --privacyStatus="public" --playlist OUTDOOR
/usr/bin/python3 /home/mpotts32/cams/upload_vid.py --file="/home/mpotts32/cams/VIDS/IN_$DATEVAR.mp4" --title="IN $DATEVAR" --privacyStatus="public" --playlist INDOOR
/usr/bin/python3 /home/mpotts32/cams/upload_vid.py --file="/home/mpotts32/cams/VIDS/Horizon_$DATEVAR.mp4" --title="HORIZON $DATEVAR" --privacyStatus="public" --playlist HORIZON

file_count=$(ls -1A /home/mpotts32/cams/VIDS/ | wc -l)

# Check if there are 16 files
if [[ "$file_count" -eq 16 ]]; then
	# Sort the files by modification time in ascending order
    	files=($(ls -t1 /home/mpotts32/cams/VIDS/))

	# Delete the oldest two files
    	rm "${files[@]: -2}"
