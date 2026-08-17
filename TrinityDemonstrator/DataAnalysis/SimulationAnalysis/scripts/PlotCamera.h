#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TBox.h>
#include <set>
#include <TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include "TLine.h"
#include <fstream>
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
#include <Event.h>
#include <IEvent.h>
#include <IPlotTools.h>
#include <ISiPM.h>
#include "SimRunData.h"




IUtilities *util;
IPlotTools *plottools;
SimRunData *simRun=0;
IEvent *ev=0;
CEvent *cev;
TFile *file;
TFile *fileOutput;
TFile *fileOutputPlots;


TH1F *hPhotonX=0;
TH1F *hPhotonY=0;
TH1F *hPhotonDcosX=0;
TH1F *hPhotonDcosY=0;
TH1F *hPhotonZ=0;
TH2F* hPhotonDist1x=0;
TH2F* hPhotonDist1x6mm=0;
TH2F* hPhotonDist4x=0;

TCanvas *canvas=0;

float PtoPEconversion = 0.4*0.83;



// std::string dataDir = "/storage/hive/project/phy-otte/shared/Trinity/Simulations/TDemSims/";
std::string mnt = "/storage/osg-otte1/shared/TrinityDemonstrator/";
std::string dataDir = "/storage/osg-otte1/shared/TrinityDemonstrator/simdata/roots/diffuse/";
std::string outDir = "/storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/SimulationAnalysis/data/";

// std::string OutputFileRoot=Form("%sSimulations20250529.root",outDir.c_str());

// Define a struct to represent one simulation event
// struct SimEventData {
//     float tb_time;
//     float cobo_time;
//     float unix_time;
//     float eventType;
//     float signalValue;
//     float roiMusicID;
//     float roiPixelID;
// };

// struct SimRunData{
//     float energy;
//     float time;
//     float posX;
//     float posY;
//     // std::vector<float> float posZ;
//     float zenith;
//     float azimuth;
//     float bsm;
//     float tilt;
// };

// struct SimAuxData {
//     Float_t IcorrectionFactor;
//     Float_t sunAzimuth;
//     Float_t sunElevation;
//     Float_t moonAzimuth;
//     Float_t moon_Elevation;
//     Float_t moon_Illumination;
//     Float_t CameraRA;
//     Float_t CameraDEC;
//     Float_t TrigEvent;
//     Float_t hv;
//     Float_t hvc;
//     Float_t ucTemp;
//     Float_t siabMPWR;
//     Float_t hvSW;
//     Float_t sipmTemp;
//     Float_t siabCurr;
//     Float_t ASADcurr;
//     Float_t TBcurr;
//     Float_t outsideTempature;
//     Float_t humidity;
// };
void DrawMUSICBoundaries();
// void create_root_file(TH2F* hist,TFile* file);
void create_root_file(TH2F* hist, TFile* file, std::vector<int>* vFADCTraces[256]);



void DrawMUSICBoundaries()
{
        TBox *b = new TBox(-48.5,-48.5,-36.5,-24.5);
        b->SetFillStyle(0);
        b->SetLineColor(kRed);
        for(int i=1; i<32; i++)
        {
                TBox *bn = (TBox*)b->Clone();
                bn->SetX1((i%8)*12-48.5);
                bn->SetX2((i%8)*12-36.5);
                bn->SetY1((i/8)*24-48.5);
                bn->SetY2((i/8)*24-24.5);
                bn->Draw();
        }
}