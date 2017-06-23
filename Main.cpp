#include "darkside/Engine.h"

#include "TFile.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"

#include "MuonRunNumbers/SaveEventFromSlad.h"

void Main() {
    Engine::init("/mnt/c/Users/Thomas/Desktop/PhysicsCode/70daySample/UAr_70d_SLAD_v2_3_2_merged_open.root");
    Engine* e = Engine::getInstance();

    e->slad->loadDefaultSlad();

    e->addModule(new SaveEventFromSlad());


    Engine::getInstance()->run();
}
