#include "darkside/Engine.h"

#include "TFile.h"

#include "darkside/SladLoader.h"
#include "darkside/Module.h"
#include "darkside/EnergyCorrections.h"

#include "DemoHist.h"
#include "CalculateEnergy.h"

void Main() {
    Engine::init("/mnt/c/Users/Thomas/Desktop/PhysicsCode/70daySample/UAr_70d_SLAD_v2_3_2_merged_open.root");
    Engine::getInstance()->slad->loadDefaultSlad();
    Engine::getInstance()->setOutput("withCorrections.root");

    Module* energyCorrections = new EnergyCorrections();
    Engine::getInstance()->addModule(energyCorrections);
    
    Module* energy = new CalculateEnergy();
    Engine::getInstance()->addModule(energy);

    Engine::getInstance()->run();
}
