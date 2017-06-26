#include "darkside/Module.h"
#include "darkside/Event.h"

#include "TGraph.h"
#include "TF1.h"

#include <iostream>
#include <vector>

using namespace std;

class S2Fitter : public Module {

    void init() {
    }
    
    TF1* runFit(TGraph* g) {
        TF1* fit = new TF1("linearFit", "[0]*x+[1]", 0, 450);
        fit->SetParameter(0, 0);
        fit->SetParameter(0, 0);
        g->Fit(fit);

        return fit;
    }

    void processEvent(Event& e) {
        int startPulse = 2;        

        if(e.npulses > 4) {
            for(int droppedPulse = startPulse; droppedPulse < e.npulses; droppedPulse++) {
                vector<double> x, y, tdrift;
                for(int currentPulse = startPulse; currentPulse < e.npulses; currentPulse++) {
                    // make sure all pulses reconstruct correctly
                    bool goodXY = (e.pulse_x_masa[currentPulse] > -20)
                                && (e.pulse_x_masa[currentPulse] < 20)
                                && (e.pulse_y_masa[currentPulse] > -20)
                                && (e.pulse_y_masa[currentPulse] < 20);

                    if((currentPulse != droppedPulse) && goodXY) {
                        x.push_back(e.pulse_x_masa[currentPulse]);
                        y.push_back(e.pulse_y_masa[currentPulse]);
                        tdrift.push_back(e.pulse_start_time[currentPulse] - e.pulse_start_time[0]);
                    }
                }

                TGraph* xvst = new TGraph(tdrift.size(), &tdrift[0], &x[0]);
                TF1* f1 = runFit(xvst);
                (droppedPulse == startPulse) ? f1->Draw() : f1->Draw("same");
                cout << "slope: " << f1->GetParameter(0) << " intercept: " << f1->GetParameter(1) << endl;


                TGraph* yvst = new TGraph(tdrift.size(), &tdrift[0], &y[0]);
                TF1* f2 = runFit(yvst);
                cout << "slope: " << f2->GetParameter(0) << " intercept: " << f2->GetParameter(1) << endl;
            }
        }


    }

    void cleanup() {
    }
};