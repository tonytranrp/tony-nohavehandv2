#pragma once
#include "../Module.h"
class AutoFallBlock : public IModule {
private:
	int obsiheight = 3;
	int fallblockheight = 3;
	bool Bypass = true;
	SettingEnum BlockID = this;
	bool tryAutoFallBlock(vec3_t AutoFallBlock);

public:
	AutoFallBlock();
	~AutoFallBlock();

	int range = 10;
	bool multiplace = false;
	bool onClick = false;
	bool notarget = false;
	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};
