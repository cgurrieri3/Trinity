#!/bin/bash

##header

#SBATCH -A hive-aotte6
#SBATCH -p hive
##SBATCH -J EC"$1"
#SBATCH -n 4
#SBATCH --mem=15G
#SBATCH --output=/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/.slurmOutputs/R-%x.%j.out
#SBATCH --error=/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/.slurmOutputs/R-%x.%j.err


##bash script to run

## Check if an argument (date) is provided
if [ -z "$1" ]; then
    echo "Usage: sbatch submit.sbatch YYYYMMDD"
    exit 1
fi

/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/FlatFieldCalibration/FlatFieldCalibration $1

/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/ClusterCleaning/ClusterCleaning $1