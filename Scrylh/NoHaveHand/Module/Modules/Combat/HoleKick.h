#pragma once
#include "../Module.h"
class HoleKick : public IModule {
private:
	
	
	bool tryAutoFallBlock1(vec3_t HoleKick);

public:
	HoleKick();
	~HoleKick();

	int range = 10;
	
	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};
