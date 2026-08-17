#!/bin/bash
# run_script.sh
# Change only this to run on a new machine
LocalPath="/home/sstepanoff"


echo "Opening container to compile code"
value=$1
apptainer exec --bind $LocalPath/TrinityDemonstrator:/mnt $LocalPath/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif make $value

chmod 775 * -R
