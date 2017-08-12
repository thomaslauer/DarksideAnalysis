#include "darkside/Module.h"
#include "darkside/Event.h"

#include "TH1.h"
#include <iostream>

using namespace std;

class CalculateEnergy : public Module {
public:
    TH1* henergy;
    TH2* henergyvslsv;
    TH1* f90;

    bool mdraw;

    CalculateEnergy(bool draw = false) {
        mdraw = draw;
    }

    void init() {
        henergy = new TH1D("henergy", "energy", 10000, 0, 100000);
        henergyvslsv = new TH2D("henergyvslsv", "energyvslsv", 1000, 0, 100000, 1000, 0, 200000);
        f90 = new TH1D("f90", "f90", 100, 0, 1);
    }
    
    void processEvent(Event& e) {

        bool basicCuts = (e.nchannels == 38)
                && (e.baseline_not_found == false)
                && ((e.live_time + e.inhibit_time) >= 1.35e-3)
                && (e.live_time < 1.);
        
        bool runNumberCuts = (e.run_id > 14463 && e.run_id < 14731)
                || (e.run_id > 15442 && e.run_id < 15620)
                || (e.run_id > 15954 && e.run_id < 16066);
        
        if(mdraw) cout << e.correctedS1 << " " << e.correctedS2 << endl;
        
        for(int i = 0; i < e.npulses; i++) {
            f90->Fill(e.pulse_f90[i]);
        }

        if(basicCuts && !runNumberCuts && e.npulses > 3 && e.total_f90>0.1 && e.s1>60&&e.tdrift>5&&e.tdrift<380&&e.has_s3==false) {
            double energy = .0195*(e.correctedS1/0.154+e.correctedS2/35.34);
            henergy->Fill(energy);
            if(mdraw) cout << "Energy is " << energy << endl;
        }
    }
};