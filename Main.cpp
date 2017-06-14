#include "darkside/Engine.h"

#include "darkside/SladLoader.h"

void Main() {
    SladLoader* slad = new SladLoader("/mnt/c/Users/Thomas/Desktop/PhysicsCode/70daySample/UAr_70d_SLAD_v2_3_2_merged_open.root");
    slad->addSladFile("_allpulses.root", "pulse_info");
    slad->addSladFile("_masas_xy.root", "allpulses_xy");
    slad->addSladFile("_veto_cluster.root", "veto");
    slad->addSladFile("_xylocator_xy.root", "allpulses_xyl_xy");
    slad->addSladFile("_aww_xy.root", "allpulses_aww_xy");

    Engine* e = new Engine(slad);
    e->run();
}