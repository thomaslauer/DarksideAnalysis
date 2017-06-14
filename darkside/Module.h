#pragma once

#include "Event.h"

class Module {
public:
    virtual void processEvent(Event* e);
};