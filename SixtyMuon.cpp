#include "darkside/Engine.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"

#include "MuonRunNumbers/SaveRunID.h"

void SixtyMuon() {
    Engine::init("/mnt/c/Users/Thomas/Documents/Muons/sixty/slad_sixty.root");

    Engine* e = Engine::getInstance();

    e->slad->addSladFile("_allpulses.root", "pulse_info");
    e->slad->addSladFile("_masas_xy.root", "allpulses_xy");
    //e->slad->addSladFile("_veto_cluster.root", "veto");
    e->slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    e->slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    e->setOutput("sixtyMuons.root");

    e->addModule(new SaveRunID());

    e->run();
}
