#include "darkside/Engine.h"

#include "TFile.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"

#include "MuonRunNumbers/SaveEventFromSlad.h"

void Main() {
    Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");
    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    // e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_veto_cluster.root", "veto");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->addModule(new SaveEventFromSlad());


    Engine::getInstance()->run();
}
