#pragma once

#include <vector>
#include <iostream>

#include "Module.h"
#include "SladLoader.h"
#include "Event.h"
#include "TStopwatch.h"

using namespace std;

class Engine {
private:
    static Engine* currentEngine;

    Engine(TString filename) {
        outputFilename = "output.root";
        slad = new SladLoader(filename);
    }

    int currentEvent = 0;
public:
    vector<Module*> modules;

    TFile* outputFile;
    TString outputFilename;

    SladLoader* slad;

    static void init(TString filename) {
        currentEngine = new Engine(filename);
    }

    static Engine* getInstance() {
        return currentEngine;
    }

    void addModule(Module* m) {
        modules.push_back(m);
    }

    void setOutput(TString newOutputFile) {
        outputFilename = newOutputFile;
    }

    void run(int nevents = -1) {

        TStopwatch* clock = new TStopwatch();
	    clock->Start();

        Event e;
        TTree* events = slad->getFullTree();

        slad->linkTree(events, e);

        outputFile = new TFile(outputFilename, "RECREATE");
        outputFile->cd();

        for(unsigned int i = 0; i < modules.size(); i++) {
            modules[i]->init();
        }

        // find number of events to run
        if(nevents == -1 || nevents > events->GetEntries()) {
            nevents = events->GetEntries();
        }

        cout << "Processing " << nevents << " events" << endl;
        for(int n = 0; n < nevents; n++) {
            currentEvent = n;
            events->GetEntry(n);

            if (n%100000==0) {
                std::cout << "Processing event " << n << "/" << nevents << std::endl;
            }


            for(unsigned int i = 0; i < modules.size(); i++) {
                // apply corrections to event
                modules[i]->processCorrections(e);
            }

            for(unsigned int i = 0; i < modules.size(); i++) {
                // process the event
                modules[i]->processEvent(e);
            }
        }

        for(unsigned int i = 0; i < modules.size(); i++) {
            modules[i]->cleanup();
        }
        outputFile->cd();
        outputFile->Write();
        outputFile->Close();

        std::cout << "Done!"<<" "<<clock->RealTime()<<" s."<<std::endl;
    }

    void runSingleEvent(int eventNumber) {

        Event e;
        TTree* events = slad->getFullTree();
        slad->linkTree(events, e);

        for(unsigned int i = 0; i < modules.size(); i++) {
            modules[i]->init();
        }

        events->GetEntry(eventNumber);

        for(unsigned int i = 0; i < modules.size(); i++) {
            // apply corrections to event
            modules[i]->processCorrections(e);
        }

        for(unsigned int i = 0; i < modules.size(); i++) {
            // process the event
            modules[i]->processEvent(e);
        }

        for(unsigned int i = 0; i < modules.size(); i++) {
            modules[i]->cleanup();
        }
    }

    unsigned int getCurrentSladEvent() {
        return currentEvent;
    }
};


Engine* Engine::currentEngine = NULL;