#include "darkside/Module.h"
#include "darkside/Event.h"
#include "darkside/Engine.h"

#include "TH2.h"
#include "TH1.h"

using namespace std;

class PlotNPEvsPulse : public Module {
    
    TH1* pulseTime;
    TH1* pulseNPE;
    TH2* npevstime;
    TH1* pulseTime3;
    TH1* pulseNPE3;
    TH2* npevstime3;

    void init() {
        pulseTime = new TH1F("pulseTime", "pulseTime", 380, 0, 380);
        pulseNPE = new TH1F("pulseNPE", "pulseNPE", 100, 0, 1000);
        npevstime = new TH2F("npevstime", "npevstime", 380, 0, 380, 1000, 0, 100000);
        pulseTime3 = new TH1F("pulseTime3", "pulseTime", 380, 0, 380);
        pulseNPE3 = new TH1F("pulseNPE3", "pulseNPE", 100, 0, 1000);
        npevstime3 = new TH2F("npevstime3", "npevstime", 380, 0, 380, 1000, 0, 100000);
    }

    void processEvent(Event& e) {
        // for(int i = 1; i < e.npulses; i++) {
        //     pulseNPE->Fill(e.pulse_total_npe[i]);
        //     pulseTime->Fill(e.pulse_end_time[i] - e.pulse_start_time[i]);
        // }

        if(e.npulses == 2) {
            pulseNPE->Fill(e.pulse_total_npe[1]);
            pulseTime->Fill(e.pulse_end_time[1] - e.pulse_start_time[1]);
            npevstime->Fill(e.pulse_end_time[1] - e.pulse_start_time[1], e.pulse_total_npe[1]);
        }

        if(e.npulses == 3) {
            pulseNPE3->Fill(e.pulse_total_npe[2]);
            pulseTime3->Fill(e.pulse_end_time[2] - e.pulse_start_time[2]);
            npevstime3->Fill(e.pulse_end_time[2] - e.pulse_start_time[2], e.pulse_total_npe[2]);
        }
    }

    void cleanup() {}
};


void S2NPE() {
    // Engine::init("~/SLAD/slad_fifty_t4.root");
    Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    //e->addModule(new S2Fit());
    e->addModule(new PlotNPEvsPulse());

    e->setOutput("output/output.root");

    //e->runSingleEvent(4);
    e->run();
}