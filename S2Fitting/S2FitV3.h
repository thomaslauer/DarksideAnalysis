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

    int xyAlgorithm;

    TH1* xresiduals;
    TH1* yresiduals;
    TH1* rresiduals;
    TH2* yvsx;
    TH2* yvsxcenter;

    TH2* residvsnpe;
    TH2* residvsradius;
    TH2* residvss1;
    TH2* residvstdrift;
    TH1* residovernpe;    
    TH1* npeoverresid;    
    TH1* npetotal;    

    TH1* npulsesFit;

    TCanvas* canvas;
    string suffix;

    S2FitV3(string histogramSuffix, int xy) {
        suffix = histogramSuffix;
        xyAlgorithm = xy;
    }
    void init() {
        output.open("output/s2FitInfo.txt");
        xresiduals = new TH1F(("xresid" + suffix).c_str(), "xresiduals", 200, -40, 40);
        yresiduals = new TH1F(("yresid" + suffix).c_str(), "yresiduals", 200, -40, 40);
        rresiduals = new TH1F(("rresid" + suffix).c_str(), "yresiduals", 200, -40, 40);
        yvsx = new TH2F(("yvsx" + suffix).c_str(), "yvsx", 100, -20, 20, 100, -20, 20);
        yvsxcenter = new TH2F(("yvsx_center_" + suffix).c_str(), "y vs x", 50, -1, 1, 50, -1, 1);

        residvsnpe = new TH2F(("residvsnpe" + suffix).c_str(), "Residual vs NPE", 100, 0, 1000, 50, -40, 40);
        residvsradius = new TH2F(("residvsradius" + suffix).c_str(), "Residual vs Radius", 40, 0, 40, 50, -40, 40);
        residvss1 = new TH2F(("residvss1" + suffix).c_str(), "Residual vs s1", 50, 0, 100000, 50, -40, 40);
        residvstdrift = new TH2F(("residvstdrift" + suffix).c_str(), "Residual vs tdrift", 20, 0, 450, 50, -40, 40);
        residovernpe = new TH1F(("residovernpe" + suffix).c_str(), "Residual/NPE", 500, 0, .01);
        npeoverresid = new TH1F(("npeoverresid" + suffix).c_str(), "NPE/Residual", 500, 0, 10000);
        npetotal = new TH1F(("npetotal" + suffix).c_str(), "NPE", 500, 0, 1000);

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

        vector<float> data_x;
        vector<float> data_y;
        vector<float> data_r;

        if(xyAlgorithm == 0) {
            data_x.assign(e.pulse_x_masa, e.pulse_x_masa + 100);
            data_y.assign(e.pulse_y_masa, e.pulse_y_masa + 100);
            data_r.assign(e.pulse_r_masa, e.pulse_r_masa + 100);
        }
        if(xyAlgorithm == 1) {
            data_x.assign(e.pulse_x_jason, e.pulse_x_jason + 100);
            data_y.assign(e.pulse_y_jason, e.pulse_y_jason + 100);
            data_r.assign(e.pulse_r_jason, e.pulse_r_jason + 100);
        }
        if(xyAlgorithm == 2) {
            data_x.assign(e.pulse_x_andrew, e.pulse_x_andrew + 100);
            data_y.assign(e.pulse_y_andrew, e.pulse_y_andrew + 100);
            data_r.assign(e.pulse_r_andrew, e.pulse_r_andrew + 100);
        }

        bool basicCuts = (e.nchannels == 38)
                && (e.baseline_not_found == false)
                && ((e.live_time + e.inhibit_time) >= 1.35e-3)
                && (e.live_time < 1.);

        int startPulse = 1;
        if(basicCuts) {
            vector<int> pulsesToUseForFitting; // a vector of pulse indices we can use for making fits later

            // Selects pulses that we can use for more in depth analysis
            for(int i = startPulse; i < e.npulses; i++) {
                bool goodPulse = (data_x[i] > -20)
                            && (data_x[i] < 20)
                            && (data_y[i] > -20)
                            && (data_y[i] < 20);
                if(goodPulse) {
                    if(e.pulse_total_npe[i] > 50) {
                        pulsesToUseForFitting.push_back(i);
                    }
                }
            }

            vector<double> testX, testY, testTDrift;

            for(int testPulse = startPulse; testPulse < e.npulses; testPulse++) {

                // check to see if the test pulse reconstructed well
                bool goodTestXY = (data_x[testPulse] > -20)
                            && (data_x[testPulse] < 20)
                            && (data_y[testPulse] > -20)
                            && (data_y[testPulse] < 20);
                
                if(goodTestXY) {
                    
                    for(int i = 0; i < pulsesToUseForFitting.size(); i++) {
                        if(pulsesToUseForFitting[i] != testPulse) {

                            //cout << i << " " << data_x[pulsesToUseForFitting[i]] << " " << data_y[pulsesToUseForFitting[i]] << endl;

                            testX.push_back(data_x[pulsesToUseForFitting[i]]);
                            testY.push_back(data_y[pulsesToUseForFitting[i]]);
                            testTDrift.push_back(e.pulse_start_time[pulsesToUseForFitting[i]] - e.pulse_start_time[0]);
                        }
                    }

                    if(testTDrift.size() >= 5) {
                        TF1* fitx = runFit(new TGraph(testTDrift.size(), &testTDrift[0], &testX[0]));
                        TF1* fity = runFit(new TGraph(testTDrift.size(), &testTDrift[0], &testY[0]));
                        double xresidValue = (fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - data_x[testPulse];
                        double yresidValue = (fity->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - data_y[testPulse];

                        double rResidValue = sqrt(xresidValue * xresidValue + yresidValue * yresidValue);
                        // double rResidValue = xresidValue;

                        float npe = e.pulse_total_npe[testPulse];
                        xresiduals->Fill(xresidValue);
                        yresiduals->Fill(yresidValue);
                        rresiduals->Fill(rResidValue);
                        yvsx->Fill(xresidValue, yresidValue);
                        yvsxcenter->Fill(xresidValue, yresidValue);



                        residvsnpe->Fill(e.pulse_total_npe[testPulse], rResidValue, 1/rResidValue);
                        if(npe > 20) residvsradius->Fill(data_r[testPulse], rResidValue, 1/rResidValue);
                        residvss1->Fill(e.pulse_total_npe[0], rResidValue, 1/rResidValue);
                        residvstdrift->Fill(e.pulse_start_time[testPulse] - e.pulse_start_time[0], rResidValue, 1/rResidValue);
                        
                        
                        residovernpe->Fill(rResidValue/e.pulse_total_npe[testPulse]);
                        if(rResidValue > 0.1) npeoverresid->Fill(e.pulse_total_npe[testPulse]);
                        npetotal->Fill(e.pulse_total_npe[testPulse]);

                        
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