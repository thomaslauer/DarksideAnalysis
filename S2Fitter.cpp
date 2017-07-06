#include "darkside/Module.h"
#include "darkside/Event.h"
#include "darkside/Engine.h"

#include "S2Fitting/S2Fit.h"
#include "S2Fitting/S2FitV2.h"
#include "S2Fitting/S2FitV3.h"

void S2Fitter() {
    Engine::init("~/SLAD/slad_350_mu.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->addModule(new S2FitV3());
    //e->addModule(new S2FitV2());

    e->setOutput("output/outputv3.root");

    //e->runSingleEvent(4);
    e->run();
}