#include "darkside/Module.h"
#include "darkside/Event.h"
#include "darkside/Engine.h"
#include "darkside/EnergyCorrections.h"
#include "CalculateEnergy.h"

#include "S2Fitting/S2Fit.h"
#include "S2Fitting/S2FitV2.h"
#include "S2Fitting/S2FitV3.h"

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

class NpeModule : public Module {
public:

    TH1* npe;
    TH1* npesat;
    TH1* s1;

    void init() {
        npe = new TH1F("npe", "npe unsaturated", 10000, 0, 10000);
        npesat = new TH1F("npesat", "npe saturated", 1000, 0, 10000000);
        s1 = new TH1F("s1", "s1 NPE", 500, 0, 100000);
    }

    void processEvent(Event& e) {

        s1->Fill(e.s1);

        for(int i = 0; i < e.npulses; i++) {
            if(e.pulse_saturated[i]) {
                npesat->Fill(e.pulse_total_npe[i]);
            } else {
                npe->Fill(e.pulse_total_npe[i]);
            }
        }
    }
};


void S2Fitter() {
    // Engine::init("~/SLAD/slad_muon_all.root");
    Engine::init("~/SLAD/slad_muon_tight.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->addModule(new EnergyCorrections());
    e->addModule(new CalculateEnergy());

    e->addModule(new S2FitV3("_masa", 0));
    e->addModule(new S2FitV3("_jason", 1));
    e->addModule(new S2FitV3("_andrew", 2));
    e->addModule(new NpeModule());
    // e->addModule(new S2Fit());

    // e->setOutput("output/output.root");
    e->setOutput("output/output_tight.root");

    // e->runSingleEvent(event);
    e->run();
}