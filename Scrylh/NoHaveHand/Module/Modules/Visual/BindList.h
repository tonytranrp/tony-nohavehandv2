#pragma once
#include "../Module.h"
class BindList : public IModule {
public:
	BindList();
	~BindList();

	// Inherited via IModule
	virtual const char* getModuleName();
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx);
	//virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
};
