#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import datetime as dt
import numpy as np
import os
import subprocess
import time
from concurrent.futures import ThreadPoolExecutor

def sortFolder():
    datapath = '/storage/hive/project/phy-otte/shared/Trinity/Data/'
    os.chdir(datapath)
    folderArray=os.listdir()

    # Remove entries containing "_"
    folderArray = [item for item in folderArray if "_" not in item]
    folderArray = [item for item in folderArray if "-" not in item]
    folderArray = [item for item in folderArray if "s" not in item]
    folderArray = [item for item in folderArray if "2023" not in item]
    folderArray = [item for item in folderArray if "202401" not in item]
    folderArray = [item for item in folderArray if "202402" not in item]
    folderArray = [item for item in folderArray if "202403" not in item]
    folderArray = [item for item in folderArray if "202404" not in item]
    folderArray = [item for item in folderArray if "202405" not in item]
    folderArray = [item for item in folderArray if "202406" not in item]
    folderArray = [item for item in folderArray if "202407" not in item]
    folderArray = [item for item in folderArray if "202408" not in item]
    folderArray = [item for item in folderArray if "202409" not in item]
    folderArray = [item for item in folderArray if "202410" not in item]
    folderArray = [item for item in folderArray if "202411" not in item]
    folderArray = [item for item in folderArray if "202412" not in item]
    folderArray = [item for item in folderArray if "202501" not in item]
    folderArray = [item for item in folderArray if "202502" not in item]
    # folderArray = [item for item in folderArray if "202503" not in item]
    #print(folderArray)
    # folderArray=removeDates(folderArray)

    folderArray=sorted(folderArray)
    return folderArray

def runFileMerge(folder):
        print(f" Calibration night and Event Cleaning night: {folder}")
        # Run ./app with arguments

        os.chdir('/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/')
        # result =subprocess.run(["./FileMerge", folder],capture_output=True, text=True)
        cmd = ["sbatch", "-J", f"EC{folder[2:]}", "merge.sbatch", folder]

        # Run the command
        subprocess.run(cmd, check=True)
        
        time.sleep(3)


def main():
    folderArray = sortFolder()  # Ensure folders are sorted
    max_workers = min(4, len(folderArray))  # Adjust the number of parallel processes
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        executor.map(runFileMerge, folderArray)

if __name__ == "__main__":
    main()
