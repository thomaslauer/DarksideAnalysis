#include "darkside/Module.h"
#include "darkside/Event.h"
#include "darkside/Engine.h"

#include "TGraph.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TAxis.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <algorithm>

using namespace std;

class S2FitV3 : public Module {

    ofstream output;

    bool draw = false;
    bool firstDraw = true;

    double residualThreshold = 6;

    TH1* xresiduals;
    TH1* yresiduals;
    TH2* yvsx;

    TH2* residvsnpe;
    TH2* residvss1;
    TH2* residvstdrift;
    TH1* residovernpe;    
    TH1* npeoverresid;    
    TH1* npe;    

    TCanvas* canvas;

    void init() {
        output.open("output/s2FitInfo.txt");
        xresiduals = new TH1F("xresid", "xresiduals", 50, -18, 18);
        yresiduals = new TH1F("yresid", "yresiduals", 50, -18, 18);
        yvsx = new TH2F("yvsx", "yvsx", 50, -18, 18, 50, -18, 18);

        residvsnpe = new TH2F("residvsnpe", "Residual vs NPE", 50, 0, 200, 15, 0, 18);
        residvss1 = new TH2F("residvss1", "Residual vs s1", 50, 0, 100000, 15, 0, 18);
        residvstdrift = new TH2F("residvstdrift", "Residual vs tdrift", 18, 0, 450, 15, 0, 18);
        residovernpe = new TH1F("residovernpe", "Residual/NPE", 500, 0, .01);
        npeoverresid = new TH1F("npeoverresid", "NPE/Residual", 500, 0, 10000);
        npe = new TH1F("npe", "NPE/Residual", 500, 0, 10000);

        if(draw) {
            canvas = new TCanvas("can");
            canvas->Divide(2, 1);
        }
    }
    
    TF1* runFit(TGraph* g) {
        TF1* fit = new TF1("linearFit", "[0]*x+[1]", 0, 450);
        fit->SetParameter(0, 0);
        fit->SetParameter(0, 0);
        g->Fit(fit, "q");

        return fit;
    }

    void processEvent(Event& e) {
        int startPulse = 1;
        
        

        if(e.npulses > 4) {

            

            vector<int> pulsesToUseForFitting; // a vector of pulses we can use for making fits later

            // Selects pulses that we can use for more in depth analysis
            for(int i = startPulse; i < e.npulses; i++) {
                bool goodPulse = (e.pulse_x_masa[i] > -20)
                            && (e.pulse_x_masa[i] < 20)
                            && (e.pulse_y_masa[i] > -20)
                            && (e.pulse_y_masa[i] < 20);
                if(goodPulse) {
                    if(e.pulse_total_npe[i] > 50) {
                        pulsesToUseForFitting.push_back(i);
                    }
                }
            }

            vector<double> testX, testY, testTDrift;

            for(int testPulse = startPulse; testPulse < e.npulses; testPulse++) {

                // check to see if the test pulse reconstructed well
                bool goodTestXY = (e.pulse_x_masa[testPulse] > -20)
                            && (e.pulse_x_masa[testPulse] < 20)
                            && (e.pulse_y_masa[testPulse] > -20)
                            && (e.pulse_y_masa[testPulse] < 20);
                
                if(goodTestXY) {
                    
                    for(int i = 0; i < pulsesToUseForFitting.size(); i++) {
                        if(pulsesToUseForFitting[i] != testPulse) {

                            //cout << i << " " << e.pulse_x_masa[pulsesToUseForFitting[i]] << " " << e.pulse_y_masa[pulsesToUseForFitting[i]] << endl;

                            testX.push_back(e.pulse_x_masa[pulsesToUseForFitting[i]]);
                            testY.push_back(e.pulse_y_masa[pulsesToUseForFitting[i]]);
                            testTDrift.push_back(e.pulse_start_time[pulsesToUseForFitting[i]] - e.pulse_start_time[0]);
                        }
                    }

                    TF1* fitx = runFit(new TGraph(testTDrift.size(), &testTDrift[0], &testX[0]));
                    TF1* fity = runFit(new TGraph(testTDrift.size(), &testTDrift[0], &testY[0]));

                    if(testTDrift.size() > 3) {
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
                        if(rResidValue > 5) npeoverresid->Fill(e.pulse_total_npe[testPulse]);
                        npe->Fill(e.pulse_total_npe[testPulse]);
                    }

                    if(draw) {
                        if(firstDraw) {
                            canvas->cd(1);
                            fitx->GetYaxis()->SetRangeUser(-18, 18);
                            fitx->Draw();

                            canvas->cd(2);
                            fity->GetYaxis()->SetRangeUser(-18, 18);
                            fity->Draw();
                            firstDraw = false;
                        } else {
                            canvas->cd(1);
                            fitx->GetYaxis()->SetRangeUser(-18, 18);
                            fitx->Draw("same");

                            canvas->cd(2);
                            fity->GetYaxis()->SetRangeUser(-18, 18);
                            fity->Draw("same");
                        }
                    }
                }
            }
        }
    }

    void cleanup() {
        output.close();
    }
};