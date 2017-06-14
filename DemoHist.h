#pragma once
#include "darkside/Module.h"

#include <iostream>

#include "TH1.h"

using namespace std;

class DemoHist : public Module {

    TH1* testHistogram;

    void init() {
        cout << "Running init" << endl;
        testHistogram = new TH1F("testHistogram", "testing a histogram", 50, 0, 50);
    }
    
    void processEvent(Event * e) {
        testHistogram->Fill(e->npulses);
        //cout << "running process on event " << e->event_id << endl;
    }

    void cleanup() {

        cout << "cleaning up" << endl;
        testHistogram->Draw();
    }
};
