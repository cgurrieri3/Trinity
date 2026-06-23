#include "PlotCamera.h"


int main(int argc, char **argv){
        if(argc < 4){
                cout << "Too few arguments; usage: PlotCamera <folder> <simrun> <mount>" << endl;
                return 1;
        }
    
        // Get the Arguments
        std::string folString = argv[1];
        std::string simrun = argv[2];

        std::string mount = argv[3];
    
        if (mount == "y"){ // with usingin htcondor you need to have contianers and some use full paths and other use mounts this lets you specify
                std::cout << "using mounted directory path" << std::endl;
                mnt="/mnt/";
                dataDir = "/mnt/simdata/daviddata/";
                outDir = "/mnt/DataAnalysis/SimulationAnalysis/data/";
        } else if (mount != "n"){
                std::cout << "using specific directory path" << std::endl;
                mnt=mount.c_str();
                dataDir = Form("%ssimdata/daviddata/",mnt.c_str());
                outDir = Form("%sDataAnalysis/SimulationAnalysis/data/",mnt.c_str());
        }


        std::string FolderPath = Form("%s%s_%s/GROPT/Tilt_91.560000/",dataDir.c_str(),folString.c_str(),simrun.c_str());
        cout << FolderPath << endl;
        std::vector<std::string> fileNamesVec;
        fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
        for (auto i: fileNamesVec)
                std::cout << i << ' ';
        cout << endl;
        if (mkdir(Form("%s%s",outDir.c_str(),folString.c_str()), 0775) == -1)
        {
                cout << "Error : possibly directory already exists" << endl;

        } else{
                cout << "Directory created" <<  endl;

        }
        cout << "Output Folder: " << Form("%s%s/",outDir.c_str(),folString.c_str()) << endl;
        // Creates a root file that stores all the data    
        // file = new TFile(OutputFileRoot.c_str(), "RECREATE");  // "RECREATE" to overwrite if it exists
        // file->Close();
        if (fileNamesVec.size()==0){
                cout << "Folder empty moving on..." << endl;
                cout << "Completed!"<< endl;
                return 0;
        }
        
        
        fileOutputPlots = new TFile(Form("%s%s/plots_%s_%s.root",outDir.c_str(),folString.c_str(),folString.c_str(),simrun.c_str()), "RECREATE");
        fileOutputPlots->Close();
        fileOutput = new TFile(Form("%s%s/datafiles_%s_%s.root",outDir.c_str(),folString.c_str(),folString.c_str(),simrun.c_str()), "RECREATE");
        // fileOutput->Close();
        
        canvas = new TCanvas("Display","Event Display",2500,1000);
        canvas->Divide(2,1);
        canvas->cd(1)->SetRightMargin(0.15);
        canvas->cd(2)->SetRightMargin(0.15);
        
        for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        // for(int f = 0; f<2; f++){
                std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
                cout << "file path: " <<FilePath << endl;
                // Create the struct instance
                // SimEventData sim;
                // SimRunData simRun;
                // SimAuxData simAux;
                if (!util->isBranchPresentInFile(FilePath, "T1")) {
                        continue; // Skip to the next branch if not present
                        
                }

                TFile* file = TFile::Open(FilePath.c_str(), "READ");
                TTree* tree = (TTree*)file->Get("T1");
                

                tree->SetBranchAddress("photonX", &vPhotonX);
                tree->SetBranchAddress("photonY", &vPhotonY);
                tree->SetBranchAddress("photonDcosX", &vPhotonDcosX);
                tree->SetBranchAddress("photonDcosY", &vPhotonDcosY);
                // tree->SetBranchAddress("eventNumber", &EventNumber);
                int nEntries = tree->GetEntries();
                // Create the TTree in the output file so the tree is not memory-resident only
                
                fileOutput->cd();
                simRun = new SimRunData(vPhotonX->size());
                treeSims = new TTree("Sim", "Events tree");
                treeSims->Branch("GrOptics","SimRunData",&simRun);
                ev = new IEvent();
                treeNew = new TTree("Test", "Events tree");
                treeNew->Branch("Events","IEvent",&ev);
                

                if (nEntries==0) {
                cout << "No events" << endl;
                continue;
                }
                cout << "Total number of entries: " << nEntries << endl;

         
                

                for(int EventCounter = 0; EventCounter < (nEntries); EventCounter++){
                        cout << "Event number: " << EventCounter << endl;
                        tree->GetEntry(EventCounter);
                        // simRun->Clear();

                        hPhotonX = new TH1F("hist photonX","PhotonX Distribution", 192, -96, 96);
                        hPhotonX->GetXaxis()->SetTitle("Photon X Position (mm)");
                        hPhotonX->GetYaxis()->SetTitle("Number of Photons");
                        hPhotonX->SetStats(0);

                        hPhotonY = new TH1F("hist photonY","PhotonY Distribution", 96, -48, 48);
                        hPhotonY->GetXaxis()->SetTitle("Photon Y Position (mm)");
                        hPhotonY->GetYaxis()->SetTitle("Number of Photons");
                        hPhotonY->SetStats(0);

                        hPhotonDcosX = new TH1F("hist photonDcosX","PhotonDcosX Distribution", 100, -1, 1);
                        hPhotonDcosX->GetXaxis()->SetTitle("PhotonDcosX");
                        hPhotonDcosX->GetYaxis()->SetTitle("Number of Photons");
                        hPhotonDcosX->SetStats(0);

                        hPhotonDcosY = new TH1F("hist photonDcosY","PhotonDcosY Distribution", 100, -1, 1);
                        hPhotonDcosY->GetXaxis()->SetTitle("PhotonDcosY");
                        hPhotonDcosY->GetYaxis()->SetTitle("Number of Photons");
                        hPhotonDcosY->SetStats(0);

                        hPhotonZ = new TH1F("hist photonZ","Photon Incidence Angle Distribution", 180, 0, 90);
                        hPhotonZ->GetXaxis()->SetTitle("Photon Incidence Angle (Deg)");
                        hPhotonZ->GetYaxis()->SetTitle("Number of Photons");
                        hPhotonZ->SetStats(0);

                        hPhotonDist1x = new TH2F("hist1x", Form("Camera View %s_%s_%i, incident photons, 1mm bins", folString.c_str(),simrun.c_str(),EventCounter), 96, -48.5, 47.5, 96, -48.5, 47.5);
                        hPhotonDist1x->GetXaxis()->SetTitle("X (mm)");
                        hPhotonDist1x->GetYaxis()->SetTitle("Y (mm)");

                        hPhotonDist1x6mm = new TH2F("hist1x6mm", Form("Camera View %s_%s_%i, incident photons, 6mm bins", folString.c_str(),simrun.c_str(),EventCounter), 16, -48.5, 47.5, 16, -48.5, 47.5);
                        hPhotonDist1x->GetXaxis()->SetTitle("X (mm)");
                        hPhotonDist1x->GetYaxis()->SetTitle("Y (mm)");

                        
                        TH2F* hPhotonDist1xPixels = new TH2F("hPhotonDist1xPixels", Form("Camera View %s_%s_%i;Pixels along camera x axis;Pixels along camera y axis;Photons", folString.c_str(),simrun.c_str(),EventCounter), CameraStep, CameraMin, CameraMax, CameraStep, CameraMin, CameraMax);
                        hPhotonDist4x = new TH2F("hist4x", Form("Camera View %s_%s (4x)", folString.c_str(),simrun.c_str()), 32*nResMult, -96.5, 95.5, 16*nResMult, -48.5, 47.5);
                        hPhotonDist4x->GetXaxis()->SetTitle("X (mm)");
                        hPhotonDist4x->GetYaxis()->SetTitle("Y (mm)");
                        hPhotonDist4x->SetStats(0);

                        
                        for (size_t j = 0; j < vPhotonX->size(); j++)
                        {
                                hPhotonX->Fill((*vPhotonX)[j]);
                                hPhotonY->Fill((*vPhotonY)[j]);
                                hPhotonDcosX->Fill((*vPhotonDcosX)[j]);
                                hPhotonDcosY->Fill((*vPhotonDcosY)[j]);
                                hPhotonZ->Fill(TMath::ASin(sqrt(pow((*vPhotonDcosX)[j],2)+pow((*vPhotonDcosY)[j],2)))*RadtoDeg);
                                hPhotonDist1x->Fill((*vPhotonX)[j], (*vPhotonY)[j]);
                                hPhotonDist1x6mm->Fill((*vPhotonX)[j], (*vPhotonY)[j]);
                                hPhotonDist4x->Fill((*vPhotonX)[j], (*vPhotonY)[j]);
                                
                        }

                        // First create the root file for this event 
                        simRun->SetSimRun(simrun);
                        simRun->SetDate(std::stoi(folString)); // placeholder for now
                        simRun->SetSimEventNumber(EventCounter);
                        
                        // Flag whether the event has enough signal to trigger. Untriggered
                        // events are still saved to the output, just marked Triggered = false.
                        bool triggered = (hPhotonDist1x6mm->GetMaximum()*PtoPEconversion >= 20);
                        
                        cout << "Number of photons detected: " << hPhotonDist1x6mm->Integral() << endl;
                        
                        
                        simRun->SetTriggered(triggered);
                        int pos =  std::string(fileNamesVec[f]).find("E_");
                        pos =  std::string(FilePath).find("Tilt_");
                        simRun->SetTilt(std::stof(std::string(FilePath).substr(pos+5, 9)));
                        simRun->SetPosX(*vPhotonX);
                        simRun->SetPosY(*vPhotonY);


                        //locate the ISOG output in the same date and then open the text file and read them to SimRunData and then save to the root file
                        std::string ISOGFolderPath = Form("%s%s_%s/ISOG/Tilt_91.560000/",dataDir.c_str(),folString.c_str(),simrun.c_str());
                        // get all the files in the ISOG folder
                        std::vector<std::string> ISOGfileNamesVec;
                        ISOGfileNamesVec=util->GetFilesInDirectory(ISOGFolderPath,".txt");
                        if (ISOGfileNamesVec.size()==0){
                                cout << "ISOG Folder empty moving on..." << endl;
                                cout << "Completed!"<< endl;
                                return 0;
                        }
                        std::string ISOGFilePath = Form("%s%s",ISOGFolderPath.c_str(),ISOGfileNamesVec[0].c_str());
                        cout << "ISOG file path: " << ISOGFilePath << endl;
                        std::ifstream ISOGfile(ISOGFilePath);

                        if (ISOGfile.is_open()) {
                                std::ifstream& file = GotoLine(ISOGfile, EventCounter+3); // Go to the second line
                                std::string line;
                                std::getline(file, line); // Read the line
                                std::istringstream iss(line);
                                cout << "ISOG line: " << line << endl;
                                std::vector<Float_t> isogValues;
                                Float_t tmpValue;
                                while (iss >> tmpValue) {
                                        isogValues.push_back(tmpValue);
                                }

                                if (!isogValues.empty()) {
                                        
                                        if (isogValues.size() > 0) simRun->SetPolarAngle(isogValues[0]);
                                        cout << "Polar Angle: " << isogValues[0] << endl;
                                        if (isogValues.size() > 1) simRun->SetAzimuthAngle(isogValues[1]);
                                        cout << "Azimuth Angle: " << isogValues[1] << endl;
                                        if (isogValues.size() > 2) simRun->SetTelescope_Xpos(isogValues[2]);
                                        cout << "Telescope X Position: " << isogValues[2] << endl;
                                        if (isogValues.size() > 3) simRun->SetTelescope_Ypos(isogValues[3]);
                                        cout << "Telescope Y Position: " << isogValues[3] << endl;
                                        if (isogValues.size() > 4) simRun->SetTelescope_Zpos(isogValues[4]);
                                        cout << "Telescope Z Position: " << isogValues[4] << endl;
                                        if (isogValues.size() > 5) simRun->SetTelescope_Radius(isogValues[5]);
                                        cout << "Telescope Radius: " << isogValues[5] << endl;
                                        if (isogValues.size() > 6) simRun->SetXMax(isogValues[6]);
                                        cout << "XMax: " << isogValues[6] << endl;
                                        if (isogValues.size() > 7) simRun->SetNMax(isogValues[7]);
                                        cout << "NMax: " << isogValues[7] << endl;
                                        if (isogValues.size() > 8) simRun->SetX0Max(isogValues[8]);
                                        cout << "X0Max: " << isogValues[8] << endl;
                                        if (isogValues.size() > 9) simRun->SetLambda(isogValues[9]);
                                        cout << "Lambda: " << isogValues[9] << endl;
                                        if (isogValues.size() > 10) simRun->SetNeutrinoEnergy(isogValues[10]);
                                        cout << "Neutrino Energy: " << isogValues[10] << endl;
                                        if (isogValues.size() > 11) simRun->SetDecayLength(isogValues[11]);
                                        cout << "Decay Length: " << isogValues[11] << endl;
                                        if (isogValues.size() > 12) simRun->SetShowerStartX(isogValues[12]);
                                        cout << "Shower Start X: " << isogValues[12] << endl;
                                        if (isogValues.size() > 13) simRun->SetShowerStartY(isogValues[13]);
                                        cout << "Shower Start Y: " << isogValues[13] << endl;
                                        if (isogValues.size() > 14) simRun->SetShowerStartZ(isogValues[14]);
                                        cout << "Shower Start Z: " << isogValues[14] << endl;
                                        if (isogValues.size() > 15) simRun->SetTauEmergenceProb(isogValues[15]);
                                        cout << "Tau Emergence Probability: " << isogValues[15] << endl;
                                        if (isogValues.size() > 16) simRun->SetEmergencePointX(isogValues[16]);
                                        cout << "Emergence Point X: " << isogValues[16] << endl;
                                        if (isogValues.size() > 17) simRun->SetEmergencePointY(isogValues[17]);
                                        cout << "Emergence Point Y: " << isogValues[17] << endl;
                                        if (isogValues.size() > 18) simRun->SetEmergencePointZ(isogValues[18]);
                                        cout << "Emergence Point Z: " << isogValues[18] << endl;
                                        if (isogValues.size() > 19) simRun->SetGlobeTheta(isogValues[19]);
                                        cout << "Globe Theta: " << isogValues[19] << endl;
                                        if (isogValues.size() > 20) simRun->SetGlobePhi(isogValues[20]);
                                        cout << "Globe Phi: " << isogValues[20] << endl;
                                        if (isogValues.size() > 21) simRun->SetTauEnergy(isogValues[21]);
                                        cout << "Tau Energy: " << isogValues[21] << endl;
                                        // if (isogValues.size() > 22) simRun->SetZenith(isogValues[22]);
                                        // cout << "Zenith: " << isogValues[22] << endl;
                                        // if (isogValues.size() > 23) simRun->SetAzimuth(isogValues[23]);
                                        // cout << "Azimuth: " << isogValues[23] << endl;
                                        // if (isogValues.size() > 24) simRun->SetTilt(isogValues[24]);
                                        // cout << "Tilt: " << isogValues[24] << endl;
                                }
                        }
                      
                        treeSims->Fill();
                        if (!triggered) {
                                cout << "Not enough photons detected in this simulation run, saving as untriggered..." << endl;
                                continue;
                        }
                        
                        // Emergence angle: wrap azimuth to [-180,180] to measure from telescope axis
                        double azimuthDeg = simRun->GetAzimuthAngle();
                        double emergenceAngle = (azimuthDeg > 180.0) ? (azimuthDeg - 360.0) : azimuthDeg;
                        // Distance to emergence point from telescope position vector
                        double Rx = simRun->GetTelescope_Xpos();
                        double Ry = simRun->GetTelescope_Ypos();
                        double Rz = simRun->GetTelescope_Zpos();
                        double emergenceDistance = sqrt(Rx*Rx + Ry*Ry + Rz*Rz);
                        cout << "Emergence Angle (from telescope axis): " << emergenceAngle << " deg" << endl;
                        cout << "Distance to Emergence Point: " << emergenceDistance << " (units of telescope coords)" << endl;

                        // Second create the plots for each event and save them to a root file

                        // create a lable with neutrino energy
                        
                        fileOutputPlots = new TFile(Form("%s%s/plots_%s_%s.root",outDir.c_str(),folString.c_str(),folString.c_str(),simrun.c_str()), "UPDATE");
                        TPaveText *ptEnergy = new TPaveText(0.1,0.9,0.9,0.95,"NDC");
                        ptEnergy->SetFillColor(0);
                        ptEnergy->SetTextAlign(12);
                        ptEnergy->SetTextSize(0.04);
                        ptEnergy->AddText(Form("Event: %i, Neutrino Energy: %.3g GeV, Emergence Angle: %.3g degrees, Shower Distance: %.3g m ,Total Photons: %i", EventCounter,simRun->GetNeutrinoEnergy(), emergenceAngle, emergenceDistance, int(hPhotonDist1x6mm->Integral())));
                        canvas->Clear();
                        canvas->Divide(2,1);
                        canvas->cd(1)->SetRightMargin(0.15);
                        canvas->cd(2)->SetRightMargin(0.15);// open the file for plots and get all plots in 
                        canvas->cd(1);
                        hPhotonDist1x->SetMarkerSize(200.0);
                        hPhotonDist1x->SetStats(0);
                        hPhotonDist1x->Draw("colz");
                        hPhotonDist1x->Write(Form("h_%s_%s_%i_1mm",folString.c_str(),simrun.c_str(),EventCounter));
                        DrawMUSICBoundaries();
                        ptEnergy->Draw("SAME");
                        canvas->cd(2);
                        hPhotonDist1x6mm->SetMarkerSize(200.0);
                        hPhotonDist1x6mm->SetStats(0);
                        hPhotonDist1x6mm->Draw("colz");
                        hPhotonDist1x6mm->Write(Form("h_%s_%s_%i_6mm",folString.c_str(),simrun.c_str(),EventCounter));
                        DrawMUSICBoundaries();
                        
                        canvas->Write(Form("C_%s_%s_%i",folString.c_str(),simrun.c_str(),EventCounter));

                        
                        
                        hPhotonDist1x6mm->Scale(PtoPEconversion);
                        // Loop through all bins (excluding underflow/overflow)
                        int nBinsX = hPhotonDist1x6mm->GetNbinsX();
                        int nBinsY = hPhotonDist1x6mm->GetNbinsY();
                        for (int ix = 1; ix <= nBinsX; ++ix) {
                                for (int iy = 1; iy <= nBinsY; ++iy) {
                                        double content = hPhotonDist1x6mm->GetBinContent(ix, iy);
                                        if (content > 160){
                                                content = 160;
                                        }
                                        hPhotonDist1xPixels->SetBinContent(ix,iy,content);
                                }
                        }
                        canvas->cd(0);
                        hPhotonDist1xPixels->SetMarkerSize(200.0);
                        hPhotonDist1xPixels->SetStats(0);
                        // hPhotonDist1xPixels->SetMinimum(-0.01);
                        hPhotonDist1xPixels->Draw("colz");
                        hPhotonDist1xPixels->Write(Form("h_%s_%s_%iPixelsPE",folString.c_str(),simrun.c_str(),EventCounter));
                        hPhotonDist1xPixels->GetZaxis()->SetTitleOffset(1.4);
                        hPhotonDist1xPixels->GetZaxis()->SetTitle("PE");
                        plottools->DrawSIABBoundaries();
                        ptEnergy->Draw("SAME");
                        canvas->SetRightMargin(0.15);
                        canvas->SetRightMargin(0.15);
                        canvas->Write(Form("C_%s_%s_%iPixelsPE",folString.c_str(),simrun.c_str(),EventCounter));
                        // close the plots file for now
                        fileOutputPlots->Close();
                        
                        create_root_file(hPhotonDist1xPixels,treeNew);
                        
                        
                       
                        
                        // delete tree;
                        delete ptEnergy;
                        delete hPhotonDist1xPixels;
                        delete hPhotonX;
                        delete hPhotonY;
                        delete hPhotonDcosX;
                        delete hPhotonDcosY;
                        delete hPhotonZ;
                        delete hPhotonDist1x;
                        delete hPhotonDist1x6mm;
                        delete hPhotonDist4x;
                        

                        
                        
                }
                
        }
        fileOutput->cd();
        treeSims->Write();
        treeNew->Write();
        fileOutput->Close();
        util->setFilePermissions(Form("%s%s/datafiles_%s_%s.root",outDir.c_str(),folString.c_str(),folString.c_str(),simrun.c_str()));
        util->setFilePermissions(Form("%s%s/plots_%s_%s.root",outDir.c_str(),folString.c_str(),folString.c_str(),simrun.c_str()));

        delete simRun;
        delete ev;
        // delete tree;
        // delete treeSims;
        delete fileOutput;
        delete fileOutputPlots;
        delete canvas;
        cout << "Completed!" << endl;   
}


void create_root_file(TH2F* hist, TTree* treeNew) {


    ev->SetCoBoTime(ULong64_t( 174000000000000ULL));
    ev->SetUNIXTime(ULong64_t(0ULL));
    ev->SetTBTime(ULong64_t( 174000000000000ULL));
    ev->SetEventType(ULong64_t(3));
    std::vector<vector<Int_t>> pixels;
    for(int i =0; i < 256; i++){
        std::vector<Int_t> trace;    
        for(int j = 0; j<512; j++) {
            trace.push_back(160.0);
        }
        // get the pixel nx, and ny

        int nx,ny;
        plottools->FindBin(i,&nx,&ny);
        // cout << "pixel:" << i << "bin content: " << hist->GetBinContent(nx+1,ny+1) << endl;
        // trace[239] = 160.0 - hist->GetBinContent(nx+1,ny+1);
        if (hist->GetBinContent(nx+1,ny+1) > 160){
                trace[239] = 160.0 - 160.0;

        } else {
                trace[239] = 160.0 - hist->GetBinContent(nx+1,ny+1);

        }
        // cout << trace[239] << endl;
        pixels.push_back(trace);
    }
    ev->SetSignalValue(pixels);
    int maxBinX, maxBinY, dummy;
    hist->GetMaximumBin(maxBinX, maxBinY, dummy);
    hist->GetBinContent(maxBinX, maxBinY);
//     cout << "Max bin content: " << hist->GetBinContent(maxBinX, maxBinY) << endl;
    int MusicID = plottools->FindMUSIC(maxBinX-1, maxBinY-1);

    ev->SetROIMusicID({MusicID,-1});
    ev->SetROIPixelID({1});
    std::vector<float> temp = {1.0};
    std::string temp_str = "";
    ev->SetParametersFromTimestamp(temp_str,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp,temp);
    
    
    treeNew->Fill();

}


