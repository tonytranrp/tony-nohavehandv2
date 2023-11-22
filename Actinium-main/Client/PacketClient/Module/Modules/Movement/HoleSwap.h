#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class HoleSwap : public IModule {
public:
	virtual const char* getModuleName();
	virtual void onTick(C_GameMode* gm);
	HoleSwap();
};