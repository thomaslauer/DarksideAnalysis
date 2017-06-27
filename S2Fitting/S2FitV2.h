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

class S2FitV2 : public Module {

    ofstream output;

    bool draw = false;
    bool firstDraw = true;

    double residualThreshold = 6;

    TH1* xresiduals;
    TH1* yresiduals;
    TH2* yvsx;

    TCanvas* canvas;

    void init() {
        output.open("output/s2FitInfo.txt");
        xresiduals = new TH1F("xresid", "xresiduals", 25, -18, 18);
        yresiduals = new TH1F("yresid", "yresiduals", 25, -18, 18);
        yvsx = new TH2F("yvsx", "yvsx", 25, -18, 18, 25, -18, 18);

        if(draw) {
            canvas = new TCanvas("can");
            canvas->Divide(2, 1);
        }
    }
    
    TF1* runFit(TGraph* g) {
        TF1* fit = new TF1("linearFit", "[0]*x+[1]", 0, 450);
        fit->SetParameter(0, 0);
        fit->SetParameter(0, 0);
        g->Fit(fit);

        return fit;
    }

    void processEvent(Event& e) {
        int startPulse = 1;
        
        

        if(e.npulses > 4) {

            // vectors for the full event
            vector<double> fullX, fullY, fullTDrift;

            // select pulses of whole event that reconstruct well
            for(int i = startPulse; i < e.npulses; i++) {
                bool goodPulse = (e.pulse_x_masa[i] > -20)
                            && (e.pulse_x_masa[i] < 20)
                            && (e.pulse_y_masa[i] > -20)
                            && (e.pulse_y_masa[i] < 20);
                if(goodPulse) {
                    fullX.push_back(e.pulse_x_masa[i]);
                    fullY.push_back(e.pulse_y_masa[i]);
                    fullTDrift.push_back(e.pulse_start_time[i] - e.pulse_start_time[0]);
                }
            }

            // Runs fits on whole dataset. These are used to see which points should be ommited from later tests 
            TF1* xFullFit = runFit(new TGraph(fullTDrift.size(), &fullTDrift[0], &fullX[0]));
            TF1* yFullFit = runFit(new TGraph(fullTDrift.size(), &fullTDrift[0], &fullY[0]));

            vector<int> pulsesToUseForFitting; // a vector of pulses we can use for making fits later

            // Selects pulses that we can use for more in depth analysis
            for(int i = startPulse; i < e.npulses; i++) {
                bool goodPulse = (e.pulse_x_masa[i] > -20)
                            && (e.pulse_x_masa[i] < 20)
                            && (e.pulse_y_masa[i] > -20)
                            && (e.pulse_y_masa[i] < 20);
                if(goodPulse) {
                    double xresid = abs(e.pulse_x_masa[i] - xFullFit->Eval(e.pulse_start_time[i] - e.pulse_start_time[0]));
                    double yresid = abs(e.pulse_y_masa[i] - yFullFit->Eval(e.pulse_start_time[i] - e.pulse_start_time[0]));

                    // if it's relatively close
                    if(xresid < residualThreshold && yresid < residualThreshold) {
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

                            cout << i << " " << e.pulse_x_masa[pulsesToUseForFitting[i]] << " " << e.pulse_y_masa[pulsesToUseForFitting[i]] << endl;

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

                        xresiduals->Fill(xresidValue);
                        yresiduals->Fill(yresidValue);
                        yvsx->Fill(xresidValue, yresidValue);
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