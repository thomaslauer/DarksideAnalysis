#include "darkside/Engine.h"
#include "darkside/Module.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"
#include "CalculateEnergy.h"

void Main() {
    Engine::init("~/SLAD/UAr_500d_SLAD_v2_3_3_merged_v0.root");

    Engine* e = Engine::getInstance();

    e->slad->loadDefaultSlad();

    e->addModule(new EnergyCorrections());
    e->addModule(new CalculateEnergy());

    e->setOutput("output/energy.root");

    e->run();
}
