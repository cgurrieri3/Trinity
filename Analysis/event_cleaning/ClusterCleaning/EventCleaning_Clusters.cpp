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


const int TriggeredChannelAmpCutOff = 8; // Cut off for the triggered music channel
const int TimeBinAll = 239; // Difference between triggered pixel time bin and the pixels around it time bin difference more that 1 risk saving cross talk events
const int CorePixelAmpCutOff = 8; // (200ish ADC) // Cut off for the pixels surrounding the triggered music channel
const int SaturatedPixelCutoff = 256; // 0 removed saturated pixels max channels from being cleaned. 1 allows them to be cleaned (changed from 0 on 2/26/2025)
const int FlasherEventsCutOff = 15; // (350 ish ADC ) Average amplitude across the camera ~1200 is  Flasher event
const int PixelSurviveCutOff = 3; //How many pixels need to survive cleaning to plot 
const int LWRatioCutOff = 60 ; // length width How elliptical you require the events to be after the Principle Compomnent Analysis (helps removed crosstalk events.) 
const int rmTopRow = 1 ; // removed the top row of the camera (sky) 0 = remove, 1 = dont remove 

//void EventCleaning_Clusters(std::string folString){
int main(int argc, char **argv){
    if(argc < 1){
		cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    std::string folString = argv[1];
    std::string dirName = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
	cout << dirName << endl;
    std::vector <std::string> fileNamesVec;
	fileNamesVec = read_directory(dirName.c_str());
    fileNamesVec.erase(fileNamesVec.begin(), fileNamesVec.begin() + 2);

    TH2F* hcam_srvedist = new TH2F("hcam_srvedist", "Distribution of # of pixels that surived cleaning over events",20, 0.5,20.5, 10, 0, 1000);
    TH2F* LW_surviving_dist = new TH2F("LW_surviving_dist", "Surviving pixels vs Length Width Ratio; LW ratio; Surviving Pixels",10, -0.05,0.95, 20, 0, 20);
    TH2F* CONC_surviving_dist = new TH2F("CONC_surviving_dist", "Surviving pixels vs Concentation; Conc; Surviving Pixels",10, -0.05,0.95, 20, 0, 20);
    TH2F* LW_CONC = new TH2F("LW_CONC", "LW vs Concentation; LW; CONC",10, -0.05,0.95,10, -0.05,0.95);
    TH2F* SIZE_CONC = new TH2F("SIZE_CONC", "SIZE vs Concentation; CONC; SIZE",10, -0.05,0.95, 100, -0.05,999.95);
    TH1D* hcam_allAmpdist = new TH1D("hcam_allAmpdist", "Distribution of Pes counts of surived pixels", 10, 0, 1000 );
    TH1D* ERhist = new TH1D("ERhist", "Distribution of Length Width Ratio",10, -0.05,0.95);

    COGgraph = new TH2F("COGgraph", "Center of Gravity", 16, -0.5, 15.5, 16, -0.5, 15.5);
    COGgraph->SetTitle("Center of Gravity;Pixel Bin along x axis of Camera;Pixel Bin along y axis of Camera");
    COGgraph->SetMarkerStyle(20);   // Set marker style
    COGgraph->SetMarkerColor(kGreen); // Green color for certain points
    COGgraph->SetLineColor(kGreen);

    COGgraphweighted = (TH2F*)COGgraph->Clone("COGgraphweighted");

    COGgraphweighted->SetTitle("Center of Gravity Weighted");
    COGgraphweighted->SetMarkerColor(kBlue); // Green color for certain points
    COGgraphweighted->SetLineColor(kBlue);

    pixeldist = (TH2F*)COGgraph->Clone("pixeldist");
    pixeldist->SetTitle("Triggered Pixels Of Surviving Events");


    
    // Create a new canvas
    TCanvas* c_cleaned = new TCanvas("CleanedDisplay", "Cleaned CameraPlot", 950, 1000);
    TCanvas* c_PCA = new TCanvas("PCADisplay", "PCE CameraPlot", 950, 1000);
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

    c_PCA->Print("/storage/hive/project/phy-otte/sstepanoff3/exact_output/EventCleaning/PCA_histograms.pdf[");
    

    c_PCA->Divide(2, 2);
    c_cleaned->Divide(2, 2);
    int tEntries = 0;
    int zeroentries = 0;
    int hledentries = 0;

    // Load in the Calibration factors from the HLED branch in the file of CalibrationFactor.cpp
    std::vector<double> CalibratedHLEDperPixel = readFileToVector(Form("%s%s_Calibration_Factor.csv", CalibrationFactorDir.c_str(), folString.c_str()));

    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
    //for(int f = 90; f<100; f++){
        // try {
        if (!isBranchPresentInFile(fileNamesVec[f], "Test")) {
            continue; // Skip to the next branch if not present
        }
        LoadEvents(fileNamesVec[f], "Test");
        LoadEventsHLED(fileNamesVec[f], "HLED");
        ev = new Event();
        evHLED = new Event();
        SetBranches(ev);
        SetBranchesHLED(evHLED);
        
        int nEntries = tree->GetEntries();
        int nEntriesHLED = treeHLED->GetEntries();
        if (nEntries==0) {
            continue;
        }   
        
        std::cout << "Total Number of Events: " << nEntries << std::endl;
        for(int EventCounter = 0; EventCounter < (nEntries+nEntriesHLED); EventCounter++){
            std::vector<double> AmplitudeValues;
            std::vector<int> peakTimeValues;
            std::vector<double> PedestalValues;
            std::vector<vector<int>> traceValues;
            int Saturated_Counter = 0;
            int totalAmpPerEvent = 0;

            // this allow you to have all the info you need from the event in one for loop so you dont have to have one for HLED and one for Test since the events can sometimes cross over.
            if (EventCounter < nEntries) {
                tree->GetEntry(EventCounter);
                //std::cout << "Event Test: "<< EventCounter << "/" << nEntries << std::endl;
                
                getSIABTriggeredInfo(ev, MaxPixelID,MaxAmplitudePixel,MaxMUSICID,MaxPeakTimePixel,AvgAmplitudePerEvent,MaxAmplitudePixelTimeBin,MaxPixelIDTimeBin);
                getPixelInfo(ev,AmplitudeValues,peakTimeValues,PedestalValues,&Saturated_Counter, &totalAmpPerEvent, traceValues);
                
            } else {
                treeHLED->GetEntry(EventCounter-nEntries);
                //std::cout << "Event HLED: "<< EventCounter-nEntries << "/" << nEntriesHLED << std::endl;
                
                getSIABTriggeredInfo(evHLED, MaxPixelID,MaxAmplitudePixel,MaxMUSICID,MaxPeakTimePixel,AvgAmplitudePerEvent,MaxAmplitudePixelTimeBin,MaxPixelIDTimeBin);
                getPixelInfo(evHLED,AmplitudeValues,peakTimeValues,PedestalValues,&Saturated_Counter, &totalAmpPerEvent, traceValues);
                
            }

            // GO through all the events at the HLED and get the ratio for amplitude/median for each pixel before you start the test branch stuff
            tEntries +=1; // keeps track of all the events
            
            std::string filenameTitle = fileNamesVec[f];
            filenameTitle = filenameTitle.substr(97,5);


            // Create histograms 
            TH2F* hcam_intial = new TH2F("hcam_intial", Form("Calibrated Image --File# %i-- Event# %i ", f ,EventCounter), 16, -0.5, 15.5, 16, -0.5, 15.5);
            TH2F* hcam_cut = new TH2F("hcam_cut", Form("Amplitude cut Image --File# %s -- Event# %i ", filenameTitle.c_str() ,EventCounter), 16, -0.5, 15.5, 16, -0.5, 15.5);
            TH2F* hcam_connected= new TH2F("hcam_connected", Form("Connected to Trig Pixel Image --File# %s -- Event# %i ", filenameTitle.c_str() ,EventCounter), 16, -0.5, 15.5, 16, -0.5, 15.5);
            TH2F* hcam_cleaned= new TH2F("hcam_cleaned", Form("Cleaned Image --File# %s -- Event# %i ", filenameTitle.c_str() ,EventCounter), 16, -0.5, 15.5, 16, -0.5, 15.5);
            
            std::vector<double> AmpCameraTimeBin;
            //std::vector<vector<int>> TempAlltimebins;
            //TempAlltimebins(ev->GetSignalValue());
            for(int j = 0; j<MaxNofChannels; j++){

                double ampattimebin = PedestalValues[j] - convertADC2PE(traceValues[j][TimeBinAll]); // no absoulte value as it allows wrong direction amplitude typical in the lower left quadrent
                //totalAmpPerEvent+= ampattimebin;

                AmpCameraTimeBin.push_back(ampattimebin/CalibratedHLEDperPixel[j]);
                int nx, ny;
                FindBin(j, &nx, &ny);
                hcam_intial->SetBinContent(nx + 1, ny + 1, AmpCameraTimeBin[j]);
                // cout << "Amp at max time bin: " << ampattimebin << endl;
            }

            // Start the cleanning Process
            // Make sure that the camera does not have any saturation or is an HLED event

            

            if (AvgAmplitudePerEvent.back() < FlasherEventsCutOff && AmpCameraTimeBin[MaxPixelIDTimeBin.back()] > TriggeredChannelAmpCutOff && Saturated_Counter <= SaturatedPixelCutoff && checkTopRow(MaxPixelIDTimeBin.back()) <= rmTopRow){
                //cout<< "entered Cleaning" << endl;
                int pixel_count = 0;
                std::vector<double> selected_pixels;
                std::vector<int> selected_pixels_ID;
                for(int j = 0; j<MaxNofChannels; j++){
                    // First Amplitude cut across the whole camera regardless of the triggered pixel 
                    int nx, ny;
                    FindBin(j, &nx, &ny);
                    hcam_intial->SetBinContent(nx + 1, ny + 1, AmpCameraTimeBin[j]);
                    if (AmpCameraTimeBin[j] >= CorePixelAmpCutOff) { 
                        pixel_count +=1;
                        selected_pixels.push_back(AmpCameraTimeBin[j]);
                        selected_pixels_ID.push_back(j);
                        hcam_cut->SetBinContent(nx + 1, ny + 1, AmpCameraTimeBin[j]);
                    }
                }
                //cout << "size of selected_pixels first screening: " << selected_pixels.size() << endl; 
                if (pixel_count >= PixelSurviveCutOff){
                    //cout << "pixel_count "<< pixel_count << endl;
                    
                    // now we need to first check the core pixel in all nearest neighbors - surrounding 8 
                    // Then we need to check broad side pixels only 
                    std::vector<double> surviving_pixels;
                    TriggeredPixelNeighborhoodChecker(selected_pixels, selected_pixels_ID,MaxPixelIDTimeBin.back(),surviving_pixels);
                    //cout << "surviving_pixels: "<<surviving_pixels.size() << endl;
                    
                    std::vector<int> surviving_pixels_PeakTime;
                    int local_peak_time = 400;
                    double TotalAmplitudeSurviving = 0;
                    removeDuplicates(surviving_pixels);
                    //set the bin content from the cleaned pixels
                    for(std::vector<int>::size_type j = 0; j<surviving_pixels.size(); j++){
                        int nx, ny;
                        FindBin(surviving_pixels[j], &nx, &ny);
                        TotalAmplitudeSurviving += AmpCameraTimeBin[surviving_pixels[j]];
                        hcam_connected->SetBinContent(nx + 1, ny + 1, AmpCameraTimeBin[surviving_pixels[j]]);


                        surviving_pixels_PeakTime.push_back(peakTimeValues[surviving_pixels[j]]);
                        if (local_peak_time > peakTimeValues[surviving_pixels[j]]){
                            local_peak_time = peakTimeValues[surviving_pixels[j]];

                        }
                    }

                    if(surviving_pixels.size() >= PixelSurviveCutOff){

                        std::vector<int> surviving_pixels_Cleaned;
                        std::vector<double> surviving_pixels_amp_Cleaned;
                        for (std::vector<int>::size_type i =0; i<surviving_pixels.size();++i){
                            if (surviving_pixels_PeakTime[i]==local_peak_time){
                                surviving_pixels_Cleaned.push_back(surviving_pixels[i]);
                            }
                        }
                        std::vector<double> surviving_pixels_final;
                        TriggeredPixelNeighborhoodChecker(surviving_pixels_amp_Cleaned, surviving_pixels_Cleaned,MaxPixelIDTimeBin.back(),surviving_pixels_final);
                        removeDuplicates(surviving_pixels_final);
                        int Cleaned_count=0;
                        double Cleaned_total_amp=0;
                        for (std::vector<int>::size_type i =0; i < surviving_pixels_final.size();i++){
                            // cout << "Pixel: " << surviving_pixels_final[i] << " Pixel Peak Time: " << local_peak_time << " local min time: " << local_peak_time << endl;
                            
                            Cleaned_count +=1;
                            Cleaned_total_amp += AmpCameraTimeBin[surviving_pixels_final[i]];
                            int nx, ny;
                            FindBin(surviving_pixels_final[i], &nx, &ny);
                            hcam_cleaned->SetBinContent(nx + 1, ny + 1, AmpCameraTimeBin[surviving_pixels_final[i]]);
        
                        }

                        if(Cleaned_count >= PixelSurviveCutOff){
                            TVectorD eigenVals;
                            TMatrixD eigenVecs; 
                            eigenVals.ResizeTo(2);
                            eigenVecs.ResizeTo(2,2);
                            std::vector<double> sigmas;
                            std::vector<double> CenterOfGravity(2, 0.0);
                            
                            //Create the PCA
                            PCA pca(2);
                            // do the Principle componet analysis on the cleaned data
                            LoadDataPCA(pca,hcam_cleaned,Cleaned_total_amp,&CenterOfGravity);

                            // Perform PCA
                            // //pca.ComputeCovarianceMatrix();
                            pca.ComputeEigenDecomposition();

                            // Print eigenvalues
                            eigenVals = pca.GetEigenValues();
                            // eigenVals->assign(EValues.GetMatrixArray(), EValues.GetMatrixArray() + EValues.GetNrows());
                            // std::cout << "Eigenvalues:\n";
                            // eigenVals.Print();
                            
                            // Print eigenvectors
                            eigenVecs = pca.GetEigenVectors();
                            // eigenVecs->resize(2, std::vector<double>(2));  // Resize for 2x2 matrix
                            // for (int i = 0; i < 2; ++i)
                            //     for (int j = 0; j < 2; ++j)
                            //         (*eigenVecs)[i][j] = EVectors(i, j);
                            // std::cout << "Eigenvectors:\n";
                            // eigenVecs.Print();

                            // // Get and print sigmas
                            sigmas = pca.GetSigmas();                            

                            double r1 = sqrt(eigenVals[0]);
                            double r2 = sqrt(eigenVals[1]);
                            double EllipicRatio = r2/r1;
                            // cout << "LW " << r1 << "," << r2 << ", ratio:" <<  EllipicRatio << endl;
                            //sleep(15);
                            // if (std::find(surviving_pixels_final.begin(), surviving_pixels_final.end(), MaxPixelIDTimeBin.back()) != surviving_pixels_final.end()) { 
                            //     cout << "HERE" << endl;
                            // }
                            // sleep(15);
                            if (EllipicRatio <=  LWRatioCutOff/100.0 && std::find(surviving_pixels_final.begin(), surviving_pixels_final.end(), MaxPixelIDTimeBin.back()) != surviving_pixels_final.end() ){
                                double meanx1 = hcam_cleaned->GetMean(1);
                                double meany1 = hcam_cleaned->GetMean(2);
                                COGgraph->Fill(meanx1,meany1);

                                std::vector<double> M3LongVar = getM3Long(meanx1, meany1, surviving_pixels_final,AmpCameraTimeBin);
                                // cout << "M3Long outside: " << M3LongVar[0] << endl;
                                // cout << "M3Long outside: " << M3LongVar[1] << endl;

                                double conc = AmpCameraTimeBin[MaxPixelIDTimeBin.back()]/Cleaned_total_amp;
                                CleanedPlot(c_cleaned, hcam_intial, hcam_cut,hcam_connected, hcam_cleaned, AvgAmplitudePerEvent.back(), MaxPixelIDTimeBin.back(),MaxMUSICID.back(), conc, eigenVals, eigenVecs, sigmas, Cleaned_count, Cleaned_total_amp, M3LongVar);
                                // sleep(60);
                        

                                std::string hcam_cleanedtitle=Form("File# %i, Event# %i",f,EventCounter);
                                hcam_cleaned->SetName(hcam_cleanedtitle.c_str());
                                savePlot(c_cleaned,hcam_cleaned,outDir,folString, file,hcam_cleanedtitle );
                                ERhist->Fill(EllipicRatio);
                                
                                hcam_srvedist->Fill(Cleaned_count, Cleaned_total_amp); 
                                LW_surviving_dist->Fill(EllipicRatio,Cleaned_count);
                                CONC_surviving_dist->Fill(conc,Cleaned_count);
                                LW_CONC->Fill(EllipicRatio,conc);
                                SIZE_CONC->Fill(conc,Cleaned_total_amp);
                                hcam_allAmpdist->Fill(Cleaned_total_amp);
                                
                                int nx, ny;
                                FindBin(MaxPixelIDTimeBin.back(), &nx, &ny);
                                pixeldist->Fill(nx, ny, 1);
                                sleep(15);
                            }
                        } else {
                        zeroentries += 1;
                        }
                    } else {
                    zeroentries += 1;
                    }
                } else {
                zeroentries += 1;
                }

            } else if (AvgAmplitudePerEvent.back() >= FlasherEventsCutOff){
                //cout << "HLED entry: " << hledentries <<endl;
                hledentries += 1;
            } else {
                //cout << "zeroentries: " <<zeroentries << "Amplitude: " << AvgAmplitudePerEvent.back() << endl;
                zeroentries += 1;
            }
            delete hcam_intial;
            delete hcam_cut;
            delete hcam_connected;
            delete hcam_cleaned;


        }
        delete fO;
        delete tree;
        delete treeHLED;
        delete ev;
        delete evHLED;
        // } catch (const std::exception& e){
        //     std::cerr << "Error: " << e.what() << std::endl;
        //     continue;
        // }

    }
    c_PCA->Print("/storage/hive/project/phy-otte/sstepanoff3/exact_output/EventCleaning/PCA_histograms.pdf]");
    c_cleaned->SetLeftMargin(0.15);  // Increase left margin
    c_cleaned->SetRightMargin(0.15);
    // c_cleaned->SetBottomMargin(0.25); // Increase bottom margin
    //c_cleaned->cd(1);
    c_cleaned->cd(0);
    hcam_srvedist->SetStats(0);
    hcam_srvedist->SetXTitle("Number of Pixels Survived Cleaning");
    hcam_srvedist->SetYTitle("SIZE (PEs)");
    hcam_srvedist->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    hcam_srvedist->GetYaxis()->SetTitleOffset(2.0); // Adjust Y-axis title offset
    int entries = hcam_srvedist->GetEntries();


    hcam_srvedist->Draw("COLZ");

    // Create a TLatex object to display the label
    TLatex *latex = new TLatex();
    latex->SetNDC(); // Use normalized coordinates
    latex->SetTextSize(0.02); // Set the text size


    latex->DrawLatex(0.3, 0.85, Form("Events #: %i", tEntries-hledentries));
    latex->DrawLatex(0.3, 0.83, Form("Flasher Events # Identified: %i", hledentries));
    latex->DrawLatex(0.3, 0.81, Form("Events # Survived: %i",entries));
    
    savePlot(c_cleaned,hcam_srvedist,outDir,folString,file, "Number_Size");
    c_cleaned->Clear();

    delete hcam_srvedist;
    delete latex;
    // c_cleaned->SetLeftMargin(0.25);  // Increase left margin
    // c_cleaned->SetRightMargin(0.25);
    // c_cleaned->SetBottomMargin(0.25); // Increase bottom margin
    c_cleaned->cd(0);
    hcam_allAmpdist->SetStats(0);
    hcam_allAmpdist->SetXTitle("SIZE (PEs)");
    hcam_allAmpdist->GetXaxis()->SetLabelSize(0.03);
    hcam_allAmpdist->SetYTitle("Instances of Total Amplitude ");
    hcam_allAmpdist->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    hcam_allAmpdist->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset

    hcam_allAmpdist->Draw();

    savePlot(c_cleaned,hcam_allAmpdist,outDir,folString, file, "Size instance");
    delete hcam_allAmpdist;


    c_cleaned->cd(0);
    ERhist->SetStats(0);
    ERhist->SetXTitle("Length Width Ratio");
    ERhist->GetXaxis()->SetLabelSize(0.03);
    //ERhist->SetYTitle(" ");
    ERhist->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    ERhist->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    ERhist->Draw();

    savePlot(c_cleaned,ERhist,outDir,folString,file, "ERhist");
    delete ERhist;

    c_cleaned->cd(0);
    LW_surviving_dist->SetStats(0);
    LW_surviving_dist->GetXaxis()->SetLabelSize(0.03);
    LW_surviving_dist->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    LW_surviving_dist->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    LW_surviving_dist->Draw("COLZ");

    savePlot(c_cleaned,LW_surviving_dist,outDir,folString,file, "LW_surviving_dist");
    delete LW_surviving_dist;


    c_cleaned->cd(0);
    LW_CONC->SetStats(0);
    LW_CONC->GetXaxis()->SetLabelSize(0.03);
    LW_CONC->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    LW_CONC->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    LW_CONC->Draw("COLZ");

    savePlot(c_cleaned, LW_CONC,outDir,folString,file, "LW_CONC_dist");
    delete LW_CONC;

    c_cleaned->cd(0);
    SIZE_CONC->SetStats(0);
    SIZE_CONC->GetXaxis()->SetLabelSize(0.03);
    SIZE_CONC->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    SIZE_CONC->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    SIZE_CONC->Draw("COLZ");

    savePlot(c_cleaned,SIZE_CONC,outDir,folString,file, "SIZE_CONC_dist");
    delete SIZE_CONC;

    c_cleaned->cd(0);
    CONC_surviving_dist->SetStats(0);
    CONC_surviving_dist->GetXaxis()->SetLabelSize(0.03);
    CONC_surviving_dist->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    CONC_surviving_dist->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    CONC_surviving_dist->Draw("COLZ");

    savePlot(c_cleaned,CONC_surviving_dist,outDir,folString,file, "CONC_surviving_dist");
    delete CONC_surviving_dist;

    c_cleaned->cd(0);
    //COGgraph->SetStats(0);
    // COGgraph->SetXTitle("X position Camera");
    // COGgraph->GetXaxis()->SetLabelSize(0.03);
    //ERhist->SetYTitle(" ");
    COGgraph->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    COGgraph->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    COGgraph->Draw("SCAT");  // "A" for axes, "P" for points
    // COGgraphweighted->Draw("SCAT SAME");  // "SAME" to overlay on the same canvas

    // TLegend *legend = new TLegend(0.7, 0.8, 0.9, 0.9);  // Position of the legend
    // legend->AddEntry(COGgraph, "Center of Gravity Mean", "p");  // "l" stands for line
    // // legend->AddEntry(COGgraphweighted, "Center Of Gravity Weighted", "p");

    // // Draw the legend
    // legend->Draw();
    DrawMUSICBoundaries();
    savePlot(c_cleaned,COGgraph,outDir,folString,file, "COG");
    delete COGgraph;
    delete COGgraphweighted;

    c_cleaned->cd(0); 
    pixeldist->SetStats(0);
    pixeldist->GetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    pixeldist->GetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    pixeldist->Draw("COLZ");  // "A" for axes, "P" for points
    DrawMUSICBoundaries();
    savePlot(c_cleaned,pixeldist,outDir,folString,file, "pixeldist");
    delete pixeldist;

    
    c_cleaned->Print(Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf]",
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

    delete c_cleaned;
    system(Form("chmod 660 %s",Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
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
        )));
    system(Form("chmod 660 %s",Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf",
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
        )));
return 0;
}

/***
Start of Functions

***/

void NeighborhoodCheckerHelper(int ID, std::vector<int>& IDSelected, std::vector<double> AmplitudesSelected,std::vector<double>& cleaned_pixels, std::vector<int>& visited_ids) {
    bool already_searched = std::find(visited_ids.begin(), visited_ids.end(), ID) != visited_ids.end();
    visited_ids.push_back(ID);
    if (already_searched == false) {
        for(std::vector<int>::size_type i=0; i < IDSelected.size(); i++){
            //cout << "ID: " << ID << " IDSelected: " << IDSelected[i] << endl;

            if (ID == IDSelected[i] ){
                cleaned_pixels.push_back(IDSelected[i]);

                // cout << "ID selected: " << ID << endl;
                // check recursilely the board side of these pixels
                // Removes the ID that has been seleceted in the array so that the loops end
                

                for (int arrayID: GetNeighborArray(ID,Form("%sneighborbroadside.csv",neighborDir.c_str()))) {
                    // cout << "ArrayID: NCH: " << arrayID << endl;
                    NeighborhoodCheckerHelper(arrayID,IDSelected,AmplitudesSelected,cleaned_pixels,visited_ids);
                }
            }

        }
    }
    //cout << ID <<", ";
}


void TriggeredPixelNeighborhoodChecker(std::vector<double> AmplitudesSelected,std::vector<int>& IDSelected, int triggeredpixel,std::vector<double>& cleaned_pixels){
    // we already know that the triggered pixel has a high enough amplitude 
    // We have a list of amplitudes that survied the first part of the the cleaning the straight amplitude cut across the whole camera
    // that will have 256 pixels and if it is to low the pixel will be 0 otherwise it will have the correct amplitude value if it meets the amplitude requirement
    //IDSelected.erase(std::remove(IDSelected.begin(), IDSelected.end(), triggeredpixel), IDSelected.end());
    
    std::vector<int> visited_ids;
    // get the Trigger pixels neighbors
    cleaned_pixels.push_back(triggeredpixel);
    for (int arrayID : GetNeighborArray(triggeredpixel,Form("%sneighborbroadside.csv",neighborDir.c_str()))) {
        // cout << "arrayID: Triggered() " << arrayID << endl;
        NeighborhoodCheckerHelper(arrayID,IDSelected,AmplitudesSelected,cleaned_pixels,visited_ids);
        // cout << "completed ID: "<< endl;
    }



}


void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp, std::vector<double> *COG){
    


    (*COG)[0] = 0.0;
    (*COG)[1] = 0.0;
   

    // Loop over the bins of the histogram and extract the bin content (data points)
    for (int bin = 0; bin <= MaxNofChannels; bin++) {
       
        int nx, ny;
        FindBin(bin, &nx, &ny);
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

            (*COG)[0] += (nx * (weight/totalAmp));
            (*COG)[1] += (ny * (weight/totalAmp));
        }
        
    }

    // // Perform PCA
    // //pca.ComputeCovarianceMatrix();
    // pca.ComputeEigenDecomposition();

    // // Print eigenvalues
    // EValues = pca.GetEigenValues();
    // eigenVals->assign(EValues.GetMatrixArray(), EValues.GetMatrixArray() + EValues.GetNrows());
    // // std::cout << "Eigenvalues:\n";
    // // eigenVals.Print();
    
    // // Print eigenvectors
    // EVectors = pca.GetEigenVectors();
    // eigenVecs->resize(2, std::vector<double>(2));  // Resize for 2x2 matrix
    // for (int i = 0; i < 2; ++i)
    //     for (int j = 0; j < 2; ++j)
    //         (*eigenVecs)[i][j] = EVectors(i, j);
    // // std::cout << "Eigenvectors:\n";
    // // eigenVecs.Print();

    
    // // Get and print sigmas
    // sigmasVec = pca.GetSigmas();
    
    // sleep(5);
}

std::vector<double> getM3Long(double xcog,double ycog, std::vector<double> sur_pix, std::vector<double> amps){
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
        FindBin(sur_pix[p], &nx, &ny);
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

std::vector<double> generateRandomNumbers() {
    // Define the random number generator and distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.49, 0.49);

    // Generate two random numbers and return them as a pair
    return {dist(gen), dist(gen)};
}

void getPixelInfo(Event *event, std::vector<double>& AmplitudeValues, std::vector<int>& peakTimeValues, std::vector<double>& PedestalValues, int* SC, int* totalAPE, std::vector<std::vector<int>>& trace){
    ExtractedData *extractedDataHLED[MaxNofChannels];
    for (int i = 0; i<MaxNofChannels; i++){
        extractedDataHLED[i] = 0;
        extractedDataHLED[i] = new ExtractedData();
    }

    trace = event->GetSignalValue();
    Pulse *pulse;
    
    for(int j = 0; j<MaxNofChannels; j++){
        pulse = new Pulse(event->GetSignalValue(j));
        extractedDataHLED[j]->SetAmplitude(convertADC2PE(pulse->GetAmplitude()));
        //cout << "Pixel: " << j << " Content: "<< extractedDataHLED[j]->GetAmplitude()<< endl;
        AmplitudeValues.push_back(extractedDataHLED[j]->GetAmplitude());
        peakTimeValues.push_back(pulse->GetTimePeak());
        double pedestalvaluesTEMP = convertADC2PE(pulse->GetPedestal());
        PedestalValues.push_back(pedestalvaluesTEMP);
        // cout<< "AMPLITUDE :"<< extractedDataHLED[j]->GetAmplitude() << endl;
        // cout<< "PED :"<< pedestalvaluesTEMP << endl;
        *totalAPE+= extractedDataHLED[j]->GetAmplitude();
        double ADCvalue = abs((extractedDataHLED[j]->GetAmplitude())-pedestalvaluesTEMP);
        //cout << extractedDataHLED[j]->GetAmplitude(); << endl;
        if (ADCvalue < 0.5){
            *SC+=1;
            //cout << "ADCValue" << ADCvalue << endl;
        }
        delete pulse;
    }
    // Deallocate each ExtractedData object
    for (int i = 0; i < MaxNofChannels; ++i) {
        delete extractedDataHLED[i];
        extractedDataHLED[i] = nullptr; // Optional: set the pointer to nullptr after deleting
    }
    
}

// Finds the music pair so that you search the whole siab instead of just one music since it seems like their is a delay
int findMUSICPair(int MUSICID){
    
    int MUSICIDPAIR;
    if (MUSICID %2 != 0){
        MUSICIDPAIR = MUSICID-1;
        return MUSICIDPAIR;

    } else if (MUSICID %2 == 0){
        MUSICIDPAIR = MUSICID +1;
        return MUSICIDPAIR;

    } else {
        return -1;
    }
}

/**
 * Given the ID of the first triggered music chip, returns a vector containing the sipm ID for each pixel
 * in the MUSIC.
 */
vector<int> FindNeighborPixels(int FirstTrigMusic)
{
    vector<int> vNeighbor;
    int FirstTrigPix = findMUSICIndex(FirstTrigMusic) * 8;
    // std::cout <<  "Pixel location: " <<  findMUSICIndex(FirstTrigMusic) << endl;

    for (int i=0; i<8; i++){
        vNeighbor.push_back(FirstTrigPix+i);
    }
    
    return vNeighbor;
}

// Function to find MUSIC index of an element in the array
int findMUSICIndex(int MUSIC) {

    std::vector<int> MUSICs = {3,2,11,10,19,18,27,26,7,6,15,14,23,22,31,30,1,0,9,8,17,16,25,24,5,4,13,12,21,20,29,28};
    // cout << "This is the value to multiple by 8: " << MUSICs[MUSIC] << " at this music: "<< MUSIC << endl;
    return MUSICs[MUSIC];

}


void getSIABTriggeredInfo(Event *event,std::vector<int>& Max_pixel, std::vector<double>& Max_Amp, std::vector<int>& Max_MUSIC, std::vector<int>& peak_time,std::vector<double>& Avg_Amp, std::vector<double>& Max_Amp_Time_Bin, std::vector<int>& Max_pixel_Time_Bin){ 
    std::vector<Int_t> newTrigPix; // vector for storing triggered pixels
	// std::vector<int> amplitude;
    
    //int maxAmplitudeIndexCount = 0;
    
    
	// Create vectors to store max amplitude and index of the pixel with max amplitude values and corresponding channel indices
    std::vector<double> maxAmplitudes;
    std::vector<int> maxAmplitudeIndices; 
    std::vector<float> maxpeaktime;
    std::vector<float> maxAmplitudeTimeBin;
    
    
	// std::vector<Int_t> TrigMus; // vector for storing triggered MUSICs
    // TrigMus = event->GetROIMusicID();
    // std::cout << "TrigMus size: " <<  << std::endl;
    // std::cout << "Triggered MUSIC: " << TrigMus[0] << "," << TrigMus[1]<< std::endl;

    newTrigPix = FindNeighborPixels((event->GetROIMusicID())[0]);
    
    
    // std::cout << "newTrigPix size: " << newTrigPix.size()<< std::endl;
    // std::cout << "First pixel of the triggered MUSIC: " << newTrigPix[0]<< std::endl;
    Pulse *pulse;
    for (size_t k = 0; k < newTrigPix.size(); k++) {
        //std::cout<<newTrigPix[k] << std::endl;
    
        pulse = new Pulse(ev->GetSignalValue(newTrigPix[k]));
        double ped =convertADC2PE(pulse->GetPedestal());
        std::vector<int> trace;     
        if (newTrigPix[k] >= 0 && newTrigPix[k] <= MaxNofChannels) {            	
            maxAmplitudes.push_back(convertADC2PE(pulse->GetAmplitude()));
            // cout << "before ADC: " << pulse->GetAmplitude() <<" converted " << convertADC2PE(pulse->GetAmplitude()) << endl;
            maxpeaktime.push_back(pulse->GetTimePeak());
            trace = event->GetSignalValue(newTrigPix[k]);
            maxAmplitudeTimeBin.push_back(ped - convertADC2PE(trace[TimeBinAll]));

            maxAmplitudeIndices.push_back(newTrigPix[k]); 
            //cout << "ID: : "<<newTrigPix[k] << " trace value " << trace[TimeBinAll] << endl;
        }
        else {
            cout << "ROIPixelID is out of range, skipping calculation." << endl;
        }
        delete pulse;
    }

    int triggeredpixelindex = std::distance(maxAmplitudes.begin(), std::max_element(maxAmplitudes.begin(), maxAmplitudes.end()));
    int triggeredpixelindexTimeBin = std::distance(maxAmplitudeTimeBin.begin(), std::max_element(maxAmplitudeTimeBin.begin(), maxAmplitudeTimeBin.end()));
    double maxVal = maxAmplitudes[triggeredpixelindex];
    // cout << "MAX VALUE: "<< maxVal << endl; 
    int maxTime = maxpeaktime[triggeredpixelindex];
    double maxValTimeBin =  maxAmplitudeTimeBin[triggeredpixelindexTimeBin];
    //cout << "index: " << triggeredpixelindexTimeBin << endl; 
    //cout << "Pixel: " <<  newTrigPix[triggeredpixelindexTimeBin] << endl;
    
    double AmpCount = 0;
    std::vector<vector<int>> trace = event->GetSignalValue();
    for (int k = 0; k < MaxNofChannels; k++) {
        pulse = new Pulse(event->GetSignalValue(k));
        AmpCount += convertADC2PE(pulse->GetAmplitude());

        if (k == newTrigPix[triggeredpixelindex]) {
                Max_Amp.push_back(maxVal);
                Max_pixel.push_back(k);
                Max_MUSIC.push_back((event->GetROIMusicID())[0]); // use with caution this only gets the ROI music ID
                peak_time.push_back(maxTime);
        } 
        if (k == maxAmplitudeIndices[triggeredpixelindexTimeBin]){
                // std::cout << "Triggered MUSIC: " << (event->GetROIMusicID())[0] <<" Pixel: "  << k << ", Amplitude " << maxValTimeBin<<std::endl;
                Max_Amp_Time_Bin.push_back(maxValTimeBin);
                Max_pixel_Time_Bin.push_back(k);
        }
        delete pulse;
    }
    Avg_Amp.push_back(AmpCount/256);

}


std::vector<double> readFileToVector(const std::string& filename) {
    std::vector<double> result;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return result; // Return empty vector if file can't be opened
    }

    double value;
    while (file >> value) {
        result.push_back(value);
    }

    file.close();
    return result;
}

void savePlot(TCanvas* c_cleaned,TH1* hist,std::string outDir,std::string folString, TFile* file,  std::string plotname){
     // Write the canvas to the file
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

    std::string histName = hist->GetName();
    std::string txtName = histName + "parameters";
    c_cleaned->Write(plotname.c_str());  // Save the canvas with a specific name
     if (TH2* hist2F = dynamic_cast<TH2*>(hist)) {
        histName = histName + "TH2";
    }   
    // Check if it's a TH1 (1D)
    else if (TH1* hist1D = dynamic_cast<TH1*>(hist)) {
        histName = histName + "TH1";

    } 
    hist->Write(histName.c_str());
    
    // get and write the parameters to a text file
    std::ifstream inputFile("output_variables.txt");
    if (inputFile.is_open()) {
        std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        inputFile.close();
        // Create a TObjString to store the content
        TObjString* textContent = new TObjString(fileContent.c_str());
        
        textContent->Write(txtName.c_str(), TObject::kOverwrite);

        delete textContent;
    }
    file->Close();

    // Delete the text file after use
    if (std::remove("output_variables.txt") != 0) {
        std::cerr << "Error deleting output_variables.txt" << std::endl;
    } else {
        std::cout << "output_variables.txt successfully deleted." << std::endl;
    }

    //file->Write(plotname.c_str());
    c_cleaned->Print(Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf",
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


}
 
//pixmeans is vector of vectors of 256 pixels, pixMeans[1] is the average HLED amplitude per pixel divided by median of all 256 pixels
 
Double_t Median(vector<int> v)
{
    //Size of vector
    int n = v.size();
    //Make temp copy of the vector to leave original in the same order
    std::vector<int> tempV(v);
    //Sort the vector
    sort(tempV.begin(), tempV.end());
    //Check if the number of elements is odd
    if(n%2!=0){
        return(Double_t)tempV[n/2];
    }
    //If the number of elements is even, return the average of the two middle elements
    return(Double_t)(tempV[(n-1)/2]+tempV[n/2])/2.0;
}
 
 
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Function to plot hcam using data from a CSV file
void CleanedPlot(TCanvas* c_cleaned, TH2F* hcam1, TH2F* hcam2, TH2F* hcam3, TH2F* hcam4, double avg_amp,int maxpixelnumberTimeBin, int maxMUSICnumber, double conc, TVectorD eigenVals, TMatrixD eigenVecs, std::vector<double> sigmas, int Cleaned_count, double Cleaned_total_amp,std::vector<double> M3LongVar) {
   
    // Plot hcam using the data from the CSV file
    c_cleaned->cd(1);
    gPad->SetLeftMargin(0.15);  // Increase left margin
    gPad->SetBottomMargin(0.25); // Increase bottom margin
    gPad->SetRightMargin(0.15);
    
    hcam1->SetStats(0);
    hcam1->GetXaxis()->SetLabelSize(0.03);
    hcam1->GetYaxis()->SetLabelSize(0.03);
    hcam1->SetXTitle("Pixel Bin along x axis of Camera");
    hcam1->SetYTitle("Pixel Bin along y axis of Camera");
    hcam1->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    hcam1->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    hcam1->Draw("colz");
    // Add a subtitle using TLatex
    TLatex *subtitle = new TLatex();
    subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
    subtitle->SetTextSize(0.03);
    subtitle->DrawLatex(0.1, 0.92, Form("Average Amplitude Whole Camera: %.2f",avg_amp));
    subtitle->DrawLatex(0.35, 0.12, Form("Triggered MUSIC: %i  Triggered Pixel: %i",maxMUSICnumber,maxpixelnumberTimeBin));
    DrawMUSICBoundaries();

    //hcam1->SetMinimum(0);
    //delete subtitle;
    
    //gPad->Modified();
    //gPad->Update();

     // Plot additional data using the selectedPixelIDs and selectedAmplitudeValues
    c_cleaned->cd(2);
    gPad->SetLeftMargin(0.15);  // Increase left margin
    gPad->SetBottomMargin(0.25); // Increase bottom margin
    gPad->SetRightMargin(0.15);
    hcam2->SetStats(0);
    hcam2->GetXaxis()->SetLabelSize(0.03);
    hcam2->GetYaxis()->SetLabelSize(0.03);
    hcam2->SetXTitle("Pixel Bin along x axis of Camera");
    hcam2->SetYTitle("Pixel Bin along y axis of Camera");
    hcam2->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    hcam2->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    hcam2->SetMinimum(CorePixelAmpCutOff);
    hcam2->Draw("colz");
    DrawMUSICBoundaries();
   

   	c_cleaned->cd(3);
    gPad->SetLeftMargin(0.15);  // Increase left margin
    gPad->SetBottomMargin(0.25); // Increase bottom margin
    gPad->SetRightMargin(0.15);
    hcam3->SetStats(0);
    hcam3->GetXaxis()->SetLabelSize(0.03);
    hcam3->GetYaxis()->SetLabelSize(0.03);
    hcam3->SetXTitle("Pixel Bin along x axis of Camera");
    hcam3->SetYTitle("Pixel Bin along y axis of Camera");
    hcam3->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    hcam3->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    hcam3->SetMinimum();
    hcam3->Draw("colz");
    DrawMUSICBoundaries();


    
   	c_cleaned->cd(4);
    //c_cleaned->cd(4)->Range(0,0,1,1);
    gPad->SetLeftMargin(0.15);  // Increase left margin
    gPad->SetBottomMargin(0.25); // Increase bottom margin
    gPad->SetRightMargin(0.15);
    hcam4->SetStats(0);
    hcam4->GetXaxis()->SetLabelSize(0.03);
    hcam4->GetYaxis()->SetLabelSize(0.03);
    hcam4->SetXTitle("Pixel Bin along x axis of Camera");
    hcam4->SetYTitle("Pixel Bin along y axis of Camera");
    hcam4->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    hcam4->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    hcam4->SetMinimum();
    hcam4->Draw("colz");
    DrawMUSICBoundaries();
    // hcam4->GetXaxis()->SetRangeUser(-5, 16.6);  // Set wider range for X-axis
    // hcam4->GetYaxis()->SetRangeUser(-5, 16.6);  // Set wider range for Y-axis

    double meanx = hcam4->GetMean(1);
    double meany = hcam4->GetMean(2);
    cout << "MEAN " << meanx << "," << meany << endl;
    // meanx = COG[0];
    // meany = COG[1];
    // cout << "MEAN " << meanx << "," << meany << endl;
    
    
    // gets the eigenvalues to sign for the eigenvalue posistion 
    // Eigenvec (E1 is eigenvalue)
    //   E1  E2
    // x E1x E2x
    // y E1y E2y
    //
    // Gets the angle between the eigenvectors of the first principle
    // cout << "P1:" << eigenVecs[0][0] << "p2:" << eigenVecs[1][0] << "tan it: " << atan( eigenVecs[1][0]/eigenVecs[0][0]) << endl;
    double angledeg = atan(eigenVecs[1][0]/eigenVecs[0][0])*(180.0/3.141592653589793238463);
    double anglerad = atan(eigenVecs[1][0]/eigenVecs[0][0]);
    // cout << "ANGLE: "<< anglerad << ", " << angledeg << endl;
    
    // get the size of the major and minor axis of the ellipse / Length and Width of the shower
    double r1 = sqrt(eigenVals[0]);
    double r2 = sqrt(eigenVals[1]);
        
    TArrow* arrow = new TArrow(meanx - 2*(r1*cos(anglerad)), meany - 2*(r1*sin(anglerad)), meanx + 2*(r1*cos(anglerad)), meany + 2*(r1*sin(anglerad)), 0.01, "|"); // "|>" option gives an arrowhead
    TArrow* arrow1 = new TArrow(meanx - 2*(r2*cos(anglerad+1.5708)), meany - 2*(r2*sin(anglerad+1.5708)), meanx + 2*(r2*cos(anglerad+1.5708)), meany + 2*(r2*sin(anglerad+1.5708)), 0.01, "|"); // "|>" option gives an arrowhead
    
    arrow->SetLineColor(kRed); // Optional: Set the color of the arrow
    //arrow->SetLineWidth(1);    // Optional: Set the width of the arrow
    arrow->Draw("SAME");             // Draw the arrow on the same canvas
    //c_cleaned->Update();
    arrow1->SetLineColor(kBlue); // Optional: Set the color of the arrow
    //arrow1->SetLineWidth(1);    // Optional: Set the width of the arrow
    arrow1->Draw("SAME");             // Draw the arrow on the same canvas


    double EllipicRatio = r2/r1;

    TEllipse* ell = new TEllipse(meanx, meany,r1,r2,0,360,angledeg);
    ell->SetFillColorAlpha(kGreen,0.00);
    ell->Draw("SAME");
    //TLatex *subtitle = new TLatex();
    subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
    subtitle->SetTextSize(0.03);
    
    double areaEllipse = (3.8/16) * (3.8/16) * (Cleaned_count); // area of an ellipse
    subtitle->DrawLatex(0.01, 0.92, Form("LW:%.2f Pixels:%i Area:%.2f Size:%.2f Conc:%.2f M3Long:(%.2f,  %.2f)",
    EllipicRatio,
    Cleaned_count,
    areaEllipse,
    Cleaned_total_amp,
    conc,
    M3LongVar[0],
    M3LongVar[1]));

    // Open a text file to write variables
    std::ofstream outputFile("output_variables.txt");
    if (outputFile.is_open()) {
        outputFile << "LWRatio: " << EllipicRatio << "\n";
        outputFile << "Cleaned_count: " << Cleaned_count << "\n";
        outputFile << "AreaEllipse: " << areaEllipse << "\n";
        outputFile << "Cleaned_total_amp: " << Cleaned_total_amp << "\n";
        outputFile << "Conc: " << conc << "\n";
        outputFile << "M3LongVar: (" << M3LongVar[0] << ", " << M3LongVar[1] << ")\n";
        outputFile.close();
    } else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }


}

// Function to open a lookup table for the neighbors of each pixel ////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> GetNeighborArray(int id, std::string filename) {

	// give name of lookup table file
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open the neighbor.csv file for reading." << std::endl;
        return std::vector<int>();
    }

    std::string line;
	// load each line from file
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char delimiter = ',';

        int neighborID;
        std::string neighborArrayStr;

        if (iss >> neighborID >> delimiter >> neighborArrayStr) {
            if (neighborID == id) {
                std::vector<int> neighborArray;

                // Handle the first value separately
                size_t pos = 0;
                int value;

                try {
                    value = std::stoi(neighborArrayStr, &pos);
                    neighborArray.push_back(value);
                } catch (const std::invalid_argument& e) {
                 //   std::cerr << "Invalid argument: " << e.what() << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Out of range: " << e.what() << std::endl;
                }

                // Continue with the rest of the values
                while (pos < neighborArrayStr.length()) {
                    size_t nextPos;
                    try {
                        value = std::stoi(neighborArrayStr.substr(pos + 1), &nextPos);
                        neighborArray.push_back(value);
                        pos += nextPos + 1;
                    } catch (const std::invalid_argument& e) {
                     //   std::cerr << "Invalid argument: " << e.what() << std::endl;
                        break;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Out of range: " << e.what() << std::endl;
                        break;
                    }
                }

                file.close();
                return neighborArray;
            }
        }
    }

    file.close();
    return std::vector<int>(); // returns array of neighbors for given pixel
}

int checkTopRow(int value) {
    // Check if the value is a multiple of 4 and within the range [0, 60]
    if (value % 4 == 0 && value >= 0 && value <= 60) {
        return 1; // Value exists in the array
    }
    return 0; // Value does not exist in the array
}




