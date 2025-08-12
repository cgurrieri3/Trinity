#include "EventCleaning.h"



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/* Main function completes the following steps:
	1. Reads CSV file which has: event number, pixel number of trigger pixel, pixel number of bifocal pixel
	2. Loops through all events provided
		a. creates canvas for each event and sets up histograms
		b. Reads CSV file for each event with info: pixelnumber, amplitude, peak time
		c. Begin the cleaning procedure
		d. Save Amplitudes for surviving pixels/events
		e. display original and cleaned plot for testing
 */


//void EventCleaning_Clusters(std::string folString){
int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    
    // Get the Arguments
    std::string folString = argv[1];
    std::string time = "";

    // Load in all the files
    std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
    std::vector<std::string> fileNamesVec;
    
    if (folString.find("bkg")==0) {
        whatData = "bkg";
        fileNamesVec=util->readFileToVectorString(Form("%s%s.txt",bkgDir.c_str(),folString.c_str()));
        // CreateFileName(folString,whatData);
        
    } else if (folString.find("muon")==0) {
        whatData = "muon";
        fileNamesVec=util->readFileToVectorString(Form("%s%s.txt",muonDir.c_str(),folString.c_str()));
        // CreateFileName(folString,whatData);

    } else if (folString.find("sim")==0) {
        folString = folString.substr(3, 19); // Extract date from filename
        cout << "folString: " << folString << endl;
        whatData = "sim";
        FolderPath = Form("%s%s/",simDir.c_str(),folString.c_str());
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
        // remove files in the vec that have plots_
        fileNamesVec.erase(
            std::remove_if(
                fileNamesVec.begin(),
                fileNamesVec.end(),
                [](const std::string& name) {
                    return name.find("plots_") != std::string::npos;
                }
            ),
            fileNamesVec.end()
        );
    } else {
        // std::string FolderPath = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
    }
    
    CreateFileName(folString,whatData);
    
    
    // Creates a root file that stores all the data    
    file = new TFile(OutputFileRoot.c_str(), "RECREATE");  // "RECREATE" to overwrite if it exists
    file->Close();
    
    
    plothelp = new PlotHelp(); // initialize the plot help class in scope for whole program
    // Create a new canvas with 2x2 sub-panels
    TCanvas* c_cleaned = new TCanvas("CleanedDisplay", "Cleaned CameraPlot", 950, 1000);
    c_cleaned->Divide(2, 2);
    c_cleaned->Print(OutputFilePDFOpen.c_str());
    
    // for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
    // for(int f = 60; f<80; f++){
    // fileNamesVec.assign(fileNamesVec.begin() + 134, fileNamesVec.begin() + 135);
    // fileNamesVec.assign(fileNamesVec.begin() + 10, fileNamesVec.begin() + 12);
    std::vector<int> indices(fileNamesVec.size());
    std::iota(indices.begin(), indices.end(), 0);  // Fill with 0, 1, ..., N-1

    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [&](int f) {
        if (whatData == "bkg" ||  whatData == "muon") {
            std::string date = fileNamesVec[f].substr(7, 10); // Extract date from filename
            // remove - from the date string
            date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
            folString = date; // Update folString with the extracted date
            FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
        } 

        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            return; // Skip to the next branch if not present
        }
        int TotalEvents;
        int nEntries;
        // cout << "whatData: " << whatData << endl;
        if (whatData !="muon" && whatData !="sim"){
            LoadEvents(FilePath, "Test");
            LoadEventsHLED(FilePath, "HLED");
            ev = new IEvent();
            evHLED = new IEvent();
            sipmInfo = new ISiPM();
            SetBranches(ev);
            SetBranchesHLED(evHLED);
            tree->SetBranchAddress("SiPM", &sipmInfo);
            treeHLED->SetBranchAddress("SiPM", &sipmInfo);
            
            
            nEntries = tree->GetEntries();
            int nEntriesHLED = treeHLED->GetEntries();
            if (nEntries==0) {
                return; // acts like continue in parrellization
            }    
            TotalEvents = nEntries + nEntriesHLED;
            std::cout << "Total Number of Events: " << nEntries << std::endl;
            std::cout << "Total Number of HLED Events: " << nEntriesHLED << std::endl;

        } else if (whatData== "muon"){
            
            LoadEvents(FilePath, "Forced");
            ev = new IEvent();
            sipmInfo = new ISiPM();
            SetBranches(ev);
            tree->SetBranchAddress("SiPM", &sipmInfo);
            
            nEntries = tree->GetEntries();
            if (nEntries==0) {
                return;
            }    
            TotalEvents = nEntries;
            std::cout << "Total Number of Forced Events: " << nEntries << std::endl;
        } else if (whatData == "sim"){
            cout << "Loading Sim File: " << FilePath << endl;
            LoadEvents(FilePath, "Test");
            ev = new IEvent();
            // sipmInfo = new ISiPM();
            SetBranches(ev);
            // tree->SetBranchAddress("SiPM", &sipmInfo);
            
            nEntries = tree->GetEntries();
            if (nEntries==0) {
                return;
            }    
            TotalEvents = nEntries;
            std::cout << "Total Number of Sim Test Events: " << nEntries << std::endl;
        }
        
        for(int EventCounter = 0; EventCounter < TotalEvents; EventCounter++){
            cev = new CEvent();
            cev->SetEventDate(stoi(folString));

            plothelp->AddTotalEvent(1);
            std::vector<Int_t> TrigMus;
            std::vector<float> AmplitudesTimeBin;
            std::vector<int> PeakTimeBin;
            std::vector<float> Pedestals;
            Pulse *pulse;
            
            if(whatData != "muon") {
                if (EventCounter < nEntries) {
                    tree->GetEntry(EventCounter);
                    TrigMus = ev->GetROIMusicID();
                } else {
                    treeHLED->GetEntry(EventCounter-nEntries);
                    TrigMus = evHLED->GetROIMusicID();
                    // EventCounter = EventCounter -nEntries;
                }
                // tree->GetEntry(EventCounter);
        
                // Set up all the information for the Cleaning Event class
                Pulse *pulse1;
                // std::vector<vector<int>> trace = event->GetSignalValue();
                
                
                for (int k = 0; k < MaxNofChannels; k++) {
                    if (EventCounter < nEntries) {
                        pulse = new Pulse(ev->GetSignalValue(k));
                        pulse1 = new Pulse(ev->GetSignalValue(k),TimeBinAll,TimeBinAll,512,true);
                    } else {
                        pulse = new Pulse(evHLED->GetSignalValue(k));
                        pulse1 = new Pulse(evHLED->GetSignalValue(k),TimeBinAll,TimeBinAll,512,true);
                    }
                    // pulse = new Pulse(ev->GetSignalValue(k));
                    Pedestals.push_back(pulse->GetPedestal());
                    PeakTimeBin.push_back(pulse->GetTimePeak());
                    AmplitudesTimeBin.push_back(pulse1->GetAmplitude());
                    delete pulse;
                    delete pulse1;
                }

            } else if  (whatData == "muon"){
                tree->GetEntry(EventCounter);
                TrigMus = ev->GetROIMusicID();
                
                for (int k = 0; k < MaxNofChannels; k++) {
                    // pulse = new Pulse(ev->GetSignalValue(k));
                    pulse = new Pulse(ev->GetSignalValue(k),230,250,512,true);

                    Pedestals.push_back(pulse->GetPedestal());
                    PeakTimeBin.push_back(pulse->GetTimePeak());
                    AmplitudesTimeBin.push_back(pulse->GetAmplitude());
                    delete pulse;

                }
            }
            
            
            
            cev->SetTriggeredMUSICID(TrigMus[0]);
            if (whatData == "sim"){
                PixelSurviveCutOff=1;
                sizeCutoff = 0; 
                NumberOfCoresCutoff = 0; 
                // PixelSurviveCutOff=5;
                // sizeCutoff = 200; 
                // NumberOfCoresCutoff = 3; 
                
                // CorePixelCutOff = 100;
                std::vector<double> fakeGain(256, 1.0); 
                cev->SetAmplitudeValuesTimeBin(AmplitudesTimeBin,CalibrationFactorDir, std::to_string(20241011),fakeGain);
            } else {
                cev->SetAmplitudeValuesTimeBin(AmplitudesTimeBin,CalibrationFactorDir, std::to_string(cev->GetEventDate()),sipmInfo->GetGain());

            }
            cev->SetPeakTimeBin(PeakTimeBin);
            AmplitudesTimeBin = cev->GetAmplitudeValuesTimeBin(); // reset the  amplitudesTimeBin to be aboslute gain calibrated
            cev->SetAverageAmplitude(util->GetEventAverageAmplitude(AmplitudesTimeBin));
            cev->SetRMS(AmplitudesTimeBin);
            plothelp->AddtoRMSratioAVGamp(cev->GetRMSoverAvgAmp());
            // cout <<"Avg amplitude Before: "  << util->GetEventAverageAmplitude(AmplitudesTimeBin) << endl;
            // cout <<"Avg amplitude: "  << cev->GetAverageAmplitude() << endl;
            
            // the amplitude is different when its prior to the october 2024 site visit
            // if (cev->GetEventDate() < 20241001 && whatData != "muon") {
            //     FlasherEventsCutOff = 350; // 350 ADC/8 PE
            // }
            cev->SetEventNumber(EventCounter);
            if (EventCounter > nEntries && whatData != "muon") {
                cev->SetEventNumber(EventCounter-nEntries);
            } 
            
            // update limit for flasher events cutoff deperciated for now
            // if (cev->GetAverageAmplitude() > (FlasherEventsCutOff/util->GetADCtoPEratio()) && whatData != "muon" && whatData != "sim") {
            //     plothelp->AddHLEDEvent(1);
            //     continue;
            // }
            
            
            if (cev->GetRMSoverAvgAmp() < 0.15 && whatData != "muon" && whatData != "sim") {
                
                plothelp->AddHLEDEvent(1);
                continue;
            }


            
            // Remove events where the door is closed. This can be done by checking the HV currents of the SiPMs and 
            // if the sky seeing Channels are below 3.8 mA you know that the door is closed.
            // if (ev->Gethvc()[0] < 3.8 && ev->Gethvc()[2] < 3.8){
                //     plothelp->AddCleanedEvent(1);
                //     continue;
                // }
                
            std::vector<int> SaturatedPixels = util->GetSaturatedPixels(ev->GetSignalValue());
            cev->SetSaturatedPixels(std::accumulate(SaturatedPixels.begin(), SaturatedPixels.end(), 0));
            if (cev->GetSaturatedPixels() > SaturatedPixelCutoff && whatData != "muon" ) {
                
                plothelp->AddPreCleaned(1);
                continue;
            }
            cout << "Event Number: " << cev->GetEventNumber() << " triggered  Music: " <<cev->GetTriggeredMUSICID() <<endl;
            // cout << "triggered Music pair: " << util->GetMUSICMate(cev->GetTriggeredMUSICID(), true) << endl;
            
            // //check which music has the highest amplitude 
            // cout << "triggered Music amp: " << util->GetMaximum(AmplitudesTimeBin,cev->GetTriggeredMUSICID()) << endl;
            // cout << "triggered Music pair amp: " << util->GetMaximum(AmplitudesTimeBin,util->GetMUSICMate(cev->GetTriggeredMUSICID(), true)) << endl;
            if(util->GetMaximum(AmplitudesTimeBin,cev->GetTriggeredMUSICID()) < util->GetMaximum(AmplitudesTimeBin,util->GetMUSICMate(cev->GetTriggeredMUSICID(), true))){
                cev->SetTriggeredMUSICID(util->GetMUSICMate(cev->GetTriggeredMUSICID(), true));
                // cout << "Replaced. " << endl;
                // cout << "triggered Music pair: " << util->GetMUSICMate(cev->GetTriggeredMUSICID(), true) << endl;
            }
            
            // cout << "triggered  Music: " <<cev->GetTriggeredMUSICID() << endl;
            cev->SetMaxAmplitude(util->GetMaximum(AmplitudesTimeBin,cev->GetTriggeredMUSICID()));
            if (cev->GetMaxAmplitude() < TriggeredChannelAmpCutOff/util->GetADCtoPEratio()) {
                plothelp->AddPreCleaned(1);
                continue;
            }
            

            cev->SetPedestalValues(Pedestals);
            cev->SetMaxAmplitudePixelID(util->GetMaximumPixelID(AmplitudesTimeBin,cev->GetTriggeredMUSICID()));
            cout << "Triggered Pixel ID: " << cev->GetMaxAmplitudePixelID() << endl;

            // removed events that have pixel 76 
            // if (cev->GetMaxAmplitudePixelID() == 78 || cev->GetMaxAmplitudePixelID() == 79 || cev->GetMaxAmplitudePixelID() == 77|| cev->GetMaxAmplitudePixelID() == 76 || cev->GetMaxAmplitudePixelID() == 75) {
            //     cout << "Triggered Pixel ID 78 or 79 skipping event." << endl;
            //     plothelp->AddPreCleaned(1);
            //     continue;
            // }

            // removed the hotspot on the worst night 
            if ((cev->GetTriggeredMUSICID() == 18 || cev->GetTriggeredMUSICID() == 19) && (folString == "20250305" || folString == "20250331" || folString == "20250328")){
                cout << "Triggered Music 18,19 skipping event." << endl;
                plothelp->AddPreCleaned(1);
                continue;
            }

            cev->SetSumAmplitude(std::accumulate(AmplitudesTimeBin.begin(), AmplitudesTimeBin.end(), 0));
            cev->SetFilename(fileNamesVec[f]);
                
                
                
                /** 
            --------Create Panel 1 
            **/
            c_cleaned->cd(1);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            std::string filenameTitle = (cev->GetFilename()).substr(18,5);
            // cout << "Filename: " << cev->GetFilename()<< endl;
            if (whatData == "sim") {
                std::string temp = (cev->GetFilename()).substr(52,3);
                filenameTitle = folString.substr(9,6);
                cout << filenameTitle << endl;
                filenameTitle = filenameTitle.append("_");
                filenameTitle = filenameTitle.append(temp);
                filenameTitle.erase(std::remove(filenameTitle.begin(), filenameTitle.end(), '.'), filenameTitle.end());
                filenameTitle.erase(std::remove(filenameTitle.begin(), filenameTitle.end(), 'r'), filenameTitle.end());
                
            }
            // create TH2D for Camera

            hcam_panel1 = new TH2F("hcam_panel1", Form("Calibrated Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()), 16, -0.5, 15.5, 16, -0.5, 15.5);
            hcam_panel1->SetDirectory(0);
            cev->SetPanel1(hcam_panel1);
            
            // cev->SetAverageAmplitudeRMS(cev->GetAmplitudeValuesTimeBin());
            // Add a subtitle using TLatex
            TLatex *subtitle = new TLatex();
            subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
            subtitle->SetTextSize(0.03);
            
            subtitle->DrawLatex(0.1, 0.92, Form("Avg Amp Whole Camera : %.2f - Avg RMS: %.2f", cev->GetAverageAmplitude(), cev->GetRMS()));
            subtitle->DrawLatex(0.25, 0.12, Form("Triggered MUSIC: %i  Triggered Pixel: %i",cev->GetTriggeredMUSICID(),util->GetMaximumPixelID(AmplitudesTimeBin,TrigMus[0])));
            delete subtitle;
            
            /** 
            --------Create Panel 2
            **/
            // Plot hcam using the data from the CSV file
            c_cleaned->cd(2);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            
            
            hcam_panel2 = (TH2F*)hcam_panel1->Clone("hcam_panel2");
            hcam_panel2->SetTitle(Form("Amplitude cut Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel2->SetDirectory(0);
            cev->SetPanel2(hcam_panel2,CorePixelAmpCutOff/util->GetADCtoPEratio());
            std::vector<int> survivingPanel2Pixels = cev->GetSurvivingPixelPanel2();
            if (survivingPanel2Pixels.size() < PixelSurviveCutOff) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete cev;
                
                plothelp->AddCleanedPanel2(1);
                cout << "Removed Panel 2" << endl;
                continue;
            }
            
            
            /** 
            --------Create Panel 3
            **/
            c_cleaned->cd(3);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            hcam_panel3 = (TH2F*)hcam_panel2->Clone("hcam_panel3");
            hcam_panel3->SetTitle(Form("Connected Pixel Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel3->SetDirectory(0);
            
            
            cev->SetPanel3(hcam_panel3, neighborDir,NumberOfCoresCutoff,CorePixelCutOff, CrosstalkAllowanceCutoff);
            
            
            //find configure events with amp high and above 13
            // cout << "Configure Event?: " << cev->isConfigureEvent() << endl;
            if (cev->isConfigureEvent() == 1){
                plothelp->AddCleanedPanel3(1);
                cout << "Removed Panel 3 Configure" << endl;
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                continue;
            }
            cout << "YOU "<<cev->GetSurvivingPixelPanel3().size() << endl;
            if (cev->GetSurvivingPixelPanel3().size() < PixelSurviveCutOff) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                
                plothelp->AddCleanedPanel3(1);
                cout<< "Removed Panel 3  NUmber of Pixels" << endl;
                continue;
            }

            if (cev->GetSurvivingPixelTotalAmpPanel3() <= sizeCutoff ) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                
                plothelp->AddCleanedPanel3(1);
                cout<< "Removed Panel 3 Size" << endl;
                continue;
            }
            cout << cev->GetMaxAmplitude()  << " , " << cev->GetSecondCoreAmp() << endl;
            if (cev->GetMaxAmplitude() == 0.0 || cev->GetSecondCoreAmp() == 0.0) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                
                plothelp->AddCleanedPanel3(1);
                cout<< "Removed Panel 3 Pixel amount" << endl;
                continue;
            }
            
            /** 
            --------Create Panel 4
            **/
            c_cleaned->cd(4);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            // At some point get panel four into the CEvent class
            // std::vector<double> CenterOfGravity(2, 0.0);
            //Create the PCA
            PCA pca(2);
            
            // do the Principle componet analysis on the cleaned data
            LoadDataPCA(pca,hcam_panel3,cev->GetSurvivingPixelTotalAmpPanel3());
            // //pca.ComputeCovarianceMatrix();
            
            TVectorD eigenVals; // remove the use of the pointer because this is already in the PCA class and calulated in LoadDataPCA
            TMatrixD eigenVecs; 
            std::vector<double> EllipicRatio=StartPanel4(pca,eigenVals,eigenVecs); // to do make EllipicRatio a member of cev
            //ex. cev->SetEllipticRatio(EllipicRatio);
            if (EllipicRatio[2] > (WLRatioCutOff/100.0)) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                plothelp->AddCleanedPanel3(1);
                continue;
            }
            
            
            plothelp->AddSurvivingEvent(1);
            plothelp->AddtoEventDate(cev->GetEventDate());
            hcam_panel4 = (TH2F*)hcam_panel3->Clone("hcam_panel4");
            hcam_panel4->SetDirectory(0);
            hcam_panel4->SetTitle(Form("Cleaned Img --N# %i --F# %s-- E# %i ", cev->GetEventDate(), filenameTitle.c_str() ,cev->GetEventNumber()));
            CompletePanel4(pca,hcam_panel4,cev,EllipicRatio,eigenVals,eigenVecs);            
            
            // create TH2D for Gain 
            file = new TFile(OutputFileRoot.c_str(), "UPDATE");
            c_cleaned->Write(Form("N%i-F%s-E%i", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel1->Write(Form("PreN%i-F%s-E%iTH2F", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()));
            hcam_panel4->Write(Form("PostN%i-F%s-E%iTH2F", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()));
            
            // get and write the parameters to a text file
            std::ifstream inputFile("output_variables.txt");
            if (inputFile.is_open()) {
                std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
                inputFile.close();
                TObjString* textContent = new TObjString(fileContent.c_str()); // Create a TObjString to store the content
                textContent->Write(Form("N%i-F%s-E%iPar", cev->GetEventDate(),filenameTitle.c_str() ,cev->GetEventNumber()), TObject::kOverwrite);
                delete textContent;
            }
            file->Close();
            c_cleaned->Print(OutputFilePDF.c_str());
            // Delete the text file after use
            if (std::remove("output_variables.txt") != 0) {
                std::cerr << "Error deleting output_variables.txt" << std::endl;
            } else {
                std::cout << "output_variables.txt successfully deleted." << std::endl;
            }
            
            delete hcam_panel1;
            delete hcam_panel2;
            delete hcam_panel3;
            delete hcam_panel4;
            delete cev;
            
        }
     
        if (whatData != "muon"){
            delete treeHLED;
            delete evHLED;
        }
        delete tree;
        delete ev;
    });
    file = new TFile(OutputFileRoot.c_str(), "UPDATE");
    
    plothelp->PlothWL(c_cleaned,OutputFilePDF,outDir,to_string(cev->GetEventDate()));
    plothelp->PlothdistLandW(c_cleaned,OutputFilePDF);
    plothelp->PlothSize(c_cleaned,OutputFilePDF);    
    plothelp->PlothSizeConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothSizeSPC(c_cleaned,OutputFilePDF);    
    plothelp->PlothCoreRatio(c_cleaned,OutputFilePDF);    
    plothelp->PlothNumberofCores(c_cleaned,OutputFilePDF);    
    plothelp->PlothCoreRatioandNumberofCores(c_cleaned,OutputFilePDF);    
    plothelp->PlothWLConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothWLSPC(c_cleaned,OutputFilePDF); 
    plothelp->PlothSIZEWL(c_cleaned,OutputFilePDF);   
    plothelp->PlothLvW(c_cleaned,OutputFilePDF);
    plothelp->PlothSPCConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothCOG(c_cleaned,OutputFilePDF);    
    plothelp->PlothTPID(c_cleaned,OutputFilePDF);
    plothelp->PlothRMSratioAvg(c_cleaned, OutputFilePDF);
    plothelp->PlothEventDateTriggeredPixel(c_cleaned, OutputFilePDF);
    plothelp->PlothncoreSIZE(c_cleaned, OutputFilePDF);
    plothelp->PlothnCoreSPC(c_cleaned, OutputFilePDF);
    plothelp->PlothCRSPC(c_cleaned, OutputFilePDF);
    plothelp->PlothSIZEWandL(c_cleaned, OutputFilePDF);

    file->Close();
    c_cleaned->Print(OutputFilePDFClose.c_str());
    util->setFilePermissions(OutputFileRoot.c_str());
    cout << "Completed!" << endl;   
    // cout << "Total Events: " << plothelp->GetTotalEvent() << endl;
    delete plothelp;
}






