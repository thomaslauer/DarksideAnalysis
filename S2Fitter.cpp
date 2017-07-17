#include "darkside/Module.h"
#include "darkside/Event.h"
#include "darkside/Engine.h"

#include "S2Fitting/S2Fit.h"
#include "S2Fitting/S2FitV2.h"
#include "S2Fitting/S2FitV3.h"

void S2Fitter() {
    Engine::init("~/SLAD/slad_muon_all.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->addModule(new S2FitV3("_masa", 0));
    e->addModule(new S2FitV3("_jason", 1));
    e->addModule(new S2FitV3("_andrew", 2));
    // e->addModule(new S2Fit());

    e->setOutput("output/output_fixed.root");

    // e->runSingleEvent(event);
    e->run();
}