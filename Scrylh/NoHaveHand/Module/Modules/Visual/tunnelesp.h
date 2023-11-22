#pragma once

#include "../../ModuleManager.h"
#include "../Module.h"

class TunnelESP : public IModule {
private:
	bool obsidian = true;
	bool bedrock = true;
	float Opacity = 1.f;
	float lineOpacity = 0.6f;
	bool outline = true;

public:
	int range = 5;
	bool Height = false;
	bool lenght = false;
	bool netherrack = false;
	float LineSize = 0.4f;

	TunnelESP();
	~TunnelESP();
	// Inherited via IModule
	virtual const char* getModuleName() override;
	//virtual const char* getModName() override;
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx) override;
	//virtual void onTick(C_GameMode* gm) override;
	//virtual const char* getRawModuleName() override;
};