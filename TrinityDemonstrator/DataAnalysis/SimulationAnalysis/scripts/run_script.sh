#!/bin/bash
# run_script.sh
# Change only this to run on a new machine
LocalPath="/home/sstepanoff"

# Variables date and files
DATE=$1 # YYYYMMDD
SIMRUN=$2 # *.root or n
# -------------------------------------------------
echo "Running for $DATE"

apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/SimulationAnalysis/scripts/SimRootFiles $DATE $SIMRUN y   
