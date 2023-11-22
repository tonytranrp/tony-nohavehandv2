#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"
#include "../pch.h"
#include <chrono>
class Watermark : public IModule {
private:
	float scale = 1.f;
	int opacity = 150;
public:
	bool outlinec = true;
	std::string message = "NoHaveHand V2";
	inline std::string& getCustomWatermarkMessage() { return message; };
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx);
	virtual const char* getModuleName();
	virtual void onEnable() override;
	Watermark();
};