//R__LOAD_LIBRARY(libExACT.so)
//#include "libExACT.so"
#include "PCA.h"
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

// Define variables (global)
std::vector<double> MaxAmplitudePixel;
std::vector<int> MaxPixelID;
std::vector<int> MaxPeakTimePixel;
std::vector<int> MaxMUSICID;
std::vector<double> MaxAmplitudePixelTimeBin;
std::vector<int> MaxPixelIDTimeBin;
std::vector<double> AvgAmplitudePerEvent;
std::vector<double> CameraAmplitudeAtTimeBin;



TTree *tree = 0;
TTree *treeHLED = 0;
Event *ev;
Event *evHLED;
TH2F* COGgraph=0;
TH2F* COGgraphweighted=0;
TH2F* pixeldist=0;

TFile *fO;
TFile *file;

TCanvas *c_cleaned =0;
TCanvas *c_PCA=0;
int MaxNofChannels = 256;

std::string neighborDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/ClusterCleaning/neighbors/";
std::string CalibrationFactorDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/FlatFieldCalibration/Output/";
std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Data/";
// std::string dataDir = "/storage/hive/project/phy-otte/sstepanoff3/Data/";
std::string outDir = "/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/EventCleaning/ClusterCleaning/Output/";
// std::string outDir = "/storage/hive/project/phy-otte/sstepanoff3/EventCleaning/ClusterCleaning/Output/";


// Initialize functions
std::vector<int> GetNeighborArray(int id, std::string filename);
void NeighborCheck(int pixelID, std::vector<int>& ampattime, std::vector<int>& correctPixelIDs, std::set<int>& addedIDs, const std::vector<double>& AmplitudeValues, const std::vector<int>& peakTimeValues, int& firstPixID,std::vector<double>& PedVal);
void TriggeredPixelNeighborhoodChecker(std::vector<double> AmplitudesSelected,std::vector<int>& IDSelected, int triggeredpixel,std::vector<double>& cleaned_pixels);
void NeighborhoodCheckerHelper(int ID, std::vector<int>& IDSelected, std::vector<double> AmplitudesSelected,std::vector<double>& cleaned_pixels, std::vector<int>& visited_ids);
void SaveAmplitudesToCSV(const std::string& fileName, int eventIndex, double allAmplitudes);
Bool_t HandleInput();
void NeighborCheckHelper(int arrayID, std::vector<int>& ampattime, std::vector<int>& correctPixelIDs, std::set<int>& addedIDs, const std::vector<double>& AmplitudeValues, const std::vector<int>& peakTimeValues, int& firstPixID,std::vector<double>& PedVal);
void SetBranches(Event *evD);
void SetBranchesHLED(Event *evD);
void LoadEvents(std::string filename, std::string treeString);
void LoadEventsHLED(string NameofFile, std::string treeString);
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();
vector<int> FindNeighborPixels(int FirstTrigMusic);
int findMUSICIndex(int MUSIC);
int findMUSICPair(int MUSICID);
void getSIABTriggeredInfo(Event *ev,std::vector<int>& Max_pixel, std::vector<double>& Max_Amp, std::vector<int>& Max_MUSIC, std::vector<int>& peak_time,std::vector<double>& Avg_Amp, std::vector<double>& Max_Amp_Time_Bin, std::vector<int>& Max_pixel_Time_Bin);
void getPixelInfo(Event *ev, std::vector<double>& AmplitudeValues, std::vector<int>& peakTimeValues, std::vector<double>& PedestalValues, int* SC, int* totalAPE, std::vector<std::vector<int>>& traceValues);
std::vector<std::string> read_directory( const std::string& path = std::string());
void removeDuplicates(std::vector<int>& arr);
void removeDuplicates(std::vector<double>& arr);
void CleanedPlot(TCanvas* c_cleaned, TH2F* hcam1, TH2F* hcam2, TH2F* hcam3, TH2F* hcam4, double avg_amp,int maxpixelnumberTimeBin, double conc, TVectorD eigenVals, TMatrixD eigenVecs, std::vector<double> sigmas, int Cleaned_count, double Cleaned_total_amp);
Double_t Median(vector<int> v);
std::vector<double> readFileToVector(const std::string& filename);
void savePlot(TCanvas* c_cleaned,TH1* hist, std::string outDir,std::string folString, TFile* file,std::string plotname);
void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp, std::vector<double> *COG);
int convertADC2PE(int ADC_counts);
double convertADC2PE(double ADC_counts);
int checkTopRow(int value);
std::vector<double> generateRandomNumbers();
bool isBranchPresentInFile(const std::string& fileName, const std::string& treeName);

// get files sorted
std::vector <std::string> read_directory(const std::string& path){
	std::vector <std::string> result;
	dirent* de;
	DIR* dp;
	errno = 0;
	dp = opendir( path.empty() ? "." : path.c_str() );
	if (dp)
	{
	while (true)
		{
		errno = 0;
		de = readdir( dp );
		if (de == NULL) break;
		
		result.push_back( Form("%s%s",path.c_str(),de->d_name) );
		}
	closedir( dp );
	std::sort( result.begin(), result.end() );
	}
	return result;
}

int convertADC2PE(int ADC_counts){
    int ADCtoPEratio = 24;
    return ADC_counts/ADCtoPEratio;
}

double convertADC2PE(double ADC_counts){
    double ADCtoPEratio = 24;
    return ADC_counts/ADCtoPEratio;
}

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

//Draws red boxes to make obvious which pixels are associated with the same MUSIC chip
void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,1.5,3.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=1; i < MaxNofChannels/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetX1((i%8)*2-0.5);
		bn->SetX2((i%8)*2+1.5);
		bn->SetY1((i/8)*4-0.5);
		bn->SetY2((i/8)*4+3.5);
		bn->Draw();
	}
    delete b;
}

void FindBin(int pixelID, int *nx, int *ny)
{
	// Calculate the SIAB number (0 to 15)
	int SIAB_Number = pixelID / 16;

        // Calculate the pixel number within the SIAB (0 to 15)
	int SIAB_Pixel_Number = pixelID % 16;

        // Calculate the row and column within the SIAB (0 to 3 for both)
	int SIAB_Pixel_Row = SIAB_Pixel_Number % 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number / 4;

        // Calculate the overall row and column	
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
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