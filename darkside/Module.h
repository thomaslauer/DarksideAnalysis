#pragma once

#include "Event.h"

class Module {
public:
    virtual void init() {}
    virtual void processCorrections(Event& e) {}
    virtual void processEvent(Event& e) {}
    virtual void cleanup() {}
};