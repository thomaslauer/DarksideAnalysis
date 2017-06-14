#pragma once

#include <vector>
#include <iostream>

#include "Module.h"
#include "SladLoader.h"
#include "Event.h"

using namespace std;

class Engine {
public:
    vector<Module*> modules;

    SladLoader* slad;

    Engine(SladLoader* sladLoader) {
        slad = sladLoader;
    }

    void addModule(Module* m) {
        modules.push_back(m);
    }

    void run(int nevents = -1) {
        Event e;
        TTree* events = slad->getFullTree();
        
        slad->linkTree(events, e);

        // find number of events to run
        if(nevents == -1 || nevents > events->GetEntries()) {
            nevents = events->GetEntries();
        }

        cout << "Processing " << nevents << " events" << endl;


        for(int n = 0; n < nevents; n++) {
            events->GetEntry(n);
        }
    }
};