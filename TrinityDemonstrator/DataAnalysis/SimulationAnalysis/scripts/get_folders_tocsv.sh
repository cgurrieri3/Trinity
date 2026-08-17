#!/bin/bash

# Output CSV file
output="folders.csv"
echo "Date,ID_ers" > "$output"

# Loop through all directories in the specified folder
for dir in /storage/osg-otte1/shared/TrinityDemonstrator/simdata/roots/diffuse/*/; do
    dirname=$(basename "$dir")
    
    # Match pattern YYYYMMDD_#########_ers
    if [[ "$dirname" =~ ^([0-9]{8})_([0-9]+_ers)$ ]]; then
        date="${BASH_REMATCH[1]}"
        id_ers="${BASH_REMATCH[2]}"
        echo "$date,$id_ers" >> "$output"
    fi
done

echo "Saved to $output"
