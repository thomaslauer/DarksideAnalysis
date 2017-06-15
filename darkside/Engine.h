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

    TString output = "output.root";

    SladLoader* slad;

    Engine(SladLoader* sladLoader) {
        slad = sladLoader;
    }

    void addModule(Module* m) {
        modules.push_back(m);
    }

    void setOutput(TString newOutputFile) {
        output = newOutputFile;
    }

    void run(int nevents = -1) {
        Event e;
        TTree* events = slad->getFullTree();

        TFile* outputFile = new TFile(output, "recreate");
        outputFile->cd();

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
        outputFile->Write();
        outputFile->Close();
    }
};