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
        fileOutput << e.run_id << " " << e.event_id << endl;
    }

    void cleanup() {
        fileOutput.close();
    }
};