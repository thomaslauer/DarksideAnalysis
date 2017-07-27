#include "darkside/Engine.h"
#include "darkside/Module.h"

#include "TFile.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"
#include "CalculateEnergy.h"

#include "MuonRunNumbers/SaveEventFromSlad.h"

#include <fstream>

class npePerPulse : public Module {
public:

    float threshNPE = 100;

    int xyAlgorithm = 0;

    ofstream output;
    void init() {
        output.open("output/fullDetector.txt");
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

        bool filledFullDetector = true;
        for(int i = 0; i < e.npulses; i++) {
            if(e.pulse_total_npe[i] < threshNPE) {
                filledFullDetector = false;
                break;
            }
        }

        int goodXY = 0;
        for(int i = 0; i < e.npulses; i++) {
            bool goodPulse = (data_x[i] > -20)
                            && (data_x[i] < 20)
                            && (data_y[i] > -20)
                            && (data_y[i] < 20);
            if(goodPulse) {
                goodXY++;
            }
        }

        if(filledFullDetector && basicCuts) {
            output << Engine::getInstance()->getCurrentSladEvent() << " " << e.run_id << " " << e.event_id << " " << goodXY << endl;
        }
    }

    void cleanup() {
        output.close();
    }

};

void Main() {
    Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");
    // Engine::init("~/SLAD/slad_muon_all.root");

    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    // e->addModule(new EnergyCorrections());
    e->addModule(new CalculateEnergy());
    // e->addModule(new npePerPulse());

    e->setOutput("output/energy.root");

    e->run();
}
