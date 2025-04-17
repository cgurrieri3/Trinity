R__LOAD_LIBRARY(libExACT.so)
#include <iostream>
#include <vector>
#include <string>
#include <sys/stat.h>  // for mkdir
#include <unistd.h>    // for access()

int AddSiPMInfoToEvents(std::string inputFileName) {
    // Get filename only (strip path from input)
    std::string baseFileName = inputFileName.substr(inputFileName.find_last_of("/\\") + 1);
    
    // Extract input directory
    std::string inputDir = inputFileName.substr(0, inputFileName.find_last_of("/\\"));
    
    // Replace "Merged" with "Calibrated" in filename
    size_t pos = baseFileName.find("Merged");
    if (pos != std::string::npos) {
        baseFileName.replace(pos, 6, "Calibrated");
    } else {
        std::cerr << "Input file name must contain 'Merged' to construct the output file name." << std::endl;
        return 1;
    }
        
    // Construct output directory relative to input directory
    std::string outputDir = inputDir + "/../RawDataCalibrated";
    if (access(outputDir.c_str(), F_OK) == -1) {
        mkdir(outputDir.c_str(), 0775);
    }

    // Set output file path
    std::string outputFileName = outputDir + "/" + baseFileName;

    // Open input file in READ mode
    TFile* inputFile = new TFile(inputFileName.c_str(), "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Failed to open input file!" << std::endl;
        return 1;
    }

    // Create output file
    TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Failed to create output file!" << std::endl;
        return 1;
    }
    
       
    
    ICalibration* calib = new ICalibration(inputFileName);  

    const double GAIN_REF = 22.1; // factor at 25 C 
    //obtained by multiplying (1 - 37.97V/5.7V * 0.001 * (25C - 12.7C)) with 24.1 DC/PE measured at 12.7C 
    //since we operate at 5V overvoltage at 25C and the average breakdown voltage is 38.44V at 25C, 
    //which corresponds to overvoltage 5.7V = 5V + 38.44V*0.001(25C - 12.7C) and average breakdown voltage 37.97V  = 38.44V * [1-0.001* (25C - 12.7C)] @ 12.7C

    std::vector<std::string> treeNames = {"HLED", "BiFocal", "Forced", "Test"};

    for (const auto& treeName : treeNames) {
    
        TTree *oldTree = (TTree *)inputFile->Get(treeName.c_str());
    if (!oldTree) {
        std::cout << "Tree " << treeName << " not found. Skipping.\n";
        continue;
    }

    outputFile->cd();
    TTree *tree = oldTree->CloneTree(0); // Make empty clone
    
    
        

    std::cout << "Processing tree: " << treeName << std::endl;

    // Set up to read from oldTree
    IEvent* ev = new IEvent();
    oldTree->SetBranchAddress("Events", &ev);

    // Prepare new tree with SiPM branch
    IEvent* dummyEv = new IEvent(); // needed to preserve structure
    ISiPM* sipmInfo = new ISiPM(256);
    tree->Branch("Events", "IEvent", &dummyEv, 64000, 99); // optional if you want full structure copied
    TBranch* sipmBranch = tree->Branch("SiPM", "ISiPM", &sipmInfo, 64000, 99);

    std::vector<float> hv;
    std::vector<float> sipmTemp;
    std::vector<double> absoluteGain(256);
    std::vector<double> tempCorrection(256);
    std::vector<double> optXTalk(256);        
    std::vector<double> amplitudeToPE(256);
    std::vector<double> chargeToPE(256);

    double hvSettingPx, relOverV, overV;
    
    Long64_t nEntries = oldTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        oldTree->GetEntry(i);

        hv = ev->Gethv();
        sipmTemp = ev->GetSiPMTemp();

        for (int j = 0; j < 256; ++j) {
            hvSettingPx = hv[IUtilities::GetHVChannel(j)];
            relOverV = ICalibration::GetRelativeOverVoltage(j, hvSettingPx, 44);
            overV = ICalibration::GetOverVoltage(j, hvSettingPx);
            tempCorrection[j] = ICalibration::GetRelativeGain(j,sipmTemp[j/16],hvSettingPx);
            //optXTalk[j] = 0.01 * calib->GetAmplitude(j);              
            absoluteGain[j] = GAIN_REF / tempCorrection[j];
            amplitudeToPE[j] = calib->GetAmplitude(j) * relOverV;
            chargeToPE[j] = calib->GetCharge(j) * relOverV;
            
            std::cout << i << "\t" << j << "\t" << calib->GetAmplitude(j) << "\t" << absoluteGain[j] << "\t"
                          << tempCorrection[j] << "\t" << amplitudeToPE[j] << "\t" << chargeToPE[j] << "\t"
                          << relOverV << "\t" << sipmTemp[j / 16] << "\t" << hvSettingPx << "\t" << overV << std::endl;
        }

        sipmInfo->SetGain(absoluteGain);
        sipmInfo->SetTCorrection(tempCorrection);
        //sipmInfo->SetOptXTalk(optXTalk);
        sipmInfo->SetAmplToPE(amplitudeToPE);
        sipmInfo->SetChgToPE(chargeToPE);

        tree->Fill(); 
    }

    tree->Write("", TObject::kOverwrite);
    delete ev;
    delete dummyEv;
    delete sipmInfo;
    }

    inputFile->Close();
    outputFile->Close();
    std::cout << "Added SiPM branch to all relevant trees and saved as: " <<outputFileName<< std::endl;
    return 0;
}
