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

    // Create a new canvas
    
    // Creates a root file that stores all the data    
    file = new TFile(Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
        outDir.c_str(),
        folString.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
    ), "RECREATE");  // "RECREATE" to overwrite if it exists
    file->Close();
    // Create the file name with the cariables in the name for sorting of them. 
    // _TC_#_ = TriggeredChannelAmpCutOff
    // TB_#_ = TimeBinAll
    // NP_#_ = CorePixelAmpCutOff
    // s_#_ = SaturatedPixelCutoff
    // FA_#_ = FlasherEventsCutOff
    // mp_# = PixelSurviveCutOff
    plothelp = new PlotHelp();
    TCanvas* c_cleaned = new TCanvas("CleanedDisplay", "Cleaned CameraPlot", 950, 1000);
    c_cleaned->Divide(2, 2);


    c_cleaned->Print(Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf[",
        outDir.c_str(),
        folString.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
        ));

    // int tEntries = 0;
    // int zeroentries = 0;
    // int hledentries = 0;

    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
    // for(int f = 192; f<196; f++){
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        if (!isBranchPresentInFile(FilePath, "Test")) {
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
            for (int i = 0; i < AmplitudesTimeBin.size(); i++){
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
            
            
            std::vector<double> CenterOfGravity(2, 0.0);
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
            file = new TFile(Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
                outDir.c_str(),
                folString.c_str(),
                TriggeredChannelAmpCutOff,
                TimeBinAll,
                CorePixelAmpCutOff, 
                SaturatedPixelCutoff,
                FlasherEventsCutOff,
                PixelSurviveCutOff,
                LWRatioCutOff,
                rmTopRow
            ), "UPDATE");
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
    file = new TFile(Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
        outDir.c_str(),
        folString.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
    ), "UPDATE");

    plothelp->PlothLW(c_cleaned);
    plothelp->PlothSize(c_cleaned);
    plothelp->PlothSizeConc(c_cleaned);
    plothelp->PlothSizeSPC(c_cleaned);
    plothelp->PlothLWConc(c_cleaned);
    plothelp->PlothLWSPC(c_cleaned);
    plothelp->PlothSPCConc(c_cleaned);
    plothelp->PlothCOG(c_cleaned);
    plothelp->PlothTPID(c_cleaned);

    file->Close();
    cout << "Completed!" << endl;   
    cout << "Total Events: " << plothelp->GetTotalEvent() << endl;
    delete plothelp;
}

// /***
// Start of Functions

// ***/



void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp){
    


    // (*COG)[0] = 0.0;
    // (*COG)[1] = 0.0;
   

    // Loop over the bins of the histogram and extract the bin content (data points)
    for (int bin = 0; bin <= MaxNofChannels; bin++) {
       
        int nx, ny;
        plottools->FindBin(bin, &nx, &ny);
        // Get the bin center for X and Y
        double weight = hist->GetBinContent(nx+1, ny+1);
         // Check if the value is infinity
        if (std::isinf(weight)) {
            weight = 0.0; // Set to 0
        }
        
        // Add the data point to the PCA object weighted by the bin content
        if (weight > 0) {  // Only add rows for non-zero bins

            std::vector<double> randomNumbers;
            for (double w = 0; w < weight; w++){
                randomNumbers = generateRandomNumbers();
                // cout << "nx: " << nx <<  " ny: " << ny << endl;
                // cout << "rand 1: " << nx + randomNumbers[0] << " rand 2: " << ny + randomNumbers[1] << endl;
                std::vector<double> data = {nx + randomNumbers[0], ny+randomNumbers[1]};
                // cout << "COG " << weight << "total" << totalAmp << endl;
                
                pca.AddRow(data);  
            }
            if (nx == 0){
                nx = 0.1;
            }
            if (ny == 0){
                ny = 0.1;
            }

            // (*COG)[0] += (nx * (weight/totalAmp));
            // (*COG)[1] += (ny * (weight/totalAmp));
        }   
    }
}

std::vector<double> generateRandomNumbers() {
    // Define the random number generator and distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.49, 0.49);

    // Generate two random numbers and return them as a pair
    return {dist(gen), dist(gen)};
}

double StartPanel4(PCA& pca, TVectorD& eigenVals, TMatrixD& eigenVecs) {
    eigenVals.ResizeTo(2);
    eigenVecs.ResizeTo(2,2);
    std::vector<double> sigmas;

    pca.ComputeEigenDecomposition();
    eigenVals = pca.GetEigenValues();
    eigenVecs = pca.GetEigenVectors();
    sigmas = pca.GetSigmas();                            
    
    double r1 = sqrt(eigenVals[0]);
    double r2 = sqrt(eigenVals[1]);
    double EllipicRatio = r2/r1;
    // cout << "LW " << r1 << "," << r2 << ", ratio:" <<  EllipicRatio << "is: " << LWRatioCutOff/100.0 << endl;
    
    return EllipicRatio;
}

void CompletePanel4(PCA& pca, TH2F* hcam_panel4, CEvent* cev, double EllipicRatio, TVectorD& eigenVals, TMatrixD& eigenVecs) {
    
    
    
    hcam_panel4->Draw("colz");
    plottools->DrawMUSICBoundaries();
    
    double meanx = hcam_panel4->GetMean(1);
    double meany = hcam_panel4->GetMean(2);
    // cout << "MEAN " << meanx << "," << meany << endl;
    
    double angledeg = atan(eigenVecs[1][0]/eigenVecs[0][0])*(180.0/3.141592653589793238463);
    double anglerad = atan(eigenVecs[1][0]/eigenVecs[0][0]);
    
    
    double r1 = sqrt(eigenVals[0]);
    double r2 = sqrt(eigenVals[1]);
    TArrow* arrow = new TArrow(meanx - 2*(r1*cos(anglerad)), meany - 2*(r1*sin(anglerad)), meanx + 2*(r1*cos(anglerad)), meany + 2*(r1*sin(anglerad)), 0.01, "|"); // "|>" option gives an arrowhead
    TArrow* arrow1 = new TArrow(meanx - 2*(r2*cos(anglerad+1.5708)), meany - 2*(r2*sin(anglerad+1.5708)), meanx + 2*(r2*cos(anglerad+1.5708)), meany + 2*(r2*sin(anglerad+1.5708)), 0.01, "|"); // "|>" option gives an arrowhead
    
    arrow->SetLineColor(kRed); // Optional: Set the color of the arrow
    arrow->Draw("SAME");             // Draw the arrow on the same canvas
    arrow1->SetLineColor(kBlue); // Optional: Set the color of the arrow
    arrow1->Draw("SAME");             // Draw the arrow on the same canvas
    
    TEllipse* ell = new TEllipse(meanx, meany,r1,r2,0,360,angledeg);
    ell->SetFillColorAlpha(kGreen,0.00);
    ell->Draw("SAME");
    TLatex* title = new TLatex();
    title->SetNDC(); // Set to Normalized Device Coordinates (NDC)
    title->SetTextSize(0.03);
    
    float areaEllipse = (3.8/16) * (3.8/16) * (cev->GetSurvivingPixelPanel3().size()); // area of an ellipse
    double conc = cev->GetAmplitudeValuesTimeBin()[cev->GetMaxAmplitudePixelID()]/cev->GetSurvivingPixelTotalAmpPanel3();
    
    std::vector<double> M3LongVar = getM3Long(meanx, meany, cev->GetSurvivingPixelPanel3(),cev->GetAmplitudeValuesTimeBin());
    std::string M3Longx = "+";
    if (M3LongVar[0] < 0){
        M3Longx = "-";
    }
    std::string M3Longy = "+";
    if (M3LongVar[1] < 0){
        M3Longy = "-";
    }
    title->DrawLatex(0.1, 0.92, Form("LW:%.2f Pixels:%i Area:%.2f Size:%.2f Conc:%.2f M3Long:(%s,  %s)",
        EllipicRatio,
        static_cast<int>((cev->GetSurvivingPixelPanel3()).size()),
        areaEllipse,
        cev->GetSurvivingPixelTotalAmpPanel3(),
        conc,
        M3Longx.data(),
        M3Longy.data()));
        delete title;
        
    plothelp->AddtoLW(EllipicRatio);
    plothelp->AddtoSize(cev->GetSurvivingPixelTotalAmpPanel3());
    plothelp->AddtoSurvivingPixelCount(cev->GetSurvivingPixelPanel3().size());
    plothelp->AddtoConcentation(conc);
    plothelp->AddtoCOGx(meanx);
    plothelp->AddtoCOGy(meany);
    plothelp->AddtoTriggeredPixelsID(cev->GetMaxAmplitudePixelID());
    
    // Open a text file to write variables
    std::ofstream outputFile("output_variables.txt");
    if (outputFile.is_open()) {
        outputFile << "LWRatio: " << EllipicRatio << "\n";
        outputFile << "Cleaned_count: " << static_cast<int>((cev->GetSurvivingPixelPanel3()).size()) << "\n";
        outputFile << "AreaEllipse: " << areaEllipse << "\n";
        outputFile << "Cleaned_total_amp: " << cev->GetSurvivingPixelTotalAmpPanel3() << "\n";
        outputFile << "Conc: " << conc << "\n";
        outputFile << "M3LongVar: (" << M3LongVar[0] << ", " << M3LongVar[1] << ")\n";
        outputFile.close();
    } else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }

            
}

std::vector<double> getM3Long(double xcog,double ycog, std::vector<int> sur_pix, std::vector<float> amps){
    // N = Max number of pixels survived
    // i = pixel index
    // x = pixel x location
    // x cog = x comp. Center of Gravity
    // q = charge of that pixel (start with amplitude for us)
    std::vector<double> M3Long = {0.0,0.0};
    int psize = sur_pix.size();

    // cout << "xcog: " << xcog << " ycog: "<< ycog << endl;
    
    std::vector<double> randomNumbers;
    for(int p = 0; p < psize; p++) {
        // randomNumbers = generateRandomNumbers();
        int nx, ny;
        plottools->FindBin(sur_pix[p], &nx, &ny);
        double p1x = std::pow((nx*1.0 - xcog*1.0),3.0);
        // cout << ny << endl;
        // cout << (ny*1.0 - ycog*1.0) << endl;
        double p1y = std::pow((ny*1.0 - ycog*1.0),3.0);
        double p2x = amps[sur_pix[p]]*p1x;
        double p2y = amps[sur_pix[p]]*p1y;
        // cout << amps[sur_pix[p]] << endl;

        M3Long[0] = M3Long[0] + p2x;
        M3Long[1] = M3Long[1] + p2y;
        // cout << "M3 summingx: " << M3Long[0] << endl;
        // cout << "M3 summingy: " << M3Long[1] << endl;
    }
    // cout << "M3 summing2x: " << M3Long[0] << endl;
    // cout << "M3 summing2y: " << M3Long[1] << endl;
    // cout << psize << endl;
    double p3x = M3Long[0] / psize;
    double p3y = M3Long[1] / psize;
    if (p3x < 0){
        M3Long[0] = -1*std::pow(p3x*-1.0,0.333333333);
    }
    if (p3y < 0){
        M3Long[1] = -1*std::pow(p3y*-1.0,0.333333333);
    }
    // cout << "M3Long Complete: " << M3Long[0] << endl;
    // cout << "M3Long Complete: " << M3Long[1] << endl;
    return M3Long;
}




