#include "SimRunData.h"

using namespace std;

ClassImp(SimRunData);
SimRunData::SimRunData(int size){
	energy = 0;
    posX = vector<Float_t>(size);
    posY = vector<Float_t>(size);
    zenith = 0;
    azimuth = 0;
    bsm = 0;
    tilt = 0; 
}

SimRunData::~SimRunData(){

}

void SimRunData::SetEnergy(Float_t temp){
    energy = temp;
}

void SimRunData::SetZenith(Float_t temp){
    zenith = temp;
}

void SimRunData::SetAzimuth(Float_t temp){
    azimuth = temp;
}

void SimRunData::SetBSM(Float_t temp){
    bsm = temp;
}

void SimRunData::SetTilt(Float_t temp){
    tilt = temp;
}

void SimRunData::SetPosX(vector<Float_t> temp){
    posX = temp;
}

void SimRunData::SetPosY(vector<Float_t> temp){
    posY = temp;
}

Float_t SimRunData::GetEnergy(){
    return energy;
}

Float_t SimRunData::GetZenith(){
    return zenith;
}

Float_t SimRunData::GetAzimuth(){
    return azimuth;
}

Float_t SimRunData::GetBSM(){
    return bsm;
}

Float_t SimRunData::GetTilt(){
    return tilt;
}

vector<Float_t> SimRunData::GetPosX(){
    return posX;
}

vector<Float_t> SimRunData::GetPosY(){
    return posY;
}