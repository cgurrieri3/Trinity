#include "TrinitySimUtilities.h"

using namespace std;


////////////////////////////////////////////////////////////////////
// Main Program
//
//////////////////////////////////////////////////////////////////
int main (int argc, char **argv) {


  //initiate root
  TROOT root("DisplayEvts","Display Results");
  TApplication *theApp = new TApplication("App",&argc,argv);
  gROOT->ProcessLine("#include <vector>"); //need this otherwise we cannot save vectors in the root file

  TrinitySimUtilities *trinityUtils = new TrinitySimUtilities();

  trinityUtils->ConfigureDemonstrator(); 
  trinityUtils->SetMaximumShowerViewingAngle(20);

  //plot the probability that a tau emerges when a neutrino hits the Earth
  //trinityUtils->PlotEmergenceProbability(); 
  //cout<<"Done with Emergence Probability Plot"<<endl;
  
  trinityUtils->SetLowerNeutrinoEnergy(6.0);
  trinityUtils->SetUpperNeutrinoEnergy(10.0);

  Double_t logEmin = 6; //min energy log
  Double_t logEmax = 10; //max energy log
  TGraph *grDiffAcceptance = new TGraph();
  //trinityUtils->GetDiffuseAcceptanceLimitedFoV(logEmin,logEmax,grDiffAcceptance);//ok
  //trinityUtils->GetDiffuseAcceptance(logEmin,logEmax,grDiffAcceptance); //ok
  //trinityUtils->CalculateDiffuseFluxDifferentialSensitivity();
  //trinityUtils->CalculateIntegralSensitivity();

  trinityUtils->SetMaximumAzimuthAngle(10); //log10 [GeV] 
  trinityUtils->DumpConfiguration();
  trinityUtils->GetPointSourceAcceptanceInFOV(logEmin, logEmax); //ok
  //trinityUtils->GetPointSourceAcceptanceSingleAngle(logEmin,logEmax); //ok
  //trinityUtils->SetNeutrinoEnergyStep(0.5); //log10 [GeV] 
  //trinityUtils->SetIntegrationRangeForSensitivity(0.5); //log10 [GeV] 
  //trinityUtils->CalculatePointSourceDifferentialSensitivity(); //running
/*
  TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
  timer.TurnOn();
  Getline("Type <return> to go on: ");
  timer.TurnOff();
*/
 cout<<"done"<<endl;
 theApp->Run();
 
 return 0;
}
