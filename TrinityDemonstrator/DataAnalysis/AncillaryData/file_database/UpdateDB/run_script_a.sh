#!/bin/bash
# run_script.sh

apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/python3_10.sif python3 /mnt/DataAnalysis/AncillaryData/file_database/UpdateDB/userUpdateDatabase.py