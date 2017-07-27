#include "darkside/Module.h"
#include "darkside/Event.h"

#include "TH1.h"
#include <iostream>
#include <fstream>

using namespace std;

class SaveEventFromSlad : public Module {

    ifstream fileInput;
    ofstream fileOutput;
    vector<int> runNumbers;
    vector<int> eventNumbers;
    vector<int> nsat;

    void init() {
        fileInput.open("runIdInfo.txt");
        fileOutput.open("sladFileNumbers.txt");
        int runNum, eventNum, nsaturated;

        while(fileInput >> runNum >> eventNum >> nsaturated) {
            runNumbers.push_back(runNum);
            eventNumbers.push_back(eventNum);
            nsat.push_back(nsaturated);
        }
    }

    void processEvent(Event& e) {
        for(int i = 0; i < runNumbers.size(); i++) {
            if(e.run_id == runNumbers[i] && e.event_id == eventNumbers[i]) {

                fileOutput << Engine::getInstance()->getCurrentSladEvent() << " " << i
                        << " " << e.run_id << " " << e.event_id << " " << e.npulses << endl;
            }
        }
    }

    void cleanup() {
        fileInput.close();
        fileOutput.close();
    }
};