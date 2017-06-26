#include "darkside/Engine.h"

#include "S2Fitter.h"

void testS2() {
    Engine::init("~/SLAD/slad_fifty_t4.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->addModule(new S2Fitter());

    //e->runSingleEvent(4);
    e->run();
}
