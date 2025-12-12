#pragma once
#include "pch.h"

class Feature
{
private:
	bool Initalized = false;

public:
	Feature() {};

	virtual bool Setup() = 0;
	virtual void Destroy() = 0;
	virtual void HandleKeys() = 0;
	virtual void DrawMenuItems() = 0;
	virtual void Render() = 0;
	virtual void Run() = 0;
};
