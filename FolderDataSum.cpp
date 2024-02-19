//tells root where to find ExACT libraries. Only needed if using as a macro for root. To compile, you remove this line and include ExACT libraries directly
R__LOAD_LIBRARY(libExACT.so)
//includes libraries used in script
#include <TH1.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <typeinfo>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>

//set "base" directories
//dataDir is where the daily data directories are saved (i.e. "YYYYMMDD" directories)
//outDir is the directory where output will be saved
std::string dataDir = "/home/jordan/Documents/Data/";
std::string outDir = "/home/jordan/Documents/Analysis/";

//initialize or declare global variables, which are allocated static memory and are available in every scope
TTree *tree = 0;
Event *ev;
TCanvas *c_disp = 0;
int MaxNofChannels = 256;

//declare functions to be defined later
void SetBranches(Event **evD);
void FindBin(int pixelID, int *nx, int *ny);
void DrawMUSICBoundaries();

//"main" function; the first function that is called when the script is started. to run in root, this can be any type of function but must be named the same as the .cpp file name. to compile, this would need to be reorganized into a file i.e. int main(int argc char* argv[])
//folString is the name of the directory under dataDir associated with the date that you want to analyze
//treeString is the name of the tree that has the data you want to analyze, i.e. "Test","HLED"
void FolderDataSum(std::string folString, std::string treeString)
{
	//assembles the directory where merged .root data files are saved; assumes that each date (folString) contains a directory "RawDataMerged" where the merged files are saved
	std::string dirname = Form("%s%s/RawDataMerged/",dataDir.c_str(),folString.c_str());
	cout << dirname << std::endl;
	//declare a counter for total number of entries to average over as well as vectors to store the averages of each parameter
	int tEntries = 0;
	//vector of vectors to store pixel averages
	//Means[0] is mead pedestal
	//Means[1] is mean pedestal RMS
	//Means[2] is mean amplitude during triggered events
	//Means[3] is the mean number of charges recorded during events
	//Means[4] is the mean time of the signal peak of each event
	std::vector<std::vector<Double_t>> Means(5,std::vector<Double_t>(MaxNofChannels, 0.0));

	//Initialize/declare vectors to store event times (TB times) and camera averages of parameters. eventMeans indexing follows same order as Means
	std::vector<uint64_t> eventTimes;
	std::vector<std::vector<Double_t>> eventMeans(5,std::vector<Double_t>());

	//Initialize vector of TGraph objects to plot eventMeans on
	TGraph *gEvents = new TGraph();
	std::vector<TGraph *> gDraw;
	//Create histogram objects
	for(int i = 0; i < 5; i++){
		TGraph *gi = new TGraph();
		gDraw.push_back(gi);
	}

	//Declare directory object and dirent struct to use to parse through all files in directory
	//DIR is a directory stream; ordered sequence of all directory entries in a directory
	//dirent is a structure which represents the directory entry at the current position in a directory stream
	DIR *dir;
	struct dirent *ent;
	//Initialize dir as the directory dirname
	if((dir = opendir(dirname.c_str())) != NULL){
		//points to each position in the directory stream dir; readdir returns the current directory stream position and moves dir to the next position; after it has gone through every position, readdir returns NULL
		//iterates through each positon(i.e. file) in dir
		while((ent = readdir(dir)) != NULL){
			//creates string for current file, including directory
			string filename =  Form("%s%s",dirname.c_str(),ent->d_name);
			//checks if file extention is "root" (typically, all files in RawDataMerged directory will be anyway)
			if(filename.substr(filename.find_last_of(".")+1) == "root"){
				cout << "Loading file: " << filename << endl;
				//opens filename
				TFile *f0 = TFile::Open(filename.c_str());
				//sets tree to treeString branch of f0
				tree = (TTree*)f0->Get(treeString.c_str());
				//create new Event object (ExACT type)
				ev = new Event();
				//SetBranches defined below
				SetBranches(&ev);
				//get number entries in current file, add them to running total
				int nEntries = tree->GetEntries();
				tEntries += nEntries;
				std::cout << "Total Number of Events: " << nEntries << std::endl;
				//iterate through each event in file
				for(int EventCounter = 0; EventCounter < nEntries; EventCounter++){
					//Get the tree entry associated with event number EventCounter
					tree->GetEntry(EventCounter);
					//adds TB time of current event to eventTimes
					eventTimes.push_back(ev->GetTBTime()*1e-8);
					//initialize eventMeans value by adding a new value of 0.0
					for(int i = 0; i < 5; i++){
						eventMeans[i].push_back(0.0);
					}
					//Create pulse object pointed named pulse
					Pulse *pulse;
					for(int i = 0; i < MaxNofChannels; i++){
						//Set pulse object pointer to new Pulse object with event signal data
						pulse = new Pulse(ev->GetSignalValue(i));
						//Add values from this event to vectors holding total values to calculate mean
						Means[0][i] += pulse->GetPedestal();
						Means[1][i] += pulse->GetPedestalRMS();
						Means[2][i] += pulse->GetAmplitude();
						Means[3][i] += pulse->GetCharge();
						Means[4][i] += pulse->GetTimePeak();

						eventMeans[0][tEntries-nEntries+EventCounter] += pulse->GetPedestal();
						eventMeans[1][tEntries-nEntries+EventCounter] += pulse->GetPedestalRMS();
						eventMeans[2][tEntries-nEntries+EventCounter] += pulse->GetAmplitude();
						eventMeans[3][tEntries-nEntries+EventCounter] += pulse->GetCharge();
						eventMeans[4][tEntries-nEntries+EventCounter] += pulse->GetTimePeak();
						//have to delete pulse object here to avoid memory leak
						delete pulse;
					}
					//Average eventMeans values; add points to TGraph objects
					for(int i = 0; i < 5; i++){
						eventMeans[i][tEntries-nEntries+EventCounter] /= MaxNofChannels;
						gDraw[i]->SetPoint(tEntries-nEntries+EventCounter,eventTimes[tEntries-nEntries+EventCounter],eventMeans[i][tEntries-nEntries+EventCounter]);
					}
				}
				//have to delete ev, tree, f0 objects here to avoid memory leak
				delete ev;
				delete tree;
				delete f0;
			}
		}
	}
	//Initialize canvas and histograms
	c_disp = new TCanvas("Display","CameraPlot",1250,1000);
	//Initialize vector of titles for plots
	std::vector<std::string> htitles = {"hPed","Average Pedestal","hPedRMS","Average Pedestal RMS","hAmp","Average Amplitude","hCharge","Average Charge","hPeakT","Average Peak Time"};
	//Initialize vector of histogram objects
	std::vector<TH2F *> hDraw;
	//Create histogram objects
	for(int i = 0; i < 5; i++){
		TH2F *hi = new TH2F(htitles[i*2].c_str(),htitles[i*2 + 1].c_str(),16,-0.5,15.5,16,-0.5,15.5);
		hDraw.push_back(hi);
	}

	//Average summed values by dividing by total entries
	for(int i = 0; i < MaxNofChannels; i++){
		//average each pixel/bin
		for(int j = 0; j < 5; j++){
			Means[j][i] /= tEntries;
		}
		int nx, ny;
		//FindBin defined below
		FindBin(i,&nx,&ny);
		//set values for each pixel/bin in histogram
		for(int j = 0; j < 5; j++){
			hDraw[j]->SetBinContent(nx+1,ny+1,Means[j][i]);
		}
	}
	//Begin writing c_disp TCanvas as a pdf; bracket "[" indicates the first page, and subsequent prints to the same file will append as pages
	c_disp->Print(Form("%s%s.pdf[",outDir.c_str(),folString.c_str()));
	//Initialize vector with ranges to use for each histogram
	std::vector<int> hRanges = {3500,4000,0,50,0,500,0,1100,235,255};
	//Loop through each histogram
	for(int i = 0; i < 5; i++){
		//Clear canvas so we can keep reusing the same canvas object
		c_disp->Clear();
		//Set min and max range for bin value gradient
		hDraw[i]->SetMinimum(hRanges[i*2]);
		hDraw[i]->SetMaximum(hRanges[i*2+1]);
		//Don't draw stats box
		hDraw[i]->SetStats(0);
		//Set margin size so palette values aren't clipped
		c_disp->SetRightMargin(0.15);
		//Draw histogram to canvas; colz is defined in THistPainter ROOT documentation
		hDraw[i]->Draw("colz");
		//DrawMUSICBoundaries is defined below
		DrawMUSICBoundaries();
		//Add current canvas as page to output pdf
		c_disp->Print(Form("%s%s.pdf",outDir.c_str(),folString.c_str()));
		c_disp->Clear();
		//Set marker to filled circle/dot
		gDraw[i]->SetMarkerStyle(20);
		//Set TGraph title
		gDraw[i]->SetTitle(htitles[i*2 + 1].c_str());
		//Set X axis to display as a readable time rather than in UNIX time/seconds
		gDraw[i]->GetXaxis()->SetTimeDisplay(1);
		gDraw[i]->GetXaxis()->SetNdivisions(505);
		gDraw[i]->GetXaxis()->SetTimeFormat("%H:%M");
		gDraw[i]->GetXaxis()->SetTimeOffset(0,"gmt");
		gDraw[i]->GetXaxis()->SetTitle("UTC Time of Events [HH:MM]");
		//Draw TGraph using markers
		gDraw[i]->Draw("AP");
		//Add current canvas as page to output pdf
		c_disp->Print(Form("%s%s.pdf",outDir.c_str(),folString.c_str()));
	}
	//Closes the pdf (doesn't add another page)
	c_disp->Print(Form("%s%s.pdf]",outDir.c_str(),folString.c_str()));
}

void SetBranches(Event **evD)
{
	//this really probably does not need to be a defined function since it's just one line. sets branch of tree to "Events"
	tree->SetBranchAddress("Events", evD);
}

//Calculates the 2D bin coordinates associated with a 1D vector/number, i.e. pixel number, based on Trinity camera layout
void FindBin(int pixelID, int *nx, int *ny)
{
	int SIAB_Number = pixelID / 16;
	int SIAB_Pixel_Number = pixelID % 16;
	int SIAB_Pixel_Row = SIAB_Pixel_Number / 4;
	int SIAB_Pixel_Col = SIAB_Pixel_Number % 4;
	*ny = SIAB_Number / 4 * 4 + SIAB_Pixel_Row;
	*nx = SIAB_Number % 4 * 4 + SIAB_Pixel_Col;
}

//Draws red boxes to make obvious which pixels are associated with the same MUSIC chip
void DrawMUSICBoundaries()
{
	//creates TBox object, makes fill transparent and border red, and draws box to active canvas
	TBox *b = new TBox(-0.5,-0.5,3.5,1.5);
	b->SetFillStyle(0);
	b->SetLineColor(kRed);
	b->Draw();
	//Adds a box for each MUSIC chip/position
	for(int i=0; i < MaxNofChannels/8; i++)
	{
		TBox *bn = (TBox*)b->Clone();
		bn->SetY1((i%8)*2-0.5);
		bn->SetY2((i%8)*2+1.5);
		bn->SetX1((i/8)*4-0.5);
		bn->SetX2((i/8)*4+3.5);
		bn->Draw();
	}
}
