#include "SimRunData.h"

using namespace std;

ClassImp(SimRunData);
SimRunData::SimRunData(int size){
    simrun = "noname";
    date = 20231003;
    simEvent = 0;
    triggered = false;
    posX = vector<Float_t>(size);
    posY = vector<Float_t>(size);
    polarangle = 0.0;
    azimuthangle = 0.0;
    telposX = 0.0;
    telposY = 0.0;
    telposZ = 0.0;
    telradius = 0.0;
    XMax = 0.0;
    NMax = 0.0;
    X0Max = 0.0;
    lambda = 0.0;
    neutrinoenergy = 0.0;
    decaylength = 0.0;
    showerstartX = 0.0;
    showerstartY = 0.0;
    showerstartZ = 0.0;
    tauemergenceprob = 0.0;
    EmergencepointX = 0.0;
    EmergencepointY = 0.0;
    EmergencepointZ = 0.0;
    globaltheta = 0.0;
    globalphi = 0.0;
    tauenergy = 0.0;
}

SimRunData::~SimRunData(){

}
// Setters

void SimRunData::SetSimRun(std::string temp){
    simrun = temp;
}

void SimRunData::SetDate(Int_t temp){
    date = temp;
}
void SimRunData::SetSimEventNumber(Int_t temp){
    simEvent = temp;
}

void SimRunData::SetTriggered(Bool_t temp){
    triggered = temp;
}

void SimRunData::SetPolarAngle(Float_t temp){
    polarangle = temp;
}

void SimRunData::SetAzimuthAngle(Float_t temp){
    azimuthangle = temp;
}

void SimRunData::SetTelescope_Xpos(Float_t temp){
    telposX = temp;
}

void SimRunData::SetTelescope_Ypos(Float_t temp){
    telposY = temp;
}

void SimRunData::SetTelescope_Zpos(Float_t temp){
    telposZ = temp;
}

void SimRunData::SetTelescope_Radius(Float_t temp){
    telradius = temp;
}

void SimRunData::SetXMax(Float_t temp){
    XMax = temp;
}

void SimRunData::SetNMax(Float_t temp){
    NMax = temp;
}

void SimRunData::SetX0Max(Float_t temp){
    X0Max = temp;
}

void SimRunData::SetLambda(Float_t temp){
    lambda = temp;
}

void SimRunData::SetNeutrinoEnergy(Float_t temp){
    neutrinoenergy = temp;
}

void SimRunData::SetDecayLength(Float_t temp){
    decaylength = temp;
}

void SimRunData::SetShowerStartX(Float_t temp){
    showerstartX = temp;
}

void SimRunData::SetShowerStartY(Float_t temp){
    showerstartY = temp;
}

void SimRunData::SetShowerStartZ(Float_t temp){
    showerstartZ = temp;
}

void SimRunData::SetTauEmergenceProb(Float_t temp){
    tauemergenceprob = temp;
}

void SimRunData::SetEmergencePointX(Float_t temp){
    EmergencepointX = temp;
}

void SimRunData::SetEmergencePointY(Float_t temp){
    EmergencepointY = temp;
}

void SimRunData::SetEmergencePointZ(Float_t temp){
    EmergencepointZ = temp;
}

void SimRunData::SetGlobeTheta(Float_t temp){
    globaltheta = temp;
}

void SimRunData::SetGlobePhi(Float_t temp){
    globalphi = temp;
}

void SimRunData::SetTauEnergy(Float_t temp){
    tauenergy = temp;
}

void SimRunData::SetPosX(vector<Float_t> temp){
    posX = temp;
}

void SimRunData::SetPosY(vector<Float_t> temp){
    posY = temp;
}

void SimRunData::SetTilt(Float_t temp){
    tilt = temp;
}

void SimRunData::SetAzimuth(Float_t temp){
    azimuth = temp;
}

void SimRunData::SetZenith(Float_t temp){
    zenith = temp;
}

// Getters

std::string SimRunData::GetSumRun(){
    return simrun;
}

Int_t SimRunData::GetDate(){
    return date;
}

Int_t SimRunData::GetSimEventNumber(){
    return simEvent;
}

Bool_t SimRunData::GetTriggered(){
    return triggered;
}

vector<Float_t> SimRunData::GetPosX(){
    return posX;
}

vector<Float_t> SimRunData::GetPosY(){
    return posY;
}

Float_t SimRunData::GetPolarAngle(){
    return polarangle;
}

Float_t SimRunData::GetAzimuthAngle(){
    return azimuthangle;
}

Float_t SimRunData::GetTelescope_Xpos(){
    return telposX;
}

Float_t SimRunData::GetTelescope_Ypos(){
    return telposY;
}

Float_t SimRunData::GetTelescope_Zpos(){
    return telposZ;
}

Float_t SimRunData::GetTelescope_Radius(){
    return telradius;
}

Float_t SimRunData::GetXMax(){
    return XMax;
}

Float_t SimRunData::GetNMax(){
    return NMax;
}

Float_t SimRunData::GetX0Max(){
    return X0Max;
}

Float_t SimRunData::GetLambda(){
    return lambda;
}

Float_t SimRunData::GetNeutrinoEnergy(){
    return neutrinoenergy;
}

Float_t SimRunData::GetDecayLength(){
    return decaylength;
}

Float_t SimRunData::GetShowerStartX(){
    return showerstartX;
}

Float_t SimRunData::GetShowerStartY(){
    return showerstartY;
}

Float_t SimRunData::GetShowerStartZ(){
    return showerstartZ;
}

Float_t SimRunData::GetTauEmergenceProb(){
    return tauemergenceprob;
}

Float_t SimRunData::GetEmergencePointX(){
    return EmergencepointX;
}

Float_t SimRunData::GetEmergencePointY(){
    return EmergencepointY;
}

Float_t SimRunData::GetEmergencePointZ(){
    return EmergencepointZ;
}

Float_t SimRunData::GetGlobeTheta(){
    return globaltheta;
}

Float_t SimRunData::GetGlobePhi(){
    return globalphi;
}

Float_t SimRunData::GetTauEnergy(){
    return tauenergy;
}

Float_t SimRunData::GetTilt(){
    return tilt;
}

Float_t SimRunData::GetAzimuth(){
    return azimuth;
}

Float_t SimRunData::GetZenith(){
    return zenith;
}