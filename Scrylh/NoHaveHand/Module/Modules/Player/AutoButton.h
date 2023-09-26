#pragma once
#include "../Module.h"
class AutoButton : public IModule {
private:
	bool silentab = true;
	void findButt();
	int slotab = 0;
	int place;
	bool onClick = false;

public:
	AutoButton();
	~AutoButton();
	bool AutoButton_tryPlace1(vec3_ti blkPlacement);
	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;

	//virtual const char* getRawModuleName() override;
};
