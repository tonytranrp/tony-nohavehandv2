#pragma once
#include "../../../../Utils/Utils.h"
#include "../../ModuleManager.h"
#include "../Module.h"

class Spammer : public IModule {
private:
	std::string message = "NoHaveHand V2 | Best 2b2e Client For 1.18.12";
	int length = 8;
	int Odelay = 0;
	int delay = 0;

public:
	inline std::string& getMessage() { return message; };
	inline int& getDelay() { return delay; };

	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	Spammer();
};