#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include "EventInfo.h"
#include "SimRunData.h"
#include <TPaveText.h>
#include <iostream>
#include  <TCanvas.h>
#include "PlotHelp.h"
#include <IUtilities.h>
#include <IPlotTools.h>
#include <TArrow.h>
#include <TLatex.h>
#include <algorithm>
#include <vector>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TEllipse.h>
// #include "libExACT.so"

using namespace std;



IPlotTools *plottools = new IPlotTools();
PlotHelp *plothelp = new PlotHelp(); // initialize the plot help class in scope for whole program
IUtilities *util = new IUtilities();;

// Shared output file names used throughout eventSelection.cpp
const std::string OutputPdfFile  = "EventSelectionPlots.pdf";
const std::string OutputRootFile = "EventSelectionPlots.root";

// Mount point holding the data; defaults to /mnt and may be overridden by argv[1]
std::string mnt = "/mnt";

// Command line configuration (set from argv in main)
std::string mount;
std::string folder;
std::string file_list; // used to sort door open and door closed files and such

// Run-wide event counters
int dooropen = 0;
int doorclosed = 0;

// add for loop all files in a directory
std::string FolderPath;
std::string AllowedFilesPath;
std::vector<std::string> fileNamesVec;
std::vector<std::string> allowedFilesVec;