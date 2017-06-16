#pragma once
#include "darkside/Module.h"

#include <iostream>

#include "TH1.h"
#include "TH2.h"

using namespace std;

class DemoHist : public Module {

    TH1* testHistogram;

    void init() {
        cout << "Running init" << endl;
        testHistogram = new TH1F("testHistogram", "testing a histogram", 1000, -20, 20);
    }
    
    void processEvent(Event& e) {
        testHistogram->Fill(e.npulses);
    }

    void cleanup() {

        cout << "cleaning up" << endl;
    }
};
