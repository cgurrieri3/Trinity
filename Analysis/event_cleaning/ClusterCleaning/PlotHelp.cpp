#include "PlotHelp.h"


// Constructor
PlotHelp::PlotHelp() {
}


void PlotHelp::AddtoLW(double LWRatio){
    LWvector.push_back(LWRatio);
}

void PlotHelp::AddtoSize(double size){
    Svector.push_back(size);
}

void PlotHelp::AddtoConcentation(double conc){
    Cvector.push_back(conc);
}

void PlotHelp::AddtoSurvivingPixelCount(int count){
    SPCvector.push_back(count);
    
}

void PlotHelp::AddtoCOGx(double COGx){
    COGxvector.push_back(COGx);
}
void PlotHelp::AddtoCOGy(double COGy){
    COGyvector.push_back(COGy);
}

void PlotHelp::AddtoTriggeredPixelsID(int id){
    TPID.push_back(id);
}

void PlotHelp::PlothLW(TCanvas* c, std::string pdf){
    TH1D* hLW = new TH1D("hLW", "Distribution of Length Width Ratio",10 ,-0.05 ,0.95);
    hLW->SetStats(0);
    hLW->SetXTitle("Length Width Ratio");
    for (std::vector<double>::size_type h = 0; h < LWvector.size(); h++) {
        hLW->Fill(LWvector[h]);
    }
    // hLW->SetXaxis()->SetLabelSize(0.03);
    // hLW->SetXaxis()->SetTitleOffset(1.2); // Adjust X-axis title offset
    // hLW->SetYaxis()->SetTitleOffset(1.5); // Adjust Y-axis title offset
    c->cd(0);
    hLW->Draw();
    //Create a TLatex object to display the label
    TLatex *latex = new TLatex();
    latex->SetNDC(); // Use normalized coordinates
    latex->SetTextSize(0.02); // Set the text size

    latex->DrawLatex(0.7, 0.85, Form("Events #: %i", TotalEvents-HLEDEvents));
    latex->DrawLatex(0.7, 0.83, Form("Flasher Events #: %i", HLEDEvents));
    latex->DrawLatex(0.7, 0.81, Form("Survived Events #: %i", SurvivingEvents));
    delete latex;
    c->Update();
    c->Write("LW_Distribution");
    hLW->Write("LW_DistributionTH1D");
    c->Print(pdf.c_str());
    delete hLW;
}

void PlotHelp::PlothSize(TCanvas* c, std::string pdf){
    TH1D* hSize = new TH1D("hSize", "Size Distribution of Surviving Pixels", SIZEstep, SIZEmin,SIZEmax );
    hSize->SetStats(0);
    hSize->SetXTitle("SIZE (PEs)");
    for (std::vector<double>::size_type h = 0; h < Svector.size(); h++) {
        hSize->Fill(Svector[h]);
    }
    c->cd(0);
    hSize->Draw();
    c->Update();
    c->Write("Size_Distribution");
    hSize->Write("Size_DistributionTH1D");
    c->Print(pdf.c_str());
    delete hSize;
}

void PlotHelp::PlothSizeConc(TCanvas* c, std::string pdf){
    TH2F* hConc = new TH2F("hConc", "SIZE vs Concentation; CONC; SIZE",ONEstep, ONEmin, ONEmax, SIZEstep, SIZEmin,SIZEmax);
    hConc->SetStats(0);

    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hConc->Fill(Cvector[h], Svector[h]);
    }
    c->cd(0);
    hConc->Draw("COLZ");
    c->Update();
    c->Write("Conc_Size");
    hConc->Write("Conc_SizeTH2F");
    c->Print(pdf.c_str());
    delete hConc;
}

void PlotHelp::PlothLWConc(TCanvas* c, std::string pdf){
    TH2F* hLWConc = new TH2F("hLWConc", "LW vs Concentation; LW; CONC",ONEstep, ONEmin, ONEmax,ONEstep, ONEmin, ONEmax);
    hLWConc->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hLWConc->Fill(LWvector[h],Cvector[h] );
    }
    c->cd(0);
    hLWConc->Draw("COLZ");
    c->Update();
    c->Write("LW_Conc");
    hLWConc->Write("LW_ConcTH2F");
    c->Print(pdf.c_str());
    delete hLWConc;
}


void PlotHelp::PlothLWSPC(TCanvas* c, std::string pdf){
    TH2F* hLWSPC = new TH2F("hLWSPC", "Surviving pixels vs Length Width Ratio; LW ratio; Surviving Pixels",ONEstep, ONEmin, ONEmax, SPstep, SPmin, SPmax);
    hLWSPC->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hLWSPC->Fill(LWvector[h],SPCvector[h] );
    }
    c->cd(0);
    hLWSPC->Draw("COLZ");
    c->Update();
    c->Write("LW_SPC");
    hLWSPC->Write("LW_SPCTH2F");
    c->Print(pdf.c_str());
    delete hLWSPC;
}

void PlotHelp::PlothSPCConc(TCanvas* c, std::string pdf){
    TH2F* hSPCConc = new TH2F("hSPCConc", "Surviving pixels vs Concentation; Conc; Surviving Pixels",ONEstep, ONEmin, ONEmax, SPstep, SPmin, SPmax);
    hSPCConc->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Cvector.size(); h++) {
        hSPCConc->Fill(Cvector[h], SPCvector[h]);
    }
    c->cd(0);
    hSPCConc->Draw("COLZ");
    c->Update();
    c->Write("SPC_Conc");
    hSPCConc->Write("SPC_ConcTH2F");
    c->Print(pdf.c_str());
    delete hSPCConc;
}

void PlotHelp::PlothSizeSPC(TCanvas* c, std::string pdf){
    TH2F* hSizeSPC = new TH2F("hSizeSPC", "Distribution of # of pixels that surived cleaning over events; Size; Surviving Pixels",SPstep,SPmin,SPmax, SIZEstep, SIZEmin,SIZEmax);
    hSizeSPC->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < Svector.size(); h++) {
        hSizeSPC->Fill(SPCvector[h], Svector[h]);
    }
    c->cd(0);
    hSizeSPC->Draw("COLZ");
    c->Update();
    c->Write("Size_SPC");
    hSizeSPC->Write("Size_SPCTH2F");
    c->Print(pdf.c_str());
    delete hSizeSPC;
}

void PlotHelp::PlothCOG(TCanvas* c, std::string pdf){
    TH2F* hCOG = new TH2F("hCOG", "Center of Gravity;Pixel Bin along x axis of Camera;Pixel Bin along y axis of Camera", CameraStep, CameraMin, CameraMax, CameraStep, CameraMin, CameraMax);
    hCOG->SetStats(0);
    for (std::vector<double>::size_type h = 0; h < COGxvector.size(); h++) {
        hCOG->Fill(COGxvector[h], COGyvector[h]);
    }
    c->cd(0);
    hCOG->SetMarkerStyle(20);   // Set marker style
    hCOG->SetMarkerColor(kGreen); // Green color for certain points
    hCOG->SetLineColor(kGreen);
    hCOG->Draw("SCAT");
    plottools->DrawMUSICBoundaries();
    c->Update();
    c->Write("COG");
    hCOG->Write("COGTH2F");
    c->Print(pdf.c_str());
    delete hCOG;
}

void PlotHelp::PlothTPID(TCanvas* c, std::string pdf){
    TH2F* hTPID = new TH2F("hTPID", "Triggered Pixels Of Surviving Events;Pixel Bin along x axis of Camera;Pixel Bin along y axis of Camera", CameraStep, CameraMin, CameraMax, CameraStep, CameraMin, CameraMax);
    hTPID->SetStats(0);
    for (std::vector<int>::size_type h = 0; h < TPID.size(); h++) {
        int nx, ny;
        plottools->FindBin(TPID[h], &nx, &ny);
        hTPID->Fill(nx,ny, 1);
    }
    c->cd(0);
    hTPID->Draw("COLZ");
    plottools->DrawMUSICBoundaries();
    c->Update();
    c->Write("TPID");
    hTPID->Write("TPIDTH2F");
    c->Print(pdf.c_str());
    delete hTPID;
}