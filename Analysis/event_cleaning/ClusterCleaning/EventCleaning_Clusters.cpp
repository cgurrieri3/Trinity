#include "EventCleaning_Clusters.h"



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
    // Load in all the files
    std::string FolderPath = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
    std::vector<std::string>fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");

    CreateFileName(folString);

    
    // Creates a root file that stores all the data    
    file = new TFile(OutputFileRoot.c_str(), "RECREATE");  // "RECREATE" to overwrite if it exists
    file->Close();
    
    
    plothelp = new PlotHelp(); // initialize the plot help class in scope for whole program
    // Create a new canvas with 2x2 sub-panels
    TCanvas* c_cleaned = new TCanvas("CleanedDisplay", "Cleaned CameraPlot", 950, 1000);
    c_cleaned->Divide(2, 2);
    c_cleaned->Print(OutputFilePDFOpen.c_str());

    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
    // for(int f = 192; f<196; f++){
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            continue; // Skip to the next branch if not present
        }
        LoadEvents(FilePath, "Test");
        LoadEventsHLED(FilePath, "HLED");
        ev = new Event();
        SetBranches(ev);
        SetBranchesHLED(evHLED);
        evHLED = new Event();
        
        
        int nEntries = tree->GetEntries();
        int nEntriesHLED = treeHLED->GetEntries();
        if (nEntries==0) {
            continue;
        }    
        int TotalEvents = nEntries + nEntriesHLED;
        std::cout << "Total Number of Events: " << nEntries << std::endl;
        std::cout << "Total Number of HLED Events: " << nEntriesHLED << std::endl;
        for(int EventCounter = 0; EventCounter < TotalEvents; EventCounter++){
            cev = new CEvent();
            plothelp->AddTotalEvent(1);
            
            std::vector<Int_t> TrigMus;
            if (EventCounter < nEntries) {
                tree->GetEntry(EventCounter);
                TrigMus = ev->GetROIMusicID();
            } else {
                treeHLED->GetEntry(EventCounter-nEntries);
                TrigMus = evHLED->GetROIMusicID();
            }
            // tree->GetEntry(EventCounter);
    
            // Set up all the information for the Cleaning Event class
            Pulse *pulse;
            Pulse *pulse1;
            // std::vector<vector<int>> trace = event->GetSignalValue();
            std::vector<float> Amplitudes;
            std::vector<float> AmplitudesTimeBin;
            std::vector<int> PeakTimeBin;
            std::vector<float> Pedestals;
            for (int k = 0; k < MaxNofChannels; k++) {
                if (EventCounter < nEntries) {
                    pulse = new Pulse(ev->GetSignalValue(k));
                    pulse1 = new Pulse(ev->GetSignalValue(k),239,239,512,true);
                } else {
                    pulse = new Pulse(evHLED->GetSignalValue(k));
                    pulse1 = new Pulse(evHLED->GetSignalValue(k),239,239,512,true);
                }
                // pulse = new Pulse(ev->GetSignalValue(k));
                Amplitudes.push_back(pulse->GetAmplitude());
                Pedestals.push_back(pulse->GetPedestal());
                PeakTimeBin.push_back(pulse->GetTimePeak());
                AmplitudesTimeBin.push_back(pulse1->GetAmplitude());
                delete pulse;
                delete pulse1;
            }
            
            cev->SetTriggeredMUSICID(TrigMus[0]);
            // cout << "Music Position ID: " << cev->GetTriggeredMUSICID() << endl;
            cev->SetAmplitudeValues(Amplitudes);
            cev->SetAmplitudeValuesTimeBin(AmplitudesTimeBin);
            cev->SetPeakTimeBin(PeakTimeBin);
            
            cev->SetAverageAmplitude(util->GetEventAverageAmplitude(Amplitudes));
            // cout << cev->GetAverageAmplitude() << endl;
            if (cev->GetAverageAmplitude() > FlasherEventsCutOff) {
                
                plothelp->AddHLEDEvent(1);
                continue;
            }
            float sumsq = 0;
            float RMS = 0;
            for (std::size_t i = 0; i < AmplitudesTimeBin.size(); i++){
                sumsq += AmplitudesTimeBin[i]*AmplitudesTimeBin[i];
                // cout << "sumsq: " << sumsq << endl;
                RMS = sqrt(sumsq)/MaxNofChannels;
                // cout << "RMS: " << RMS << endl;
            }
            if (cev->GetAverageAmplitude() > 350 && RMS < 15){
                
                plothelp->AddHLEDEvent(1);
                continue;
            }
            
            std::vector<int> SaturatedPixels = util->GetSaturatedPixels(ev->GetSignalValue());
            cev->SetSaturatedPixels(std::accumulate(SaturatedPixels.begin(), SaturatedPixels.end(), 0));
            if (cev->GetSaturatedPixels() > SaturatedPixelCutoff) {
                
                plothelp->AddCleanedEvent(1);
                continue;
            }
            
            cev->SetMaxAmplitude(util->GetMaximum(Amplitudes,TrigMus[0]));
            if (cev->GetMaxAmplitude() < TriggeredChannelAmpCutOff) {
                
                plothelp->AddCleanedEvent(1);
                continue;
            }
            
            cev->SetPedestalValues(Pedestals);
            cev->SetEventNumber(EventCounter);
            cev->SetMaxAmplitudePixelID(util->GetMaximumPixelID(Amplitudes,TrigMus[0]));
            cev->SetSumAmplitude(std::accumulate(Amplitudes.begin(), Amplitudes.end(), 0));
            cev->SetFilename(fileNamesVec[f]);

            /** 
            --------Create Panel 1 
            **/
            c_cleaned->cd(1);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);

            std::string filenameTitle = (cev->GetFilename()).substr(23,5);
            // create TH2D for Camera
            hcam_panel1 = new TH2F("hcam_panel1", Form("Calibrated Image --File# %s-- Event# %i ", filenameTitle.c_str() ,EventCounter), 16, -0.5, 15.5, 16, -0.5, 15.5);
            hcam_panel1->SetDirectory(0);
            cev->SetPanel1(hcam_panel1,CalibrationFactorDir, folString);
            
            // cev->SetAverageAmplitudeRMS(cev->GetAmplitudeValuesTimeBin());
            // Add a subtitle using TLatex
            TLatex *subtitle = new TLatex();
            subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
            subtitle->SetTextSize(0.03);
            
            subtitle->DrawLatex(0.1, 0.92, Form("Average Amplitude Whole Camera: %.2f - Average RMS Whole Camera: %.2f", util->convertADC2PE(cev->GetAverageAmplitude()), RMS));
            subtitle->DrawLatex(0.25, 0.12, Form("Triggered MUSIC: %i  Triggered Pixel: %i",cev->GetTriggeredMUSICID(),util->GetMaximumPixelID(Amplitudes,TrigMus[0])));
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
            hcam_panel2->SetTitle(Form("Amplitude cut Image --File# %s -- Event# %i ", filenameTitle.c_str() ,EventCounter));
            hcam_panel2->SetDirectory(0);
            cev->SetPanel2(hcam_panel2,CorePixelAmpCutOff);
            std::vector<int> survivingPanel2Pixels = cev->GetSurvivingPixelPanel2();
            if (survivingPanel2Pixels.size() < PixelSurviveCutOff) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete cev;
      
                plothelp->AddCleanedEvent(1);
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
            hcam_panel3->SetTitle(Form("Connected Pixel Image --File# %s -- Event# %i ", filenameTitle.c_str() ,EventCounter));
            hcam_panel3->SetDirectory(0);
            
            cev->SetPanel3(hcam_panel3, neighborDir);
            
            if (cev->GetSurvivingPixelPanel3().size() < PixelSurviveCutOff) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
        
                plothelp->AddCleanedEvent(1);
                continue;
            }
            
            /** 
            --------Create Panel 4
            **/
            c_cleaned->cd(4);
            gPad->SetLeftMargin(0.15);  // Increase left margin
            gPad->SetBottomMargin(0.25); // Increase bottom margin
            gPad->SetRightMargin(0.15);
            
            
            // std::vector<double> CenterOfGravity(2, 0.0);
            //Create the PCA
            PCA pca(2);
            
            // do the Principle componet analysis on the cleaned data
            LoadDataPCA(pca,hcam_panel3,cev->GetSurvivingPixelTotalAmpPanel3());
            // //pca.ComputeCovarianceMatrix();
            
            TVectorD eigenVals;
            TMatrixD eigenVecs; 
            double EllipicRatio=StartPanel4(pca,eigenVals,eigenVecs);
            if (EllipicRatio > (LWRatioCutOff/100.0)) {
                delete hcam_panel1;
                delete hcam_panel2;
                delete hcam_panel3;
                delete cev;
                plothelp->AddCleanedEvent(1);
                continue;
            }
            
            plothelp->AddSurvivingEvent(1);
            hcam_panel4 = (TH2F*)hcam_panel3->Clone("hcam_panel4");
            hcam_panel4->SetDirectory(0);
            hcam_panel4->SetTitle(Form("Cleaned Image --File# %s -- Event# %i ", filenameTitle.c_str() ,EventCounter));
            CompletePanel4(pca,hcam_panel4,cev,EllipicRatio,eigenVals,eigenVecs);            

            // create TH2D for Gain 
            file = new TFile(OutputFileRoot.c_str(), "UPDATE");
            c_cleaned->Write(Form("F%s-E%i", filenameTitle.c_str() ,EventCounter));
            hcam_panel4->Write(Form("F%s-E%iTH2F", filenameTitle.c_str() ,EventCounter));

            // get and write the parameters to a text file
            std::ifstream inputFile("output_variables.txt");
            if (inputFile.is_open()) {
                std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
                inputFile.close();
                TObjString* textContent = new TObjString(fileContent.c_str()); // Create a TObjString to store the content
                textContent->Write(Form("F%s-E%iPar", filenameTitle.c_str() ,EventCounter), TObject::kOverwrite);
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
        delete tree;
        delete treeHLED;
        delete ev;
        delete evHLED;
    }
    file = new TFile(OutputFileRoot.c_str(), "UPDATE");
    
    plothelp->PlothLW(c_cleaned,OutputFilePDF);
    plothelp->PlothSize(c_cleaned,OutputFilePDF);    
    plothelp->PlothSizeConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothSizeSPC(c_cleaned,OutputFilePDF);    
    plothelp->PlothLWConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothLWSPC(c_cleaned,OutputFilePDF);    
    plothelp->PlothSPCConc(c_cleaned,OutputFilePDF);    
    plothelp->PlothCOG(c_cleaned,OutputFilePDF);    
    plothelp->PlothTPID(c_cleaned,OutputFilePDF);
    
    
    file->Close();
    c_cleaned->Print(OutputFilePDFClose.c_str());
    cout << "Completed!" << endl;   
    cout << "Total Events: " << plothelp->GetTotalEvent() << endl;
    delete plothelp;
}






