#include "darkside/Module.h"
#include "darkside/Event.h"
#include "darkside/Engine.h"

#include "TH2.h"
#include "TH1.h"

using namespace std;

class PlotNPEvsPulse : public Module {
    
    TH1* pulseTime;

    TH2* pulseStartTimes;
    TH2* pulseEndTimes;

    TH2* npevspulse;

    TH1* pulseNPE;
    TH2* npevstime;

    void init() {
        pulseTime = new TH1F("pulseTime", "pulseDuration", 380, 0, 380);
        pulseNPE = new TH1F("pulseNPE", "pulseNPE", 10000, 0, 100000);
        npevstime = new TH2F("npevstime", "npevstime", 150, 0, 380, 500, 0, 100000);

        npevspulse = new TH2F("npevspulse", "NPE vs Pulse", 18, 0, 18, 25, 100, 10000);

        pulseStartTimes = new TH2F("pulseStartTimes", "Pulse Starts", 100, 0, 450, 20, 0, 20);
        pulseEndTimes = new TH2F("pulseEndTimes", "Pulse Ends", 100, 0, 450, 20, 0, 20);
    }

    void processEvent(Event& e) {
        // for(int i = 1; i < e.npulses; i++) {
        //     pulseNPE->Fill(e.pulse_total_npe[i]);
        //     pulseTime->Fill(e.pulse_end_time[i] - e.pulse_start_time[i]);
        // }

        bool runNumberCuts = (e.run_id > 14463 && e.run_id < 14731)
                || (e.run_id > 15442 && e.run_id < 15620)
                || (e.run_id > 15954 && e.run_id < 16066);

        bool basicCuts = (e.nchannels == 38)
                && (e.baseline_not_found == false)
                && ((e.live_time + e.inhibit_time) >= 1.35e-3)
                && (e.live_time < 1.)
                && !runNumberCuts
                && e.total_f90>0.15 && e.s1>60&&e.tdrift>5&&e.tdrift<380&&e.has_s3==false;

        if(basicCuts) {
            

            for(int i = 1; i < e.npulses; i++) {
                pulseStartTimes->Fill(e.pulse_start_time[i], i);
                pulseEndTimes->Fill(e.pulse_end_time[i], i);
                npevspulse->Fill(i, e.pulse_total_npe[i]);
                pulseNPE->Fill(e.pulse_total_npe[i]);
            }

            pulseTime->Fill(e.pulse_end_time[1] - e.pulse_start_time[1]);
            npevstime->Fill(e.pulse_end_time[1] - e.pulse_start_time[1], e.pulse_total_npe[1]);
        }
    }

    void cleanup() {}
};


void S2NPE() {
    // Engine::init("~/SLAD/slad_fifty_t4.root");
    Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");
    // Engine::init("~/SLAD/slad_350_mu.root");

    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    //e->addModule(new S2Fit());
    e->addModule(new PlotNPEvsPulse());

    e->setOutput("output/500day.root");

    //e->runSingleEvent(4);
    e->run();
}