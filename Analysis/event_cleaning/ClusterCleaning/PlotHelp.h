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


class PlotHelp { 
    public:
        PlotHelp();
        void AddtoLW(double LWRatio);
        void AddtoSize(double TotalAmp); // Distribution of Total Amplitude of surviving pixels
        void AddtoConcentation(double conc); // Distribution of Total Amplitude of surviving pixels
        void AddtoSurvivingPixelCount(int count); // Distribution of Total Amplitude of surviving pixels
        void AddtoCOGx(double COGx); // Distribution of Total Amplitude of surviving pixels
        void AddtoCOGy(double COGy); // Distribution of Total Amplitude of surviving pixels
        void AddtoTriggeredPixelsID(int id); // Distribution of Total Amplitude of surviving pixels

        void PlothSize(TCanvas* c, std::string pdf);
        void PlothSizeConc(TCanvas* c, std::string pdf);
        void PlothSizeSPC(TCanvas* c, std::string pdf);
        void PlothLW(TCanvas* c, std::string pdf);
        void PlothLWConc(TCanvas* c, std::string pdf);
        void PlothLWSPC(TCanvas* c, std::string pdf);
        void PlothSPCConc(TCanvas* c, std::string pdf);
        void PlothCOG(TCanvas* c, std::string pdf);
        void PlothTPID(TCanvas* c, std::string pdf);

        void AddTotalEvent(int i) {TotalEvents += i; }
        void AddHLEDEvent(int i) {HLEDEvents += i; }
        void AddCleanedEvent(int i) {CleanedEvents += i; }
        void AddSurvivingEvent(int i) {SurvivingEvents += i; }
        inline int GetTotalEvent() {return TotalEvents; }
        inline int GetHLEDEvent() {return HLEDEvents; }
        inline int GetCleanedEvent() {return CleanedEvents; }
        inline int GetSurvivingEvent() {return SurvivingEvents; }

    private:
        std::vector<double> LWvector = {}; //Length Width Ratio
        std::vector<double> Svector = {}; //Size
        std::vector<double> Cvector = {}; //Concentration
        std::vector<int> SPCvector = {}; //Surviving Pixel Count
        std::vector<double> COGxvector = {}; //Center of Gravity X
        std::vector<double> COGyvector = {}; //Center of Gravity Y
        std::vector<int> TPID = {}; //Triggered Pixel ID
        IPlotTools *plottools;

        int TotalEvents = 0;
        int HLEDEvents = 0;
        int CleanedEvents = 0;
        int SurvivingEvents = 0;

        // plot help with step size, min and max for TH*
        int SIZEstep =15;
        float SIZEmin = -0.05;
        float SIZEmax = 1499.95;

        int ONEstep = 10;
        float ONEmin = -0.05;
        float ONEmax = 0.95;
        
        int SPstep = 15;
        float SPmin = 2.95;
        float SPmax = 32.95;

        int CameraStep = 16;
        float CameraMin = -0.5;
        float CameraMax = 15.5;

    };  
#endif