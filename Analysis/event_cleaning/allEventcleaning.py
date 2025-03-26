#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import datetime as dt
import numpy as np
import os
import subprocess
import time


def removeDates(f):
    removeList = ['0104',"0105","0109","0110","0111","0122","0130","0131","0201","0202","0203",'0206', '0207',
        '0208', '0209', '0210', '0211', '0212', '0213', '0214', '0215', '0216', '0217', '0218', '0219', '0220', '0221',
        '0222', '0223', '0224', '0225', '0226', '0227', '0228', '0229', '0301', '0302', '0303', '0304', '0305', '0306',
        '0307', '0308', '0309', '0313', '0314', '0315', '0316', '0317', '0320', '0321', '0322', '0323', '0324', '0325',
        '0326', '0327', '0328', '0329', '0330', '0331', '0401', '0402', '0403', '0404', '0405', '0406', '0407', '0408',
        '0412', '0418', '0419', '0420', '0421', '0422', '0423', '0424', '0425', '0426', '0427', '0504', '0505', '0506', '0508',
        '0511', '0514', '0515', '0517', '0519', '0520', '0521', '0522', '0523', '0524', '0525', '0526', '0527', '0528', '0529',
        '0530', '0601', '0602', '0603', '0604', '0605', '0606', '0607', '0608', '0612', '0613', '0614', '0615', '0616',
        '0618', '0619', '0620', '0621', '0622', '0623', '0624', '0625', '0626', '0627', '0628', '0629', '0630', '0701',
        '0702', '0703', '0704', '0705', '0706', '0707', '0708', '0709', '0711', '0712', '0713', '0714', '0715', '0716',
        '0717', '0719', '0720', '0721', '0722', '0723', '0724', '0725', '0726', '0727', '0728', '0729','0803', '0810', '0811',
        '0812', '0813', '0817', '0818', '0819', '0820', '0821', '0822', '0823', '0824', '0905', '0910', '0914', '0915',
        '0916', '0917', '0918', '0919', '0920', '0921', '0922', '0923', '0928', '1002', '1003', '1004', '1005', '1006',
        '1007', '1008', '1009', '1010', '1011', '1012', '1013', '1014', '1015', '1016', '1017', '1018', '1019', '1020',
        '1021', '1029', '1030']
    for rm in removeList:

        if f'2024{rm}' in f:
            f.remove(f'2024{rm}')
        # else:
        #     print(f'Item "2024{rm}" not found in list.')

    #print(f)
    return f

def main():
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
    # folderArray = [item for item in folderArray if "202405" not in item]
    # folderArray = [item for item in folderArray if "202406" not in item]
    # folderArray = [item for item in folderArray if "202407" not in item]
    # folderArray = [item for item in folderArray if "202408" not in item]
    # folderArray = [item for item in folderArray if "202409" not in item]
    folderArray = [item for item in folderArray if "202410" not in item]
    # folderArray = [item for item in folderArray if "202411" not in item]
    #print(folderArray)
    folderArray=removeDates(folderArray)

    folderArray=sorted(folderArray)
    for folder in folderArray:
        print(f"Calibration of {folder}")
        # Run ./app with arguments
        try:
            os.chdir('/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/FlatFieldCalibration/')
            result =subprocess.run(["./FlatFieldCalibration", folder],capture_output=True, text=True)
            # Print stdout and stderr
            print("stdout:", result.stdout)
            print("stderr:", result.stderr)
            time.sleep(3)
        except:
            print("already made calibration file")

        print(f"Event Cleaning of {folder}")
        os.chdir('/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/ClusterCleaning/')
        subprocess.run(["./ClusterCleaning", folder],capture_output=True, text=True)
        print("stdout:", result.stdout)
        print("stderr:", result.stderr)
        time.sleep(3)

if __name__ == "__main__":
    main()