#include "darkside/Engine.h"

#include "darkside/SladLoader.h"
#include "DemoHist.h"
#include "darkside/Module.h"

void Main() {
    SladLoader* slad = new SladLoader("/mnt/c/Users/Thomas/Desktop/PhysicsCode/70daySample/UAr_70d_SLAD_v2_3_2_merged_open.root");
    slad->loadDefaultSlad();
    Engine* e = new Engine(slad);

    Module* module = new DemoHist();
    e->addModule(module);

    e->run();
}