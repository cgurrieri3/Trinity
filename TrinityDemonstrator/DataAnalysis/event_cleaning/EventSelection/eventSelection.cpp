#include "eventSelection.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <mount>%s <folder>%s <filelist>%s \n", argv[0], argv[1], argv[2], argv[3]);
        return 1;
    }
    
    mount = argv[1];
    folder = argv[2];
    file_list = argv[3]; // set globals declared in eventSelection.h
    // read in the file list and then only allow events that are within the file list to be saved to
    // the output pdf/ root file. 
    // if no file_list is given then all files will go to the pdf/root file

    // ensure the file is mounted correctly in the container
    std::cout << "mount: " << mount << " folder: " << folder << "File_list:" << file_list << std::endl;
    
    std::cout << "using standard directory path: " << mnt << std::endl;
    if (mount != "y"){
        std::cout << "using specific directory path: " << mnt << std::endl;
        mnt=mount.c_str();
    }
    // add the mount path to the any paths
    // if the file list refers to simulation data (contains "sim") read from OutputSim
    bool isSim = (file_list.find("sim") != std::string::npos);
    if (isSim) {
        std::cout << "simulation file list detected, using OutputSim path" << std::endl;
        FolderPath = Form("%s/DataAnalysis/event_cleaning/OutputSim/%s/", mnt.c_str(), folder.c_str());
    } else {
        FolderPath = Form("%s/DataAnalysis/event_cleaning/Output/%s/", mnt.c_str(), folder.c_str());
    }
    AllowedFilesPath = Form("%s/%s", mnt.c_str(), file_list.c_str());
    std::cout << "folder path: " << FolderPath << "\nAllowed path: " << AllowedFilesPath <<std::endl;

    fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
    cout << "This is the first file: " << fileNamesVec[0] << endl;

    if (file_list != "n" && file_list != "sim"){
        allowedFilesVec=util->readFileToVectorString(AllowedFilesPath.c_str());
        cout << "Number of allowed files: " << allowedFilesVec.size() << endl;
        cout << "Sample file from allowed Files " << allowedFilesVec[50] << endl;
    }

    TCanvas* c_cleaned = new TCanvas("c_cleaned", "c", 10, 10, 800, 600);
    c_cleaned->Print((OutputPdfFile + "[").c_str());
    TFile *file = new TFile(OutputRootFile.c_str(), "RECREATE");
    file->Close();

    for (auto f: fileNamesVec) {
        
        std::string fname = Form("%s%s",FolderPath.c_str(), f.c_str());
        cout << fname << endl;
        TFile* file = TFile::Open(fname.c_str(), "READ");
        if (!file || file->IsZombie()) {
            printf("Error opening file %s\n", f.c_str());
            return 1;
        }

        // Get the tree containing EventInfo objects
        TTree* tree = nullptr;
        file->GetObject("EventCleaning", tree); // Replace "Events" with your actual tree name
        if (!tree) {
            printf("TTree 'EventCleaning' not found in file.\n");
            file->Close();
            return 1;
        }

        EventInfo* event = nullptr;
        tree->SetBranchAddress("Cleaned", &event); // Replace "EventInfo" with your branch name

        // For simulation data, EventCleaning copies the input "Sim" (GrOptics / SimRunData)
        // tree into the output file aligned 1:1 with the EventCleaning entries. Read it so the
        // per-event neutrino energy is available for the saved-vs-removed plot.
        TTree* simTree = nullptr;
        SimRunData* simRunData = nullptr;
        if (isSim) {
            simTree = (TTree*)file->Get("Sim");
            if (simTree) {
                simRunData = new SimRunData();
                simTree->SetBranchAddress("GrOptics", &simRunData);
            } else {
                printf("TTree 'Sim' not found in sim file %s; energies will be skipped.\n", f.c_str());
            }
        }

        Long64_t nEntries = tree->GetEntries();
        // load into the PlotHelp class 
        // nEntries = 100;
        for (Long64_t i = 0; i < nEntries; ++i) {
            // Testing on My Machine
            tree->GetEntry(i);
            if (!event) continue;

            // sims: read the neutrino energy for this event (aligned by entry) and track it so
            // AddEventFlags can bin saved (flag 5) vs removed energies. Non-sim: do not track.
            if (isSim && simTree && simRunData && i < simTree->GetEntries()) {
                simTree->GetEntry(i);
                plothelp->SetSimEnergy(simRunData->GetNeutrinoEnergy(), true);
            } else {
                plothelp->SetSimEnergy(0.0, false);
            }

            // For actual use
            // plothelp->AddTotalEvent(1);
            // Add the logic for these events so i can practice counting them
            // print the before and after histgrams to the pdf/root file
            
            // if all the pre cleaning/ cleanedpanel are false then print the pdf/roof 2 panel image
            
            
            
            plothelp->AddtoEventDate(event->GetDate());
            plothelp->AddtoRMSratioAVGamp(event->GetRMS_AvgAmp());
            
            std::string currentfilename = (event->GetFile()).erase(0, 7);
            cout << "Current File Name: " << currentfilename << endl;
            
            if (event->GetEventFlag() != 5) {
                plothelp->AddEventFlags(event->GetEventFlag());
                if (event->GetEventFlag() == 9) {
                    continue;
                }
                
                if (isSim) {
                    // simulation events are neither door open nor door closed
                } else if (file_list == "n") {
                    std::cout << ("no file list saving all  ") << std::endl;

                } else if ( std::find(allowedFilesVec.begin(), allowedFilesVec.end(), currentfilename) != allowedFilesVec.end() ){
                    // std::cout << ("File found in list saving") << std::endl;
                    dooropen = dooropen + 1;

                } else {
                    // std::cout << ("Not in the list skipping") << std::endl;
                    doorclosed = doorclosed + 1;


                }


                continue;
            }

            if (isSim) {
                // simulation events are neither door open nor door closed; save and continue to selection
                plothelp->AddEventFlags(event->GetEventFlag());
            } else if (file_list == "n") {
                std::cout << ("no file list saving all  ") << std::endl;
                plothelp->AddEventFlags(event->GetEventFlag());
            } else if ( std::find(allowedFilesVec.begin(), allowedFilesVec.end(), currentfilename) != allowedFilesVec.end() ){
                std::cout << ("File found in list saving") << std::endl;
                plothelp->AddEventFlags(event->GetEventFlag());
                dooropen = dooropen + 1;
                continue; // add for door closed
            } else {
                std::cout << ("Not in the list skipping") << std::endl;
                plothelp->AddEventFlags(4);
                
                doorclosed = doorclosed + 1;
                // continue; // add for no closed door
                
            }
            
            cout << "Event Flag: " << event->GetEventFlag() << endl;
            // Event was never cleaned away so that means it survived and now needs
            // Event selection
            // create the 2 panel image
            auto c = new TCanvas("c", "2-d options",10,10, 800, 600);
            c->Divide(2, 1);
            c->SetRightMargin(0.15); // Adjust right margin for color palette
            c->SetBottomMargin(0.15); // Adjust bottom margin for x-axis labels
            c->SetLeftMargin(0.15); // Adjust left margin for y-axis labels
            
            TH2F* hist = event->GetHPanel1();
            TH2F* hist4 = event->GetHPanel4();
            // std::vector<float> hvcurrent = event->Gethvc();
            // float hvsum = 0.0;
            // for (const auto& hv : hvcurrent) {
            //     hvsum += hv;
            // }
            // float hvavg = hvsum / hvcurrent.size();
            // cout << "Average HV: " << hvavg << " V" << endl;
            // if (hist4 && hvavg > 4.0) {
            if (hist4) {
                // printf("I Exist");
                c_cleaned->cd(0);
                c_cleaned->Divide(2, 1);
                c_cleaned->cd(1);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                TLatex *subtitle = new TLatex();
                subtitle->SetNDC(); // Set to Normalized Device Coordinates (NDC)
                subtitle->SetTextSize(0.03);
                
                subtitle->DrawLatex(0.1, 0.93, Form("Avg Amp Whole Camera : %.2f - Avg RMS: %.2f", event->GetAvgAmp(), event->GetRMS()));
                subtitle->DrawLatex(0.1, 0.91, Form("Triggered MUSIC: %i  Triggered Pixel: %i", event->GetTriggeredMUSICID(), event->GetTriggeredPixelID()));




                
                c_cleaned->Update();
                c->cd(1);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                c->Update();
                c_cleaned->cd(2);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist4->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                c->cd(2);
                gPad->SetLeftMargin(0.15);  // Increase left margin
                gPad->SetBottomMargin(0.25); // Increase bottom margin
                gPad->SetRightMargin(0.15);
                hist4->Draw("COLZ");
                plottools->DrawMUSICBoundaries();
                c_cleaned->cd(2);
                
                
                double meanx = event->GetCOGx(); 
                double meany = event->GetCOGy(); 
                cout << "MEAN " << meanx << "," << meany << endl;
                double anglerad = event->GetAngle();
                double angledeg = anglerad*(180.0/3.141592653589793238463);
                
                
                double r1 = event->Getr1();
                double r2 = event->Getr2();    
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
                
                // cev->GetSurvivingPixelTotalAmpPanel3()/util->GetADCtoPEratio(),
                title->DrawLatex(0, 0.93, Form("WL:%.2f Pixels:%i Area:%.2f Size:%.2f Conc:%.2f M3Long:(%s,  %s)",
                    event->GetWLRatio(),
                    event->GetSurvivngPixels(),
                    event->GetArea(),
                    event->GetSize(),
                    event->GetConc(),
                    event->GetM3Longx().c_str(),
                    event->GetM3Longy().c_str()));

                // For sims, draw the emergence angle and distance to the emergence point at the
                // bottom of the canvas. Drawn before the pdf Print and root Write so it appears in both.
                TPaveText* simInfo = nullptr;
                if (isSim && simRunData) {
                    // distance to the emergence point from the telescope position vector
                    double Rx = simRunData->GetTelescope_Xpos();
                    double Ry = simRunData->GetTelescope_Ypos();
                    double Rz = simRunData->GetTelescope_Zpos();
                    double emergenceDistance = sqrt(Rx*Rx + Ry*Ry + Rz*Rz);

                    // emergence angle: wrap azimuth to [-180,180] to measure from the telescope axis
                    double azimuthDeg = simRunData->GetAzimuthAngle();
                    double emergenceAngle = (azimuthDeg > 180.0) ? (azimuthDeg - 360.0) : azimuthDeg;

                    c_cleaned->cd(0);
                    simInfo = new TPaveText(0.2, 0.0, 0.8, 0.06, "NDC");
                    simInfo->SetFillColorAlpha(0, 0.0);
                    simInfo->SetFillStyle(0);
                    simInfo->SetBorderSize(0);
                    simInfo->SetTextSize(0.02);
                    simInfo->AddText(Form("Energy: %.3g GeV    Emergence Angle: %.3g deg    Distance to Emergence Point: %.3g m",
                        simRunData->GetNeutrinoEnergy(), emergenceAngle, emergenceDistance));
                    simInfo->Draw();
                }

                c->Update();
                c_cleaned->Update();
                c_cleaned->Print(OutputPdfFile.c_str());
                file = new TFile(OutputRootFile.c_str(), "UPDATE");
                c_cleaned->Write(Form("N%i-F%s-E%i", event->GetDate(),event->GetFile().c_str() ,event->GetEvent()));
                hist->Write(Form("PreN%i-F%s-E%iTH2F", event->GetDate(),event->GetFile().c_str() ,event->GetEvent()));
                hist4->Write(Form("PostN%i-F%s-E%iTH2F",event->GetDate(),event->GetFile().c_str() ,event->GetEvent()));
                file->Close();
                c_cleaned->Clear();
                    delete subtitle;
                delete title;
                if (simInfo) delete simInfo;
                // c->SaveAs(Form("hist_entry_%lld.png", i));
            } else {
                printf("Histogram not found in EventInfo object at entry %lld.\n", i);
            }

            


            
            int date = event->GetDate();
            cout << "Date: " << date << endl;
            cout << "Event Number: " << event->GetEvent() << endl;
            
            // add to test scirpt
            plothelp->AddtoWL(event->GetWLRatio());
            plothelp->AddtoL(event->GetL());
            plothelp->AddtoW(event->GetW());
            plothelp->AddtoSize(event->GetSize());
            plothelp->AddtoSurvivingPixelCount(event->GetSurvivngPixels());
            plothelp->AddtoConcentation(event->GetConc());
            plothelp->AddtoCOGx(event->GetCOGx());
            plothelp->AddtoCOGy(event->GetCOGy());
            plothelp->AddtoTriggeredPixelsID(event->GetTriggeredPixelID());
            plothelp->AddtoCoreRatio(event->GetCoreRatio());
            // cout << "Ratio core:" << event->GetCoreRatio() << endl;
            // sleep(1);
            plothelp->AddtoNumberOfCores(event->GetNumberOfCores());
            // cout << "# core:" << event->GetNumberOfCores() << endl;
            // sleep(1);



            delete c; // Clean up the canvas after use
            // delete hist; // Clean up the histogram after use
            // delete hist4; // Clean up the histogram after use
        }
        if (simRunData) { delete simRunData; } // simTree is owned by file and freed on Close
        file->Close();
    }
        // Make cuts

    // TCanvas* c_cleaned = new TCanvas("c", "c", 800, 600);
    // c_cleaned->Print("EventSelectionPlots.pdf[");
    // Make Plots
    
   file = new TFile(OutputRootFile.c_str(), "UPDATE");
    
    plothelp->PlothWL(c_cleaned, OutputPdfFile);
    plothelp->PlotEventFlags(c_cleaned, OutputPdfFile);
    plothelp->PlothdistLandW(c_cleaned, OutputPdfFile);
    plothelp->PlothSize(c_cleaned, OutputPdfFile);    
    plothelp->PlothSizeConc(c_cleaned, OutputPdfFile);    
    plothelp->PlothSizeSPC(c_cleaned, OutputPdfFile);    
    plothelp->PlothCoreRatio(c_cleaned, OutputPdfFile);    
    plothelp->PlothNumberofCores(c_cleaned, OutputPdfFile);    
    plothelp->PlothCoreRatioandNumberofCores(c_cleaned, OutputPdfFile);    
    plothelp->PlothWLConc(c_cleaned, OutputPdfFile);    
    plothelp->PlothWLSPC(c_cleaned, OutputPdfFile); 
    plothelp->PlothSIZEWL(c_cleaned, OutputPdfFile);   
    plothelp->PlothLvW(c_cleaned, OutputPdfFile);
    plothelp->PlothSPCConc(c_cleaned, OutputPdfFile);    
    plothelp->PlothCOG(c_cleaned, OutputPdfFile);    
    plothelp->PlothTPID(c_cleaned, OutputPdfFile);
    plothelp->PlothRMSratioAvg(c_cleaned, OutputPdfFile);
    plothelp->PlothEventDateTriggeredPixel(c_cleaned, OutputPdfFile);
    plothelp->PlothncoreSIZE(c_cleaned, OutputPdfFile);
    plothelp->PlothnCoreSPC(c_cleaned, OutputPdfFile);
    plothelp->PlothCRSPC(c_cleaned, OutputPdfFile);
    plothelp->PlothSIZEWandL(c_cleaned, OutputPdfFile);
    plothelp->PlothSimEnergySavedVsNotSaved(c_cleaned, OutputPdfFile); // sims only: saved vs removed neutrino energy
    
    file->Close();
    // c_cleaned->Print("EventSelectionPlots.pdf"Close.c_str());
    util->setFilePermissions(OutputRootFile.c_str());
    // cout << "Completed!" << endl;   
    // // cout << "Total Events: " << plothelp->GetTotalEvent() << endl;
    // delete plothelp;
    
    c_cleaned->Print((OutputPdfFile + "]").c_str());


    // cout << "Total Events Processed: " << plothelp->GetTotalEvent() << endl;
    // cout << "Total Surviving Events: " << plothelp->GetSurvivingEvent() << endl;
    // cout << "Total HLED Events: " << plothelp->GetHLEDEvent() << endl;
    // cout << "Total Pre Cleaned Events: " << plothelp->GetPreCleanedEvent() << endl;
    // cout << "Total Panel 2 Cleaned Events: " << plothelp->GetPanel2CleanedEvent() << endl;
    // cout << "Total Panel 3 Cleaned Events: " << plothelp->GetPanel3CleanedEvent() << endl;
    cout << "Door Open Event Total: " << dooropen << endl;
    cout << "Door Closed Event Total: " << doorclosed << endl;
    cout << "Completed" << endl;
    
    
    return 0;
}