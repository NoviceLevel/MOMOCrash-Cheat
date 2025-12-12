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
        Utils::LogToFile("Quit::Setup() started");
        Initalized = true;
        Utils::LogToFile("Quit::Setup() completed");
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
