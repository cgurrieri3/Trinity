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
std::string OutputFileRoot = "";
std::string OutputFilePDF = "";
std::string OutputFilePDFOpen = "";
std::string OutputFilePDFClose = "";




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

void LoadDataPCA(PCA& pca, TH2F* hist, int totalAmp);
double StartPanel4(PCA& pca, TVectorD& eigenVals, TMatrixD& eigenVecs);
void CompletePanel4(PCA& pca, TH2F* hcam_panel4, CEvent* cev, double EllipicRatio, TVectorD& eigenVals, TMatrixD& eigenVecs);
std::vector<double> getM3Long(double xcog,double ycog, std::vector<int> sur_pix, std::vector<float> amps);
std::vector<double> generateRandomNumbers();
void CreateFileName(std::string filename);

void CreateFileName(std::string filename) {
    OutputFileRoot=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.root",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
    );

    // Create the file name with the cariables in the name for sorting of them. 
    // _TC_#_ = TriggeredChannelAmpCutOff
    // TB_#_ = TimeBinAll
    // NP_#_ = CorePixelAmpCutOff
    // s_#_ = SaturatedPixelCutoff
    // FA_#_ = FlasherEventsCutOff
    // mp_# = PixelSurviveCutOff
    OutputFilePDF=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
    );

    OutputFilePDFOpen=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf[",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
    );

    OutputFilePDFClose=Form("%sEventCleanedCluster%s_TC_%i_TB_%i_NP_%i_s_%i_FA_%i_mp_%i_er_%i_tr_%i.pdf]",
        outDir.c_str(),
        filename.c_str(),
        TriggeredChannelAmpCutOff,
        TimeBinAll,
        CorePixelAmpCutOff, 
        SaturatedPixelCutoff,
        FlasherEventsCutOff,
        PixelSurviveCutOff,
        LWRatioCutOff,
        rmTopRow
    );
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
        cev->GetSurvivingPixelTotalAmpPanel3()/util->GetADCtoPEratio(),
        conc,
        M3Longx.data(),
        M3Longy.data()));
        delete title;
        
    plothelp->AddtoLW(EllipicRatio);
    plothelp->AddtoSize(cev->GetSurvivingPixelTotalAmpPanel3()/util->GetADCtoPEratio());
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