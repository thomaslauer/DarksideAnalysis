#include "../darkside/Module.h"
#include "../darkside/Event.h"

#include "TH1.h"
#include <iostream>
#include <fstream>

using namespace std;

class SaveRunID : public Module {

    ofstream fileOutput;

    void init() {
        fileOutput.open("runIdInfo.txt");
    }
    
    void processCorrections(Event& e) {

    }

    void processEvent(Event& e) {
        int nsat = 0;

        for(int i = 0; i < e.npulses; i++) {
            nsat += e.pulse_saturated[i];
        }

        fileOutput << e.run_id << " " << e.event_id << " " << nsat << endl;
    }

    void cleanup() {
        fileOutput.close();
    }
};