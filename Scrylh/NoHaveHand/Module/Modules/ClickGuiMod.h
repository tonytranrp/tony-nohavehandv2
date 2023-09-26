#pragma once
#include "../../Menu/HudEditor.h"
#include "../../Menu/ClickGui.h"
#include "Module.h"

class ClickGUIMod : public IModule {
public:
	bool hudeditor = true;
	bool hasOpenedGUI = false;
	bool headerAnimation = false;
	bool openAnimation = false;
	bool showTooltips = true;
	bool rainbowColor = false;
	bool animations = true;
	bool enabled = true;
	bool sounds = true;
	bool cFont = false;
	bool outline = true;
	bool line = false;

	bool ModuleOutline = false;
	bool Crossline = false;

	float animation = 1.f;
	float openAnim = 1.f;
	int opacity = 175;
	int t = 0;
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx);
	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx);
	virtual void onPlayerTick(C_Player* plr);
	virtual void onLoadConfig(void* conf);
	virtual void onSaveConfig(void* conf);
	virtual const char* getModuleName();
	virtual bool allowAutoStart();
	virtual void onDisable();
	virtual void onEnable();
	ClickGUIMod();
};