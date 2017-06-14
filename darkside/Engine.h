#pragma once

#include <vector>
#include <iostream>

#include "TThread.h"

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

        for(unsigned int i = 0; i < modules.size(); i++) {
            modules[i]->init();
        }

        slad->linkTree(events, e);

        // find number of events to run
        if(nevents == -1 || nevents > events->GetEntries()) {
            nevents = events->GetEntries();
        }

        cout << "Processing " << nevents << " events" << endl;
        for(int n = 0; n < nevents; n++) {
            events->GetEntry(n);

            if (n%100000==0) {
                std::cout << "Processing event " << n << "/" << nevents << std::endl;
            }

            for(unsigned int i = 0; i < modules.size(); i++) {
                modules[i]->processEvent(&e);
            }
        }

        for(unsigned int i = 0; i < modules.size(); i++) {
            modules[i]->cleanup();
        }
    }
};