#!/bin/bash
# run_script.sh

# Change only this to run on a new machine
LocalPath="/data/TrinityLabComputer"  

DATE=$1
FILENAME=$2
echo "Running for $DATE"
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/event_cleaning/ClusterCleaning/new_cleaning/EventCleaning $DATE y $FILENAME    
