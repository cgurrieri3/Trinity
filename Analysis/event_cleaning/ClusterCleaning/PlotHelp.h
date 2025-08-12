#ifndef PlotHelp_H
#define PlotHelp_H

#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>
#include <TVectorD.h>
#include <vector>
#include <TH2F.h>
#include <TH1D.h>
#include <TFile.h>
#include <IPlotTools.h>
#include <iostream>
#include <TCanvas.h>
#include <CEvent.h>
#include <TLatex.h>
#include <TMath.h>
#include <TDatime.h>
#include <TLegend.h>
#include "TStyle.h"
#include <THStack.h>


class PlotHelp { 
    public:
        PlotHelp();
        void AddtoWL(double WLRatio);
        void AddtoL(double L);
        void AddtoW(double W);
        void AddtoSize(double TotalAmp); // Distribution of Total Amplitude of surviving pixels
        void AddtoConcentation(double conc); // Distribution of Total Amplitude of surviving pixels
        void AddtoSurvivingPixelCount(int count); // Distribution of Total Amplitude of surviving pixels
        void AddtoCOGx(double COGx); // Distribution of Total Amplitude of surviving pixels
        void AddtoCOGy(double COGy); // Distribution of Total Amplitude of surviving pixels
        void AddtoTriggeredPixelsID(int id); // Distribution of Total Amplitude of surviving pixels
        void AddtoEventDate(int date);
        void AddtoRMSratioAVGamp(double rmsoveravg); // Distribution of Total Amplitude of surviving pixels
        void AddtoCoreRatio(double cr);
        void AddtoNumberOfCores(double num);

        void PlothSize(TCanvas* c, std::string pdf);
        void PlothSizeConc(TCanvas* c, std::string pdf);
        void PlothSizeSPC(TCanvas* c, std::string pdf);
        void PlothWL(TCanvas* c, std::string pdf,std::string outDir,std::string date);
        void PlothdistLandW(TCanvas* c, std::string pdf);
        void PlothLvW(TCanvas* c, std::string pdf);
        void PlothWLConc(TCanvas* c, std::string pdf);
        void PlothWLSPC(TCanvas* c, std::string pdf);
        void PlothSIZEWL(TCanvas* c, std::string pdf);
        void PlothSPCConc(TCanvas* c, std::string pdf);
        void PlothCOG(TCanvas* c, std::string pdf);
        void PlothTPID(TCanvas* c, std::string pdf);
        void PlothRMSratioAvg(TCanvas* c, std::string pdf);
        void PlothEventDateTriggeredPixel(TCanvas* c, std::string pdf);
        void PlothCoreRatio(TCanvas* c, std::string pdf);
        void PlothNumberofCores(TCanvas* c, std::string pdf);
        void PlothCoreRatioandNumberofCores(TCanvas* c, std::string pdf);
        void PlothncoreSIZE(TCanvas* c, std::string pdf);
        void PlothnCoreSPC(TCanvas* c, std::string pdf);
        void PlothCRSPC(TCanvas* c, std::string pdf);
        void PlothSIZEWandL(TCanvas* c, std::string pdf);


        void AddTotalEvent(int i) {TotalEvents += i; }
        void AddHLEDEvent(int i) {HLEDEvents += i; }
        void AddPreCleaned(int i) {PreCleanedEvents += i; }
        void AddCleanedPanel2(int i) {Panel2CleanedEvents += i; }
        void AddCleanedPanel3(int i) {Panel3CleanedEvents += i; }
        void AddSurvivingEvent(int i) {SurvivingEvents += i; }
        inline int GetTotalEvent() {return TotalEvents; }
        inline int GetHLEDEvent() {return HLEDEvents; }
        inline int GetPreCleanedEvent() {return PreCleanedEvents; }
        inline int GetPanel2CleanedEvent() {return Panel2CleanedEvents; }
        inline int GetPanel3CleanedEvent() {return Panel3CleanedEvents; }
        inline int GetSurvivingEvent() {return SurvivingEvents; }
        void LogBinning(TH2F* hist);
        void LogBinning(TH1D* hist);

    private:
        
        
        int GetSequentialDayIndex(int dateStr);

        std::vector<double> WLvector = {}; //Length Width Ratio
        std::vector<double> Lvector = {}; // Length
        std::vector<double> Wvector = {}; // Width 
        std::vector<double> Svector = {}; //Size
        std::vector<double> Cvector = {}; //Concentration
        std::vector<double> Corevector = {}; //Core Ratio
        std::vector<double> NCorevector = {}; //Core Ratio
        std::vector<int> SPCvector = {}; //Surviving Pixel Count
        std::vector<double> COGxvector = {}; //Center of Gravity X
        std::vector<double> COGyvector = {}; //Center of Gravity Y
        std::vector<int> TPID = {}; //Triggered Pixel ID
        std::vector<int> Datevecctor = {}; // date of events
        std::vector<double> RMSoverAVGvector = {}; //RMS over Average Amplitude
        IPlotTools *plottools;

        int TotalEvents = 0;
        int HLEDEvents = 0;
        int PreCleanedEvents = 0;
        int Panel2CleanedEvents = 0;
        int Panel3CleanedEvents = 0;
        int SurvivingEvents = 0;

        // plot help with step size, min and max for TH*
        int SIZEstep =50;
        float SIZEmin = 1; // in log scale
        float SIZEmax = 5;

        int ONEstep = 22;
        float ONEmin = -0.05;
        float ONEmax = 1.05;
        
        int SPstep = 28;
        float SPmin = -0.5;
        float SPmax = 27.5;

        int CameraStep = 16;
        float CameraMin = -0.5;
        float CameraMax = 15.5;

        int nCoreStep = 20;
        float nCoreMin = 0.5;
        float nCoreMax = 21.5;

        

        
    };  
#endif