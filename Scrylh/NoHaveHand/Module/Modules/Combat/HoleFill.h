#pragma once

#include "../../ModuleManager.h"
#include "../Module.h"

class HoleFill : public IModule {
private:
	bool obsidian = true;
	bool bedrock = true;
	bool onclick = false;
	SettingEnum mode = this;
public:
	int range = 5;

	bool tryHoleFill(vec3_ti blkPlacement);
	bool canpla(vec3_ti blockPos);

	HoleFill();
	~HoleFill();
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};