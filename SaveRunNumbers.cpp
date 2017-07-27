#include "darkside/Engine.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"

#include "MuonRunNumbers/SaveRunID.h"
#include "MuonRunNumbers/SaveEventFromSlad.h"

void SaveRunNumbers() {
    //Engine::init("~/SLAD/slad_muon_all.root");
    // Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");
    Engine::init("~/SLAD/slad_muon_tight_more.root");

    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    // e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    // e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    // e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->setOutput("output/tightMuons.root");

    e->addModule(new SaveRunID()); // run on muons
    // e->addModule(new SaveEventFromSlad()); // run on full slad file

    e->run();
}
