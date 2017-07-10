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
#include <string>

using namespace std;

class S2FitV3 : public Module {
public:
    ofstream output;

    bool draw = false;
    bool firstDraw = true;

    double residualThreshold = 6;

    TH1* xresiduals;
    TH1* yresiduals;
    TH1* rresiduals;
    TH2* yvsx;

    TH2* residvsnpe;
    TH2* residvsradius;
    TH2* residvss1;
    TH2* residvstdrift;
    TH1* residovernpe;    
    TH1* npeoverresid;    
    TH1* npe;    

    TH1* npulsesFit;

    TCanvas* canvas;
    string suffix;

    S2FitV3(string histogramSuffix) {
        suffix = histogramSuffix;
    }
    void init() {
        output.open("output/s2FitInfo.txt");
        xresiduals = new TH1F(("xresid" + suffix).c_str(), "xresiduals", 100, -20, 20);
        yresiduals = new TH1F(("yresid" + suffix).c_str(), "yresiduals", 100, -20, 20);
        rresiduals = new TH1F(("rresid" + suffix).c_str(), "yresiduals", 100, -20, 20);
        yvsx = new TH2F(("yvsx" + suffix).c_str(), "yvsx", 200, -20, 20, 200, -20, 20);

        residvsnpe = new TH2F(("residvsnpe" + suffix).c_str(), "Residual vs NPE", 100, 0, 200, 50, -20, 20);
        residvsradius = new TH2F(("residvsradius" + suffix).c_str(), "Residual vs Radius", 15, 0, 20, 50, -20, 20);
        residvss1 = new TH2F(("residvss1" + suffix).c_str(), "Residual vs s1", 50, 0, 100000, 50, -20, 20);
        residvstdrift = new TH2F(("residvstdrift" + suffix).c_str(), "Residual vs tdrift", 20, 0, 450, 50, -20, 20);
        residovernpe = new TH1F(("residovernpe" + suffix).c_str(), "Residual/NPE", 500, 0, .01);
        npeoverresid = new TH1F(("npeoverresid" + suffix).c_str(), "NPE/Residual", 500, 0, 10000);
        npe = new TH1F(("npe" + suffix).c_str(), "NPE/Residual", 500, 0, 10000);

        npulsesFit = new TH1F("npulsesfit", "npulses that fit", 20, 0, 20);

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

                    
                    npulsesFit->Fill(testTDrift.size());
                    if(testTDrift.size() >= 5) {
                        TF1* fitx = runFit(new TGraph(testTDrift.size(), &testTDrift[0], &testX[0]));
                        TF1* fity = runFit(new TGraph(testTDrift.size(), &testTDrift[0], &testY[0]));
                        double xresidValue = (fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - e.pulse_x_masa[testPulse];
                        double yresidValue = (fity->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - e.pulse_y_masa[testPulse];

                        double rResidValue = sqrt(xresidValue * xresidValue + yresidValue * yresidValue);
                        // double rResidValue = xresidValue;
                        xresiduals->Fill(xresidValue);
                        yresiduals->Fill(yresidValue);
                        rresiduals->Fill(rResidValue);
                        yvsx->Fill(xresidValue, yresidValue);
                        residvsnpe->Fill(e.pulse_total_npe[testPulse], rResidValue);
                        residvsradius->Fill(e.pulse_r_masa[testPulse], rResidValue);
                        residvss1->Fill(e.pulse_total_npe[0], rResidValue);
                        residvstdrift->Fill(e.pulse_start_time[testPulse] - e.pulse_start_time[0], rResidValue);
                        residovernpe->Fill(rResidValue/e.pulse_total_npe[testPulse]);
                        if(rResidValue > 5) npeoverresid->Fill(e.pulse_total_npe[testPulse]);
                        npe->Fill(e.pulse_total_npe[testPulse]);

                        
                        if(draw) {
                            if(firstDraw) {
                                canvas->cd(1);
                                fitx->GetYaxis()->SetRangeUser(-20, 20);
                                fitx->Draw();

                                canvas->cd(2);
                                fity->GetYaxis()->SetRangeUser(-20, 20);
                                fity->Draw();
                                firstDraw = false;
                            } else {
                                canvas->cd(1);
                                fitx->GetYaxis()->SetRangeUser(-20, 20);
                                fitx->Draw("same");

                                canvas->cd(2);
                                fity->GetYaxis()->SetRangeUser(-20, 20);
                                fity->Draw("same");
                            }
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