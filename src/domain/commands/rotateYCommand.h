#pragma once

#include "command.h"

class RotateYCommand : public Command {
public: 
    RotateYCommand(float & target, float oldValue, float newValue)
        : target(target)
        , oldValue(oldValue)
        , newValue(newValue) { }

    void undo() override { target = oldValue; }
    void redo() override { target = newValue; }

private:
    float & target;
    float oldValue;
    float newValue;
};
