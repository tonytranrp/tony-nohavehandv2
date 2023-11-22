#pragma once

#include "../../ModuleManager.h"
#include "../Module.h"

class Speed : public IModule {
private:
	int offGroundTicks = 0;
   int groundTicks = 0;
	float speed = 0.325f;
	bool ssp = true;
	int timer = 24;
	float sneakspeed = 1.f;
	float speedreal = 0.5f;
	std::string name;
	SettingEnum Mode;
public:
	Speed();
	~Speed();

	// Inherited via IModule
	virtual void onTick(C_GameMode* gm) override;
	virtual const char* getRawModuleName() override;
	virtual const char* getModuleName() override;
	virtual void onMove(C_MoveInputHandler* input) override;
	virtual void onEnable() override;
	virtual void onDisable() override;
};
