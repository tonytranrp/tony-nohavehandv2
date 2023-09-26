#pragma once
#include "../Module.h"
#include "../../../../Utils/TargetUtil.h"
#include "../../ModuleManager.h"
class AutoTrap : public IModule {
private:

	bool tryAutoTrap(vec3_t AutoTrap);

public:
	AutoTrap();
	~AutoTrap();
	SettingEnum mode = this;
	int range = 7;
	bool onClick = false;
	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};
