#pragma once
#include <ctime>

#include "../../../../Utils/TargetUtil.h"
#include "../../ModuleManager.h"
#include "../Module.h"

class AutoEZ : public IModule {
private:
	bool sayUserDied = true;
	bool sayEnemDied = true;

	std::string deathMsg = "GG~ NoHaveHand V2 on top!";
	std::string killMsg = "GG~NoHaveHand V2 on top!";

public:
	bool foundPlayer = false;
	bool isDead = false;
	bool antiSpamCheck = true;
	AutoEZ();
	~AutoEZ();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};