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

    TH2* residvsnpe;
    TH2* residvss1;
    TH2* residvstdrift;
    TH1* residovernpe;    
    TH1* npeoverresid;

    void init() {
        output.open("output/s2FitInfo.txt");
        xresiduals = new TH1F("xresid", "xresiduals", 25, -18, 18);
        yresiduals = new TH1F("yresid", "yresiduals", 25, -18, 18);
        yvsx = new TH2F("yvsx", "yvsx", 50, -18, 18, 50, -18, 18);

        residvsnpe = new TH2F("residvsnpe", "Residual vs NPE", 150, 0, 100000, 15, 0, 18);
        residvss1 = new TH2F("residvss1", "Residual vs s1", 150, 0, 100000, 15, 0, 18);
        residvstdrift = new TH2F("residvstdrift", "Residual vs tdrift", 18, 0, 450, 15, 0, 18);
        residovernpe = new TH1F("residovernpe", "Residual/NPE", 500, 0, .01);
        npeoverresid = new TH1F("npeoverresid", "NPE/Residual", 500, 0, 10000);

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
            for(int testPulse = startPulse; testPulse < e.npulses; testPulse++) {
                vector<double> x, y, tdrift; // vectors to hold fit data
                vector<double> xNoDrop, yNoDrop, tdriftNoDrop; // vectors to hold all pulses WITHOUT dropping this one

                bool droppedGoodXY = (e.pulse_x_masa[testPulse] > -20)
                            && (e.pulse_x_masa[testPulse] < 20)
                            && (e.pulse_y_masa[testPulse] > -20)
                            && (e.pulse_y_masa[testPulse] < 20);
                
                if(droppedGoodXY) { 
                    for(int currentPulse = startPulse; currentPulse < e.npulses; currentPulse++) {
                        bool currentGoodXY = (e.pulse_x_masa[currentPulse] > -20)
                                    && (e.pulse_x_masa[currentPulse] < 20)
                                    && (e.pulse_y_masa[currentPulse] > -20)
                                    && (e.pulse_y_masa[currentPulse] < 20);

                        if((currentPulse != testPulse) && currentGoodXY) {
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
                        TF1* fitx = runFit(xvst);
                        if(draw) {
                            canvas->cd(1);
                            if(testPulse == startPulse) {
                                fitx->Draw();
                                fitx->GetYaxis()->SetRangeUser(-18, 18);
                            } else { 
                                fitx->Draw("same");
                            }
                        }

                        output << e.pulse_start_time[testPulse] - e.pulse_start_time[0] << " "
                                << e.pulse_x_masa[testPulse] << " "
                                << e.pulse_y_masa[testPulse] << " "
                                << fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0]) << " ";
                        xresiduals->Fill((fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - e.pulse_x_masa[testPulse]);
                        //double xresidValue = (fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - e.pulse_x_masa[testPulse];
                        // start fitting y vs tdrift
                        TGraph* yvst = new TGraph(tdrift.size(), &tdrift[0], &y[0]);
                        TF1* fity = runFit(yvst);
                        if(draw) {
                            canvas->cd(2);
                            if(testPulse == startPulse) {
                                fity->Draw();
                                fity->GetYaxis()->SetRangeUser(-18, 18);
                            } else { 
                                fity->Draw("same");
                            }             
                        }

                        double xresidValue = (fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - e.pulse_x_masa[testPulse];
                        double yresidValue = (fity->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - e.pulse_y_masa[testPulse];

                        double rResidValue = sqrt(xresidValue * xresidValue + yresidValue * yresidValue);

                        xresiduals->Fill(xresidValue);
                        yresiduals->Fill(yresidValue);
                        yvsx->Fill(xresidValue, yresidValue);
                        residvsnpe->Fill(e.pulse_total_npe[testPulse], rResidValue);
                        residvss1->Fill(e.pulse_total_npe[0], rResidValue);
                        residvstdrift->Fill(e.pulse_start_time[testPulse] - e.pulse_start_time[0], rResidValue);
                        residovernpe->Fill(rResidValue/e.pulse_total_npe[testPulse]);
                        if(rResidValue > 0) npeoverresid->Fill(e.pulse_total_npe[testPulse]/rResidValue);
                    }
                }
            }
        }
    }

    void cleanup() {
        output.close();
    }
};