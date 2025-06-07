#include "CEvent.h"
#include <iostream>

// Constructor
CEvent::CEvent() : TriggeredMUSICID(-1) {
    // Initialize TriggeredMUSICID to -1 (default value)
}

void CEvent::LoadFlasherCalibration(std::string CalibrationFactorDir, std::string folString) {
    // Start by creating a histogram for panel 1 then use that to copy and create the 2nd panel
    // create the first panel, apply the Gain, apply the ADC to PE 
    TFile *flasher = new TFile(Form("%s%s_FlasherCalibration_Factor.root", CalibrationFactorDir.c_str(), folString.c_str()));
    if (!flasher || flasher->IsZombie()) {
        std::cerr << "Error: Could not open file " << Form("%s%s_FlasherCalibration_Factor.root", CalibrationFactorDir.c_str(), folString.c_str()) << std::endl;
    }

    hflasher = (TH2F*)flasher->Get("CamFlasher");
    if (!hflasher || hflasher->IsZombie()) {
        std::cerr << "Error: Could not find 'CamFlasher' in the file or it is invalid." << std::endl;
    }
    hflasher->SetDirectory(0);
    flasher->Close();
    delete flasher;
}

// this is for when you set all your gain and such when outside the script
void CEvent::SetAmplitudeValues(std::vector<float> amp) {
    // sets the peak amplitude  and assumes that gain calibration as already been set to the CEvent object
    AmplitudeValues = amp;
}

// setting the abs gain and the flasher calibration within the CEvent object for when you pull a time bin
void CEvent::SetAmplitudeValuesTimeBin(std::vector<float> amp, std::string CalibrationFactorDir, std::string folString, std::vector<double> absgain) {
    // sets the peak amplitude  and assumes that gain calibration as already been set to the CEvent object
    // Load the flasher calibration histogram
    LoadFlasherCalibration(CalibrationFactorDir, folString);
    TH2F* hf = GetFlasherCalibration();
    
    // set the absolute gain from the ISiPM calss
    SetAbsoluteGain(absgain);

    AmplitudeValuesTimeBin = amp;
    for (int i = 0; i < MaxNofChannels; i++) {
        int nx, ny;
        IPlotTools::FindBin(i, &nx, &ny);
        double flashercalibratedamp = amp[i]*(1.0/hf->GetBinContent(nx + 1, ny + 1));
        // std::cout << "flasher calib: " << flashercalibratedamp << std::endl;
        AmplitudeValuesTimeBin[i] = flashercalibratedamp*(1.0/AbsoluteGain[i]);
        // std::cout << "AmplitudeValuesTimeBin[" << i << "]: " << AmplitudeValuesTimeBin[i] << std::endl;
    }
    delete hf;
}

void CEvent::SetRMS(std::vector<float> amp){
    float sumsq = 0;
    for (std::size_t i = 0; i < amp.size(); i++){
        sumsq += amp[i]*amp[i];
        // cout << "sumsq: " << sumsq << endl;
        RMS = sqrt(sumsq)/MaxNofChannels;
        // cout << "RMS: " << RMS << endl;
    }
}

void CEvent::SetPanel1(TH2F* hcam_panel1) {
    hcam_panel1->SetStats(0);
    hcam_panel1->GetXaxis()->SetLabelSize(0.03);
    hcam_panel1->GetYaxis()->SetLabelSize(0.03);
    hcam_panel1->SetXTitle("Pixel Bin along x axis of Camera");
    hcam_panel1->SetYTitle("Pixel Bin along y axis of Camera");
    hcam_panel1->GetXaxis()->SetTitleOffset(1.1); // Adjust X-axis title offset
    hcam_panel1->GetYaxis()->SetTitleOffset(1.0); // Adjust Y-axis title offset
    hcam_panel1->SetMinimum(-1);
    
    for(int j = 0; j<MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        hcam_panel1->SetBinContent(nx + 1, ny + 1, AmplitudeValuesTimeBin[j]);
    }
    
    hcam_panel1->Draw("colz");
    IPlotTools::DrawMUSICBoundaries();
    
}

void CEvent::SetPanel2(TH2F* hcam_panel2, int CorePixelAmpCutOff) {

    
    for(int j = 0; j<MaxNofChannels; j++){
        
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        float binContent= hcam_panel2->GetBinContent(nx+1,ny+1);
        if (binContent >= CorePixelAmpCutOff) { 
            hcam_panel2->SetBinContent(nx + 1, ny + 1, binContent);
            //std::cout << "Surviving Panel 2 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            SurvivingPixelPanel2.push_back(j);
        } else {
            hcam_panel2->SetBinContent(nx + 1, ny + 1, 0);
        }
    }

    hcam_panel2->Draw("colz");
    hcam_panel2->SetMinimum(7.95);
    IPlotTools::DrawMUSICBoundaries();
}

void CEvent::SetPanel3(TH2F* hcam_panel3, std::string neighborDir) {
    std::vector<int> PixelPeakTimes = PeakTimeBin;
    int TriggeredPixelPeakTime = PixelPeakTimes[MaxAmplitdePixelID];
    // std::cout << "Triggered Pixel Peak Time: " << TriggeredPixelPeakTime << std::endl;
    // Get neighboring pixels to triggered pixel
    std::vector<int> neighborsurvivingPanel3Pixels;
    std::vector<int> tempSurvivingPixelPanel3;
    TriggeredPixelNeighborhoodChecker(SurvivingPixelPanel2,MaxAmplitdePixelID,neighborsurvivingPanel3Pixels, neighborDir);
    for(int j = 0; j < MaxNofChannels; j++){
        int nx, ny;
        IPlotTools::FindBin(j, &nx, &ny);
        if (PeakTimeBin[j] == TriggeredPixelPeakTime && std::find(std::begin(neighborsurvivingPanel3Pixels), std::end(neighborsurvivingPanel3Pixels), j) != std::end(neighborsurvivingPanel3Pixels)) {
            // std::cout << "Surviving Panel 3 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
            tempSurvivingPixelPanel3.push_back(j);
            //hcam_panel3->SetBinContent(nx + 1, ny + 1, AmplitudeValuesTimeBin[j]);
        } //else {
            // hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
            //}
    }
        
        neighborsurvivingPanel3Pixels={};
        TriggeredPixelNeighborhoodChecker(tempSurvivingPixelPanel3,MaxAmplitdePixelID,neighborsurvivingPanel3Pixels, neighborDir);
        for(int j = 0; j < MaxNofChannels; j++){
            int nx, ny;
            IPlotTools::FindBin(j, &nx, &ny);
            float binContent = hcam_panel3->GetBinContent(nx+1,ny+1);
            if (PeakTimeBin[j] == TriggeredPixelPeakTime && std::find(std::begin(neighborsurvivingPanel3Pixels), std::end(neighborsurvivingPanel3Pixels), j) != std::end(neighborsurvivingPanel3Pixels)) {
                // std::cout << "Surviving Panel 3 Pixel: " << j << " Amplitude: " << AmplitudeValuesTimeBin[j] << std::endl;
                SurvivingPixelPanel3.push_back(j);
                SurvivingPixelTotalAmpPanel3 += binContent;
                hcam_panel3->SetBinContent(nx + 1, ny + 1, binContent);
            } else {
            hcam_panel3->SetBinContent(nx + 1, ny + 1, 0);
            }
        }
    hcam_panel3->Draw("colz");
    IPlotTools::DrawMUSICBoundaries();
}

void CEvent::NeighborhoodCheckerHelper(int ID, std::vector<int>& IDSelected,std::vector<int>& cleaned_pixels, std::vector<int>& visited_ids, std::string neighborDir) {
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
                    NeighborhoodCheckerHelper(arrayID,IDSelected, cleaned_pixels,visited_ids, neighborDir);
                }
            }

        }
    }
    //cout << ID <<", ";
}


void CEvent::TriggeredPixelNeighborhoodChecker(std::vector<int>& IDSelected, int triggeredpixel,std::vector<int>& cleaned_pixels, std::string neighborDir){
    // we already know that the triggered pixel has a high enough amplitude 
    // We have a list of amplitudes that survied the first part of the the cleaning the straight amplitude cut across the whole camera
    // that will have 256 pixels and if it is to low the pixel will be 0 otherwise it will have the correct amplitude value if it meets the amplitude requirement
    //IDSelected.erase(std::remove(IDSelected.begin(), IDSelected.end(), triggeredpixel), IDSelected.end());
    
    std::vector<int> visited_ids;
    // get the Trigger pixels neighbors
    cleaned_pixels.push_back(triggeredpixel);
    for (int arrayID : GetNeighborArray(triggeredpixel,Form("%sneighborbroadside.csv",neighborDir.c_str()))) {
        // cout << "arrayID: Triggered() " << arrayID << endl;
        NeighborhoodCheckerHelper(arrayID,IDSelected,cleaned_pixels,visited_ids, neighborDir);
        // cout << "completed ID: "<< endl;
    }



}

// Function to open a lookup table for the neighbors of each pixel ////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> CEvent::GetNeighborArray(int id, std::string filename) {

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

int CEvent::isConfigureEvent(){
    if (SurvivingPixelPanel3.size() < 16 && SurvivingPixelTotalAmpPanel3 < 2080){
        return 0;
    }
    std::vector<int> survivingSIABS; 
    for(unsigned int i=0; i < SurvivingPixelPanel3.size(); i++){
        int nx, ny;
        IPlotTools::FindBin(SurvivingPixelPanel3[i],&nx,&ny);
        survivingSIABS.push_back(IPlotTools::FindSIAB(nx,ny));
    }

    std::sort(survivingSIABS.begin(), survivingSIABS.end());
    auto unique=std::unique(survivingSIABS.begin(), survivingSIABS.end());
    survivingSIABS.erase(unique, survivingSIABS.end());

    if (survivingSIABS.size() > 1){
        return 0;
    }
    return 1;

}



