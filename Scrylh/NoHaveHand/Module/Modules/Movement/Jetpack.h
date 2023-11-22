#pragma once
#include "../Module.h"
#include "../../ModuleManager.h"

class Jetpack : public IModule {
private:
	int delay = 0;
	bool isBypass = false;
	float speedMod = 1;
	//bool hold = true;

public:
	C_MovePlayerPacket* jetpack = nullptr;
	Jetpack();
	~Jetpack();

	virtual void onEnable();
	virtual void onTick(C_GameMode* gm);
	//virtual bool isHoldMode();

	// Inherited via IModule
	virtual const char* getModuleName();
};
