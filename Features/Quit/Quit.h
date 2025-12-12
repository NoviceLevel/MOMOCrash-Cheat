#pragma once
#include "pch.h"

class Quit : public Feature
{
private:
    bool Initalized = false;

public:
    Quit() {};

    virtual bool Setup()
    {
        Initalized = true;
        return true;
    }

    virtual void Destroy()
    {
        if (!Initalized)
            return;
    }

    virtual void HandleKeys() {}

    virtual void DrawMenuItems() {}

    virtual void Render() {}

    virtual void Run() {}
};
