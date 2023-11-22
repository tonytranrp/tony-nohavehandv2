#pragma once

#include "../Module.h"
#include "../../ModuleManager.h"

class AntiAnvil : public IModule {
private:
	bool caidelljthe = false;
	vec3_ti blockpos3 = { 0, 0, 0 };
	void findButt();
	int slottd2 = 0;
	float height = 3.0f; // Added height variable

public:
	bool silenttd2 = false;

	AntiAnvil();

	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onTick(C_GameMode* gm) override;
	virtual const char* getModuleName() override;
	bool isFallingAnvilAbovePlayer(C_Player* plr);

	virtual void onEnable() override;
	virtual void onDisable() override;
};
