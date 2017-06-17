#include "darkside/Module.h"
#include "darkside/Event.h"

#include "TH1.h"
#include <iostream>

using namespace std;

class CalculateEnergy : public Module {
    
    TH1* henergy;

    void init() {
        std::cout << "RUNNING INIT" << std::endl;
        henergy = new TH1D("henergy", "energy", 10000, 0, 5000);
    }
    
    void processEvent(Event& e) {

        bool basicCuts = (e.nchannels == 38)
                && (e.baseline_not_found == false)
                && ((e.live_time + e.inhibit_time) >= 1.35e-3)
                && (e.live_time < 1.);
        
        bool runNumberCuts = (e.run_id > 14463 && e.run_id < 14731)
                || (e.run_id > 15442 && e.run_id < 15620)
                || (e.run_id > 15954 && e.run_id < 16066);
        

        if(basicCuts && !runNumberCuts && e.npulses > 3 && e.total_f90>0.15 && e.s1>60&&e.tdrift>5&&e.tdrift<380&&e.has_s3==false) {
            double energy = .0195*(e.correctedS1/0.154+e.correctedS2/35.34);
            henergy->Fill(energy);
        }
    }

    void cleanup() {
    }
};