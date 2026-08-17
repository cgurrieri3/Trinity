// #include <TROOT.h>
#ifndef SIMRUNDATA_H
#define SIMRUNDATA_H
#include <vector>
#include "TROOT.h"     // general ROOT stuff
#include "TFile.h"     // for TFile
#include "TTree.h"     // for TTree
#include "TObject.h"
using namespace std;

class SimRunData : public TObject {
    public:
        SimRunData(int size = 1);

        ~SimRunData();

        void SetEnergy(Float_t temp);
        void SetZenith(Float_t temp);
        void SetAzimuth(Float_t temp);
        void SetBSM(Float_t temp);
        void SetTilt(Float_t temp);
        void SetPosX(vector<Float_t> temp);
        void SetPosY(vector<Float_t> temp);

        Float_t GetEnergy();
        Float_t GetZenith();
        Float_t GetAzimuth();
        Float_t GetBSM();
        Float_t GetTilt();
        vector<Float_t> GetPosX();
        vector<Float_t> GetPosY();


    protected:

        Float_t energy;
        vector<Float_t> posX;
        vector<Float_t> posY;
        Float_t zenith;
        Float_t azimuth;
        Float_t bsm;
        Float_t tilt;

        ClassDef(SimRunData, 1);  // ROOT I/O support

};
#endif

