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

        void PlothSize(TCanvas* c_cleaned);
        void PlothSizeConc(TCanvas* c_cleaned);
        void PlothSizeSPC(TCanvas* c_cleaned);
        void PlothLW(TCanvas* c_cleaned);
        void PlothLWConc(TCanvas* c_cleaned);
        void PlothLWSPC(TCanvas* c_cleaned);
        void PlothSPCConc(TCanvas* c_cleaned);
        void PlothCOG(TCanvas* c_cleaned);
        void PlothTPID(TCanvas* c_cleaned);

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
    };  
#endif