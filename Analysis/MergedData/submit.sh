#!/bin/bash

# Check for date argument
if [ -z "$1" ]; then
  echo "Usage: $0 YYYYMMDD"
  exit 1
fi

DATE=$1
TARGET_DIR="/storage/osg-otte1/shared/TrinityDemonstrator/Data/$DATE/RawDataMerged/"
SUBMIT_TEMPLATE="condense_SM.submit"

# Loop through files matching the date
for FILE in "$TARGET_DIR"*; do
  [ -e "$FILE" ] || continue  # skip if no match

  BASENAME=$(basename "$FILE")
  echo "Submitting job for file: $FILE (basename: $BASENAME)"
  condor_submit Date=$DATE Filename=$BASENAME "$SUBMIT_TEMPLATE"
done
echo "Submitted all jobs for date $DATE"

mkdir -p /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/Output/$DATE
echo "Watcher started..."
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt \
  /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif \
  python3 /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/MergedData/watcher.py \
  -i /mnt/DataAnalysis/MergedData/ \
  -o /mnt/DataAnalysis/MergedData/Output/ \
  -l /mnt/DataAnalysis/MergedData/.logs/watcher.log \
  -t 120
echo "Watcher finished."