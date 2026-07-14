R__LOAD_LIBRARY(libExACT.so)
#include<TH1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <SEvent.h>

using namespace std;


int iLastPix = -1;
int eventCounter = 0;
int eventFail = 0;
TLatex *text = 0;
TTree *tree = 0;
// The branch holds either an Event (raw data) or an IEvent (simulations /
// auxilliary data). IEvent derives from Event, so everything below reads the
// event through evBase; only one of evPlain/evSims is ever allocated.
// ROOT keeps the address of the pointer itself, so these must stay in scope.
Event *evPlain = 0;
IEvent *evSims = 0;
Event *ev = 0;
bool isIEvent = false;

// Simulation  lives in its own "Sim" tree (branch "GrOptics", one SEvent
// per event, indexed 1:1 with the Events tree). Real data files have no such
// tree, so its presence is what marks a file as a simulation.
TTree *simTree = 0;
SEvent *sEvent = 0;
bool isSim = false;
TPaveText *simInfoBox = 0;

IPlotTools *plottools;
IUtilities *util;

TCanvas *c_disp = 0;
TH1D *hPixelTrace = 0;

int MaxNofChannels = 256;
int LengthofPedestal = 200;
int SignalStart = 230;
int SignalWidth = 20;

TButton *but1;
bool ADC = true; // simulations are recorded in PE, so a sim file flips this to false

TH2F *hCam;
TH1F *hAmp;
    

Bool_t HandleInput()
{
	TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
	while (1)
	{
        	timer.TurnOn();
        	TString input = Getline("Type 'q' to exit, <return> to go on:\n");
        	timer.TurnOff();
        	if (input=="q\n") return kFALSE;
        	if (input=="\n") return kTRUE;
	}
	return kFALSE;
}

void PlotTrace(int iPix)
{
    if(hPixelTrace == 0)
    {
        hPixelTrace = new TH1D("hPixelTrace","Pixel Trace",500,-0.5,499.5);
        hPixelTrace->SetStats(0);
        hPixelTrace->GetXaxis()->SetTitle("ADC sample");
        hPixelTrace->GetYaxis()->SetTitle("ADC counts");     
    }

    hPixelTrace->Reset();
    if(ADC == true){
        hPixelTrace->GetYaxis()->SetTitle("ADC counts");
    } else {
        hPixelTrace->GetYaxis()->SetTitle("PE");
        
    }

    TString title;
    title.Form("Trace of Pixel %i", iPix);
    hPixelTrace->SetTitle(title);
    vector<Int_t> trace = vector<Int_t>(MaxNofChannels);

    trace  = ev->GetSignalValue(iPix);
    for(int k = 0; k<512; k++){
        if(ADC==true){
            hPixelTrace->SetBinContent(k+1,trace[k]);
        } else {
            hPixelTrace->SetBinContent(k+1,util->convertADC2PE(trace[k]));
        }
 
        
    }

    c_disp->cd(3);
    hPixelTrace->Draw();
    gPad->Modified();
    gPad->Update();
}

void ShowInfoAtCursor(int x, int y)
{
 
    TString statusline;
    statusline.Form("Pixel: %i, MUSIC: %i, SIAB: %i", plottools->FindPixel(x,y), plottools->FindMUSIC(x,y), plottools->FindSIAB(x,y));
    if(text!=0)
        text->Delete();
    TLatex T1;
    text = T1.DrawLatexNDC(0.25,0.95,statusline.Data());
    gPad->Modified();
    gPad->Update();
}


void PixelClicked()
{
    int event = gPad->GetEvent();
    TObject *o = gPad->GetSelected();
    if (!o){return;}
    if (!(o->InheritsFrom("TH2"))){return;}
    TH2F *h = (TH2F*)o;
    int px = gPad->GetEventX();
    int py = gPad->GetEventY();
    Float_t xx = gPad->AbsPixeltoX(px);
    Float_t yy = gPad->AbsPixeltoY(py);
    Float_t x = 0.5+gPad->PadtoX(xx);
    Float_t y = 0.5+gPad->PadtoY(yy);
    int pix = plottools->FindPixel((int)x,(int)y);
    if(pix!=iLastPix)
    {
        iLastPix = pix;
        ShowInfoAtCursor((int) x, (int) y);
    }
    if (event == 11)
    {
        PlotTrace(pix);
    }
}


// hAmp is filled in whichever unit is currently selected, so its range and title
// have to follow the ADC/PE toggle. SetBins also clears the contents.
void ConfigureAmpHist()
{
    if(hAmp == 0){return;}
    if(ADC == true){
        hAmp->SetBins(410, 0, 4096);
        hAmp->GetXaxis()->SetTitle("Signal Max Amplitude [ADC]");
    } else {
        hAmp->SetBins(410, 0, 4096.0/IUtilities::GetADCtoPEratio());
        hAmp->GetXaxis()->SetTitle("Signal Max Amplitude [PE]");
    }
}

// Fill the camera map and the amplitude distribution for the event currently
// loaded in ev, in the selected unit. Called on every event and again whenever
// the ADC/PE button is pushed, so both histograms always agree with the button.
void FillHistograms()
{
    if(ev == 0 || hCam == 0 || hAmp == 0){return;}

    hCam->Reset();
    ConfigureAmpHist();

    Pulse *pulse;
    for(int i = 0; i < MaxNofChannels; i++){
            pulse = new Pulse(ev->GetSignalValue(i));
            int nx, ny;
            plottools->FindBin(i,&nx,&ny);
            float amplitude = pulse->GetAmplitude();
            if(ADC == false){amplitude = util->convertADC2PE(amplitude);}
            hCam->SetBinContent(nx+1,ny+1,amplitude);
            hAmp->Fill(amplitude);
            delete pulse;
    }
}

// Read the SEvent  for this event and show it on the (otherwise unused)
// fourth pad. Distance and emergence angle are not stored on SEvent; they are
// derived from the azimuth angle and the telescope position.
void ShowSimInfo(int iEvent)
{
    if(!isSim){return;}
    if(iEvent >= simTree->GetEntries()){return;}

    simTree->GetEntry(iEvent);

    double emergenceAngle    = IUtilities::GetEmergenceAngle(sEvent->GetAzimuthAngle());
    double emergenceDistance = IUtilities::GetEmergenceDistance(sEvent->GetTelescope_Xpos(),
                                                                sEvent->GetTelescope_Ypos(),
                                                                sEvent->GetTelescope_Zpos());

    cout << "Sim Run: " << sEvent->GetSumRun()
         << ", Date: " << sEvent->GetDate()
         << ", Sim Event: " << sEvent->GetSimEventNumber() << endl;
    cout << "Neutrino Energy: " << sEvent->GetNeutrinoEnergy() << " GeV"
         << ", Emergence Distance: " << emergenceDistance << " m"
         << ", Emergence Angle: " << emergenceAngle << " deg"
         << ", Number of Photons: "<< (sEvent->GetPosX()).size() << endl; 

    c_disp->cd(4);
    if(simInfoBox == 0){
        simInfoBox = new TPaveText(0.05,0.05,0.95,0.95,"NDC");
        simInfoBox->SetFillColor(0);
        simInfoBox->SetTextAlign(12);
    }
    simInfoBox->Clear();
    simInfoBox->AddText("Simulation (SEvent)");
    simInfoBox->AddText(Form("Sim Run: %s", sEvent->GetSumRun().c_str()));
    simInfoBox->AddText(Form("Date: %i", sEvent->GetDate()));
    simInfoBox->AddText(Form("Sim Event: %i", sEvent->GetSimEventNumber()));
    simInfoBox->AddText(Form("Neutrino Energy: %.3g GeV", sEvent->GetNeutrinoEnergy()));
    simInfoBox->AddText(Form("Emergence Distance: %.3g m", emergenceDistance));
    simInfoBox->AddText(Form("Emergence Angle: %.3g deg", emergenceAngle));
    simInfoBox->AddText(Form("Number of Photons: %zu", (sEvent->GetPosX()).size()));
    simInfoBox->Draw();
    c_disp->cd(4)->Modified();
    c_disp->cd(4)->Update();
}

void PlotEvent()
{
    c_disp->cd(1);
    gPad->AddExec("ev","PixelClicked()");

    hCam = new TH2F("hCam","",16,-0.5,15.5,16,-0.5,15.5);
    hCam->SetStats(0);
    hCam->Draw("colz");
    plottools->DrawMUSICBoundaries();
    int nEntries = tree->GetEntries();
    cout << "Total Number of Events: " << nEntries << endl;

    hAmp = new TH1F("hAmp","Amplitude Distribution",410, 0, 4096);
    hAmp->SetStats(0);
    hAmp->GetYaxis()->SetTitle("No. of Pixels");

    while(1){
        tree->GetEntry(eventCounter);
        // ROOT may replace the object it hands back, so refresh the base pointer
        ev = isIEvent ? (Event*)evSims : evPlain;
        cout << "Event# " << eventCounter << " is displayed." << endl;
        int MUSICpos = (ev->GetROIMusicID())[0];
        cout << "The MUSIC which triggered the DAQ for event # " << eventCounter << " is " << MUSICpos << endl;

        FillHistograms();
        ShowSimInfo(eventCounter);

        uint64_t convtime = ((uint64_t)ev->GetTBTime() / 100000000ULL);
        //Assuming timeTrig is in the format "seconds since epoch with 10^-7 second resolution"
        time_t epochTime = static_cast<time_t>(convtime); // Convert to seconds
        int microsec = convtime % 1000000;

        // Convert to struct tm in UTC
        tm* utcTime = gmtime(&epochTime);

        // Format and display the UTC time
        cout << "Trigger Time (UTC): ";
        cout << put_time(utcTime, "%Y-%m-%d %H:%M:%S") << "." << setfill('0') << setw(6) << microsec << endl;
        // cout << "time " << ev->GetTBTime() <<" is displayed." << endl;


        c_disp->cd(1);
        c_disp->SetTheta(90);
        c_disp->SetPhi(90);
        hCam->SetMinimum();
        hCam->SetMaximum();
        c_disp->cd(1)->Modified();
        c_disp->cd(1)->Update();

        c_disp->cd(2);
        hAmp->Draw();
        c_disp->cd(2)->Modified();
        c_disp->cd(2)->Update();


        eventCounter++;
        if(eventCounter>=nEntries){eventCounter = 0;}
        if(!HandleInput()){break;}
    }
}

bool SetBranches()
{
    TBranch *branch = tree->GetBranch("Events");
    if(branch == 0)
    {
        cout << "No branch called 'Events' in tree " << tree->GetName() << endl;
        return false;
    }

    TString className = branch->GetClassName();
    cout << "Branch 'Events' holds objects of class " << className << endl;

    if(className == "IEvent")
    {
        isIEvent = true;
        evSims = new IEvent();
        tree->SetBranchAddress("Events", &evSims);
        ev = evSims;
    }
    else if(className == "Event")
    {
        isIEvent = false;
        evPlain = new Event();
        tree->SetBranchAddress("Events", &evPlain);
        ev = evPlain;
    }
    else
    {
        cout << "Unsupported event class '" << className << "', expected Event or IEvent." << endl;
        return false;
    }
    return true;
}

// A simulation file carries a "Sim" tree alongside the events tree. This is
// independent of whether the events are Event or IEvent: IEvent marks auxilliary
// (weather / HV / pointing) data, not simulated data.
bool SetSimBranch()
{
    TFile *file = tree->GetCurrentFile();
    if(file == 0){return false;}

    simTree = (TTree*)file->Get("Sim");
    if(simTree == 0){return false;}

    if(simTree->GetBranch("GrOptics") == 0)
    {
        cout << "Tree 'Sim' has no 'GrOptics' branch, treating file as non-simulation." << endl;
        simTree = 0;
        return false;
    }

    sEvent = new SEvent();
    simTree->SetBranchAddress("GrOptics", &sEvent);
    return true;
}

void LoadEvents(string fileName, string treeString)
{
    cout << "Loading file: " << fileName << endl;
    TFile *fO = new TFile(fileName.c_str(), "READ");
    tree = (TTree*)fO->Get(treeString.c_str());
}

void PushButton(){

    if(ADC == true){
        cout<< "Changing to PE" << endl;
        ADC = false;
        but1->SetTitle("ADC");
    }
    else {
        cout<< "Changing to ADC" << endl;
        ADC = true;
        but1->SetTitle("PE");
    }

    // Refill rather than rescale: hAmp needs a new range, and the open trace has
    // to be redrawn in the new unit too.
    FillHistograms();
    c_disp->cd(1)->Modified();
    c_disp->cd(1)->Update();
    c_disp->cd(2)->Modified();
    c_disp->cd(2)->Update();
    if(iLastPix >= 0){PlotTrace(iLastPix);}
}

void TCameraPlotEvents_Trinity_Fancy(string fileName,string treeString)
{   
    
	c_disp = new TCanvas("Display","CameraPlot",750,750);
	c_disp->Divide(2,2);
	LoadEvents(fileName, treeString);
	if(tree == 0)
	{
		cout << "Could not find tree " << treeString << " in " << fileName << endl;
		return;
	}
	if(!SetBranches()){return;}

    isSim = SetSimBranch();
    if(isSim)
    {
        cout << "Simulation file detected ('Sim' tree found): showing signals in PE." << endl;
        ADC = false; // simulations are always in PE
    }

    // The button is labelled with the unit it switches TO, so it shows the unit
    // that is not currently displayed.
    but1 = new TButton(ADC ? "PE" : "ADC","PushButton()",.01,.95,.07,.99);
    but1->SetFillColor(42);
    but1->Draw();

	PlotEvent();
}
