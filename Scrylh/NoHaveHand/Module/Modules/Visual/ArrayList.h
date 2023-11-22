#pragma once
#include "../../Utils/DrawUtils.h"
#include "../../Utils/ColorUtil.h"
#include "../../ModuleManager.h"
#include "../Module.h"

class ArrayList : public IModule {
private:
	SettingEnum mode = this;
	SettingEnum animation = this;

	float spacing = 0.f;
	float scale = 1.f;
	bool bottom = false;
	int opacity = 150;
public:
	bool showkb = true;
	bool focused = false;
	bool invert = false;
	bool modes = true;

	// Positions
	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
	float positionX = windowSize.x;
	float positionY = 0.f;
	std::unordered_map<std::shared_ptr<IModule>, bool> prevEnabledStates;
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx);
	virtual const char* getModuleName();
	ArrayList();
};