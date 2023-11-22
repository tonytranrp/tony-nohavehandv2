#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class CrystalAuraOW : public IModule {
private:
	int delay = 0;
	bool autoplace = true;
	bool crystalCheck = true;
	bool yLock = false;
	bool isClick = false;
	//	bool doMultiple = true;
	bool silent = false;
	int slotCA = 0;
	bool renderCA = true;
	bool oldrdca = false;

public:
	CrystalAuraOW();
	~CrystalAuraOW();

	// Inherited via IModule
	virtual const char* getModuleName();
	
	int range = 7;
	
};
#pragma once
