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

class S2FitV4 : public Module {
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

    TH2* residvsnpe;
    TH2* xvsnpe;
    TH2* residvsradius;
    TH2* residvss1;
    TH2* residvstdrift;

    TH2* topLocation;
    TH2* bottomLocation;
    TH2* yvsxslopes;

    TCanvas* canvas;
    string suffix;

    S2FitV4(string histogramSuffix, int xy, bool doDraw = false) {
        draw = doDraw;
        suffix = histogramSuffix;
        xyAlgorithm = xy;
    }
    void init() {
        output.open("output/fittableMuonEvents.txt");
        xresiduals = new TH1F(("xresid" + suffix).c_str(), "xresiduals", 400, -20, 20);
        yresiduals = new TH1F(("yresid" + suffix).c_str(), "yresiduals", 400, -20, 20);
        rresiduals = new TH1F(("rresid" + suffix).c_str(), "yresiduals", 400, -20, 20);
        yvsx = new TH2F(("yvsx" + suffix).c_str(), "yvsx", 100, -20, 20, 100, -20, 20);

        residvsnpe = new TH2F(("residvsnpe" + suffix).c_str(), "Residual vs NPE", 100, 0, 1000, 50, 0, 40);
        xvsnpe = new TH2F(("xvsnpe" + suffix).c_str(), "X Residual vs NPE", 100, 0, 1000, 100, -40, 40);
        residvsradius = new TH2F(("residvsradius" + suffix).c_str(), "Residual vs Radius", 40, 0, 40, 50, 0, 40);
        residvss1 = new TH2F(("residvss1" + suffix).c_str(), "Residual vs s1", 50, 0, 100000, 50, -40, 40);
        residvstdrift = new TH2F(("residvstdrift" + suffix).c_str(), "Residual vs tdrift", 18, 0, 450, 50, 0, 40);

        topLocation = new TH2F(("topLocation" + suffix).c_str(), "Top location", 50, -40, 40, 50, -40, 40);
        bottomLocation = new TH2F(("bottomLocation" + suffix).c_str(), "bottom location", 50, -40, 40, 50, -40, 40);
        yvsxslopes = new TH2F(("yvsxslopes" + suffix).c_str(), "slopes", 100, -.5, .5, 100, -.5, .5);

        if(draw) {
            canvas = new TCanvas("can");
            canvas->Divide(3, 1);
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
                if(goodPulse && e.pulse_saturated[i] == true) {
                    if(e.pulse_saturated[i] == true) { // e.pulse_total_npe[i] > 200
                        pulsesToUseForFitting.push_back(i);
                        if(draw) cout << e.pulse_total_npe[i] << endl;
                    }
                }
            }

            if((pulsesToUseForFitting.size() >= 5) && (e.run_id > 10000)) {
                output << e.run_id << " " << e.event_id << endl;
            }

            for(int testPulse = startPulse; testPulse < e.npulses; testPulse++) {
                vector<double> testX, testY, testTDrift;

                if(draw && e.pulse_saturated[testPulse] > 0) {
                    cout << "Pulse " << testPulse << " at " << e.pulse_start_time[testPulse] << " is saturated" << endl;
                }

                if(false) {
                    cout << "Peak time " << e.pulse_peak_time[testPulse];
                    cout << " Peak amp " << e.pulse_peak_amp[testPulse];

                    cout << endl;
                }

                // check to see if the test pulse reconstructed well
                bool goodTestXY = (data_x[testPulse] > -20)
                            && (data_x[testPulse] < 20)
                            && (data_y[testPulse] > -20)
                            && (data_y[testPulse] < 20);
                
                if(goodTestXY && e.pulse_saturated[testPulse] == true) {
                    
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
                        double xResidValue = (fitx->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - data_x[testPulse];
                        double yResidValue = (fity->Eval(e.pulse_start_time[testPulse] - e.pulse_start_time[0])) - data_y[testPulse];

                        double rResidValue = sqrt(xResidValue * xResidValue + yResidValue * yResidValue);
                        // double rResidValue = xResidValue;

                        float npe = e.pulse_total_npe[testPulse];
                        xresiduals->Fill(xResidValue);
                        yresiduals->Fill(yResidValue);
                        rresiduals->Fill(rResidValue);
                        yvsx->Fill(xResidValue, yResidValue);

                        residvsnpe->Fill(e.pulse_total_npe[testPulse], rResidValue);
                        xvsnpe->Fill(e.pulse_total_npe[testPulse], xResidValue);
                        if(npe > 20) residvsradius->Fill(data_r[testPulse], rResidValue, 1/rResidValue);
                        residvss1->Fill(e.pulse_total_npe[0], rResidValue);
                        residvstdrift->Fill(e.pulse_start_time[testPulse] - e.pulse_start_time[0], rResidValue);

                        // residvsnpe->Fill(e.pulse_total_npe[testPulse], rResidValue, 1/rResidValue);
                        // if(npe > 20) residvsradius->Fill(data_r[testPulse], rResidValue, 1/rResidValue);
                        // residvss1->Fill(e.pulse_total_npe[0], rResidValue, 1/rResidValue);
                        // residvstdrift->Fill(e.pulse_start_time[testPulse] - e.pulse_start_time[0], rResidValue, 1/rResidValue);

                        topLocation->Fill(fitx->Eval(0), fity->Eval(0));
                        bottomLocation->Fill(fitx->Eval(350), fity->Eval(350));
                        yvsxslopes->Fill(fitx->GetParameter(0), fity->GetParameter(0));

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

            if(draw) {
                vector<float> pulseX, pulseY, pulseT;

                for(int i = 0; i < e.npulses; i++) {
                    pulseX.push_back(data_x[i]);
                    pulseY.push_back(data_y[i]);
                    pulseT.push_back(e.pulse_start_time[i]);
                }

                canvas->cd(1);
                TGraph* xvst = new TGraph(pulseT.size(), &pulseT[0], &pulseX[0]);
                xvst->Draw("P*");

                canvas->cd(2);
                TGraph* yvst = new TGraph(pulseT.size(), &pulseT[0], &pulseY[0]);
                yvst->Draw("P*");

                vector<float> pulse_npe_graph;
                for(int i = 0; i < e.npulses; i++) {
                    pulse_npe_graph.push_back(e.pulse_total_npe[i]);
                }

                canvas->cd(3);
                TGraph* npevst = new TGraph(pulseT.size(), &pulseT[0], &pulse_npe_graph[0]);
                npevst->Draw("AC*");
            }
        }
    }

    void cleanup() {
        output.close();
    }
};