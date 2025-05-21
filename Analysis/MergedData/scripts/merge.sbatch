#!/bin/bash

##header

#SBATCH -A hive-aotte6
#SBATCH -p hive
##SBATCH -J MerF"$1"
#SBATCH -n 4
#SBATCH --mem=15G
#SBATCH --output=/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/shared/Trinity/DataAnalysis/DataCalibration/MergedData/Output/joboutput.out
#SBATCH --error=/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/shared/Trinity/DataAnalysis/DataCalibration/MergedData/Output/joberror.err
#SBATCH --open-mode=append

##bash script to run

## Check if an argument (date) is provided
if [ -z "$1" ]; then
    echo "Usage: sbatch submit.sbatch YYYYMMDD"
    exit 1
fi

/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/MergedData/FileMerge $1

sleep 10

/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/MergedData/AddCalibData $1

