//R__LOAD_LIBRARY(libExACT.so)
//#include "libExACT.so"
#include "PCA.h"
#include "PlotHelp.h"
#include <set>
#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include "TLine.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <TPaletteAxis.h>
#include <TEllipse.h>
#include <IUtilities.h>
#include <TObjString.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <TArrow.h>
#include <sys/types.h>
#include <vector>
#include <TLatex.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <Event.h>
#include <BiFocal.h>
#include <Pulse.h>
#include <TFile.h>
#include <TBox.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <Getline.h>
#include <TRandom.h>
#include <TTimer.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TPaveStats.h>
#include <TList.h>
#include <TFile.h>
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TMath.h>
#include <TPrincipal.h>
#include "Math/Integrator.h"
#include <TPaveText.h>
#include <random>
#include <utility>
#include <CEvent.h>
#include <IPlotTools.h>

// Define variables (global)
std::vector<double> MaxAmplitudePixel;
std::vector<int> MaxPixelID;
std::vector<int> MaxPeakTimePixel;
std::vector<int> MaxMUSICID;
std::vector<double> MaxAmplitudePixelTimeBin;
std::vector<int> MaxPixelIDTimeBin;
std::vector<double> AvgAmplitudePerEvent;
std::vector<double> CameraAmplitudeAtTimeBin;

IUtilities *util;
IPlotTools *plottools;
CEvent *cev;
PlotHelp *plothelp;

TTree *tree = 0;
TTree *treeHLED = 0;
Event *ev;
Event *evHLED;


TH2F* hcam_panel1=0;
TH2F* hcam_panel2=0;
TH2F* hcam_panel3=0;
TH2F* hcam_panel4=0;
TH2F* COGgraph=0;
TH2F* COGgraphweighted=0;
TH2F* pixeldist=0;

TFile *fO;
TFile *file;

TCanvas *c_cleaned =0;
TCanvas *c_PCA=0;
int MaxNofChannels = 256;

std::string neighborDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/ClusterCleaning/neighbors/";
std::string CalibrationFactorDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/FlasherCalibration/Output/";
std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
// std::string dataDir = "/storage/hive/project/phy-otte/sstepanoff3/Data/";
std::string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/Output/";
// std::string outDir = "/storage/hive/project/phy-otte/sstepanoff3/EventCleaning/ClusterCleaning/Output/";

const int TriggeredChannelAmpCutOff = 200; // (200 ADC/8 PE) Cut off for the triggered music channel
const int TimeBinAll = 239; // Difference between triggered pixel time bin and the pixels around it time bin difference more that 1 risk saving cross talk events
const int CorePixelAmpCutOff = 200; // (200ish ADC/8PE) // Cut off for the pixels surrounding the triggered music channel
const int SaturatedPixelCutoff = 256; // 0 removed saturated pixels max channels from being cleaned. 1 allows them to be cleaned (changed from 0 on 2/26/2025)
const int FlasherEventsCutOff = 800; // (350 ish ADC/15 PE ) Average amplitude across the camera ~1200 is  Flasher event
const int PixelSurviveCutOff = 3; //How many pixels need to survive cleaning to plot 
const int LWRatioCutOff = 50 ; // length width How elliptical you require the events to be after the Principle Compomnent Analysis (helps removed crosstalk events.) 
const int rmTopRow = 1 ; // removed the top row of the camera (sky) 0 = remove, 1 = dont remove 

void SetBranches(Event *evD);
void SetBranchesHLED(Event *evD);
void LoadEvents(std::string filename, std::string treeString);
void LoadEventsHLED(string NameofFile, std::string treeString);
void removeDuplicates(std::vector<int>& arr);
void removeDuplicates(std::vector<double>& arr);
void CleanedPlot(TCanvas* c_cleaned, TH2F* hcam1, TH2F* hcam2, TH2F* hcam3, TH2F* hcam4, double avg_amp,int maxpixelnumberTimeBin, int maxMUSICnumber, double conc, TVectorD eigenVals, TMatrixD eigenVecs, std::vector<double> sigmas, int Cleaned_count, double Cleaned_total_amp);
Double_t Median(vector<int> v);
std::vector<double> readFileToVector(const std::string& filename);
void savePlot(TCanvas* c_cleaned,TH1* hist, std::string outDir,std::string folString, TFile* file,std::string plotname);
void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp);
double StartPanel4(PCA& pca, TVectorD& eigenVals, TMatrixD& eigenVecs);
void CompletePanel4(PCA& pca, TH2F* hcam_panel4, CEvent* cev, double EllipicRatio, TVectorD& eigenVals, TMatrixD& eigenVecs);
std::vector<double> getM3Long(double xcog,double ycog, std::vector<int> sur_pix, std::vector<float> amps);
std::vector<double> generateRandomNumbers();



void removeDuplicates(std::vector<int>& arr) {
    // Sort the array
    std::sort(arr.begin(), arr.end());

    // Use std::unique to remove duplicates
    auto last = std::unique(arr.begin(), arr.end());

    // Resize the vector to remove the undefined tail elements
    arr.erase(last, arr.end());
}

void removeDuplicates(std::vector<double>& arr) {
    // Sort the array
    std::sort(arr.begin(), arr.end());

    // Use std::unique to remove duplicates
    auto last = std::unique(arr.begin(), arr.end());

    // Resize the vector to remove the undefined tail elements
    arr.erase(last, arr.end());
}

void LoadEvents(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    cout << "Loading file: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void LoadEventsHLED(string NameofFile, std::string treeString)
{
    NameofFile = NameofFile;
    //cout << "Loading file for HLED: " << NameofFile << endl;
    TFile *fO = new TFile(NameofFile.c_str(), "READ");
    treeHLED = (TTree*)fO->Get(treeString.c_str());
}


void SetBranches(Event *evD)
{
    tree->SetBranchAddress("Events", &ev);
}

void SetBranchesHLED(Event *evD)
{
    treeHLED->SetBranchAddress("Events", &evHLED);
}



// Function to check if a branch exists in a ROOT file
bool isBranchPresentInFile(const std::string& fileName, const std::string& treeName) {
    // Open the ROOT file
    TFile* file = TFile::Open(fileName.c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open the .root file: " << fileName << std::endl;
        delete file;
        return false;
    }

    // Check if the tree exists
    bool treeExists = (file->Get(treeName.c_str()) != nullptr);
    if (!treeExists) {
        std::cout << "Tree '" << treeName << "' is not present in the file. Skipping..." << std::endl;
    }

    // Clean up
    file->Close();
    delete file;

    return treeExists; // Return whether the tree exists
}