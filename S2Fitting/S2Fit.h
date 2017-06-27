#include "../darkside/Module.h"
#include "../darkside/Event.h"
#include "../darkside/Engine.h"

#include "TGraph.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TAxis.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class S2Fit : public Module {

    TH2* residualX_vs_tDrift;
    ofstream output;

    bool draw = false;

    TH1* xresiduals;
    TH1* yresiduals;
    TH2* yvsx;

    void init() {
        output.open("output/s2FitInfo.txt");
        xresiduals = new TH1F("xresid", "x residuals", 25, -18, 18);
        yresiduals = new TH1F("yresid", "y residuals", 25, -18, 18);
        yvsx = new TH2F("yvsx", "yvsx", 25, -18, 18, 25, -18, 18);

    }
    
    TF1* runFit(TGraph* g) {
        TF1* fit = new TF1("linearFit", "[0]*x+[1]", 0, 450);
        fit->SetParameter(0, 0);
        fit->SetParameter(0, 0);
        g->Fit(fit, "q");

        return fit;
    }

    void processEvent(Event& e) {
        int startPulse = 2;   
        TCanvas* canvas;
        if(draw) {
            canvas = new TCanvas("can");
            canvas->Divide(2, 1);
        }
        if(e.npulses > 4) {
            for(int droppedPulse = startPulse; droppedPulse < e.npulses; droppedPulse++) {
                vector<double> x, y, tdrift; // vectors to hold fit data
                vector<double> xNoDrop, yNoDrop, tdriftNoDrop; // vectors to hold all pulses WITHOUT dropping this one

                bool droppedGoodXY = (e.pulse_x_masa[droppedPulse] > -20)
                            && (e.pulse_x_masa[droppedPulse] < 20)
                            && (e.pulse_y_masa[droppedPulse] > -20)
                            && (e.pulse_y_masa[droppedPulse] < 20);
                
                if(droppedGoodXY) { 
                    for(int currentPulse = startPulse; currentPulse < e.npulses; currentPulse++) {
                        bool currentGoodXY = (e.pulse_x_masa[currentPulse] > -20)
                                    && (e.pulse_x_masa[currentPulse] < 20)
                                    && (e.pulse_y_masa[currentPulse] > -20)
                                    && (e.pulse_y_masa[currentPulse] < 20);

                        if((currentPulse != droppedPulse) && currentGoodXY) {
                            x.push_back(e.pulse_x_masa[currentPulse]);
                            y.push_back(e.pulse_y_masa[currentPulse]);
                            tdrift.push_back(e.pulse_start_time[currentPulse] - e.pulse_start_time[0]);

                            xNoDrop.push_back(e.pulse_x_masa[currentPulse]);
                            yNoDrop.push_back(e.pulse_y_masa[currentPulse]);
                            tdriftNoDrop.push_back(e.pulse_start_time[currentPulse] - e.pulse_start_time[0]);
                        }
                    }
                    
                    if(tdrift.size() > 3) { // make sure enough pulses reconstructed to make a fit

                        TGraph* xvst = new TGraph(tdrift.size(), &tdrift[0], &x[0]);
                        TF1* f1 = runFit(xvst);
                        if(draw) {
                            canvas->cd(1);
                            if(droppedPulse == startPulse) {
                                f1->Draw();
                                f1->GetYaxis()->SetRangeUser(-18, 18);
                            } else { 
                                f1->Draw("same");
                            }
                        }

                        output << e.pulse_start_time[droppedPulse] - e.pulse_start_time[0] << " "
                                << e.pulse_x_masa[droppedPulse] << " "
                                << e.pulse_y_masa[droppedPulse] << " "
                                << f1->Eval(e.pulse_start_time[droppedPulse] - e.pulse_start_time[0]) << " ";
                        xresiduals->Fill((f1->Eval(e.pulse_start_time[droppedPulse] - e.pulse_start_time[0])) - e.pulse_x_masa[droppedPulse]);
                        double xresidValue = (f1->Eval(e.pulse_start_time[droppedPulse] - e.pulse_start_time[0])) - e.pulse_x_masa[droppedPulse];
                        // start fitting y vs tdrift
                        TGraph* yvst = new TGraph(tdrift.size(), &tdrift[0], &y[0]);
                        TF1* f2 = runFit(yvst);
                        if(draw) {
                            canvas->cd(2);
                            if(droppedPulse == startPulse) {
                                f2->Draw();
                                f2->GetYaxis()->SetRangeUser(-18, 18);
                            } else { 
                                f2->Draw("same");
                            }             
                        }

                        double yresidValue = (f2->Eval(e.pulse_start_time[droppedPulse] - e.pulse_start_time[0])) - e.pulse_y_masa[droppedPulse];

                        yvsx->Fill(xresidValue, yresidValue);

                        output << f2->Eval(e.pulse_start_time[droppedPulse] - e.pulse_start_time[0]) << endl;
                        yresiduals->Fill((f2->Eval(e.pulse_start_time[droppedPulse] - e.pulse_start_time[0])) - e.pulse_y_masa[droppedPulse]);
                    }
                }
            }
        }
    }

    void cleanup() {
        output.close();
    }
};