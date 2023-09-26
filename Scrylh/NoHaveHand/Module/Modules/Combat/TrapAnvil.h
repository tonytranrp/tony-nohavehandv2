#pragma once
#include "../Module.h"
#include "../../../../Utils/TargetUtil.h"
#include "../../ModuleManager.h"
class TrapAnvil : public IModule {
private:

	bool tryAutoTrap123(vec3_t TrapAnvil);

public:
	TrapAnvil();
	~TrapAnvil();
	SettingEnum mode = this;
	int range = 7;
	bool onClick = false;
	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};
