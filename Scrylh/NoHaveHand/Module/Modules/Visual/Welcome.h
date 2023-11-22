#pragma once
#include "../Module.h"
class Welcome : public IModule {
public:
	Welcome();
	~Welcome();

	float scale = 1.f;
	int opati = 0;
	bool outline = false;
	std::string name = "";
	SettingEnum TextMode = this;
	// Inherited via IModule
	virtual const char* getModuleName();
	//virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx);
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx);
};
