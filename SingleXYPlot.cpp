#include "darkside/Engine.h"

#include <vector>

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"
#include "CalculateEnergy.h"

#include "S2Fitting/S2FitV3.h"

#include "TGraph.h"
#include "TH1.h"
#include "TCanvas.h"

using namespace std;

class XYPlot : public Module {

    vector<float> pulseX;
    vector<float> pulseY;
    vector<float> tdrift;
    vector<float> brightness;

    TGraph* graph;

    TH1* brightnessPerPulse;

    void processEvent(Event& e) {
        brightnessPerPulse = new TH1F("BPP", "brightness per pulse", e.npulses, 0, e.npulses);
        for(int i = 1; i < e.npulses; i++) {
            if(true || (e.pulse_x_masa[i] < 20 && e.pulse_x_masa[i] > -20)) {
                pulseX.push_back(e.pulse_x_masa[i]);
                pulseY.push_back(e.pulse_y_masa[i]);
                tdrift.push_back(e.pulse_start_time[i] - e.pulse_start_time[0]);
                brightness.push_back(e.pulse_total_npe[i]);
                cout << "Pulse start time " << e.pulse_start_time[i] << ", Pulse end time " 
                        << e.pulse_end_time[i] << ", total npe " 
                        << e.pulse_total_npe[i] << endl;
            }
            brightnessPerPulse->SetBinContent(i, e.pulse_total_npe[i]);
            //brightnessPerPulse->SetBinContent(i, e.pulse_total_npe[i] - brightnessPerPulse->Integral());
        }
    }

    void cleanup() {
        TCanvas* canvas = new TCanvas("can", "canvas");
        canvas->Divide(3, 1);
        //canvas->Divide(2, 1);
        canvas->cd(1);
        TGraph* xvst = new TGraph(tdrift.size(), &tdrift[0], &pulseX[0]);
        xvst->SetName("x vs tdrift");
        xvst->SetTitle("X position vs drift time");

        xvst->GetYaxis()->SetTitle("X position");
        xvst->GetXaxis()->SetTitle("drift time");
        xvst->GetYaxis()->SetRangeUser(-18, 18);
        //xvst->Fit("pol1");
        xvst->Draw("A*E");

        canvas->cd(2);
        TGraph* yvst = new TGraph(tdrift.size(), &tdrift[0], &pulseY[0]);
        yvst->SetName("y vs tdrift");
        yvst->SetTitle("Y position vs drift time");

        yvst->GetYaxis()->SetTitle("Y position");
        yvst->GetXaxis()->SetTitle("drift time");
        yvst->GetYaxis()->SetRangeUser(-18, 18);
        //yvst->Fit("pol1");
        yvst->Draw("A*E");
        
        canvas->cd(3);
        // TGraph* fullGraph = new TGraph(tdrift.size(), &pulseX[0], &pulseY[0]);
        // TH1F* hist = new TH1F("testHist", "", 1000, -18, 18);
        // fullGraph->SetHistogram(hist);
        // fullGraph->SetName("y vs x");
        // fullGraph->Draw("A*");
        // fullGraph->GetYaxis()->SetRangeUser(-18, 18);
        // fullGraph->SetTitle("Top down view");
        // fullGraph->GetYaxis()->SetTitle("Y position");
        // fullGraph->GetXaxis()->SetTitle("X position");
        // //fullGraph->Fit("pol1");
        // fullGraph->Draw("A*");
        brightnessPerPulse->Draw();

    }
};


void SingleXYPlot() {
    // Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");
    //Engine::init("/mnt/c/Users/Thomas/Desktop/slad_sixty.root");
    // Engine::init("~/SLAD/slad_muon_all.root");
    Engine::init("~/SLAD/slad_muon_tight.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    // e->addModule(new XYPlot());
    e->addModule(new S2FitV3("", 0, true));

    e->runSingleEvent(6);
}
