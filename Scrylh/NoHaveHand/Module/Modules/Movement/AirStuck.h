#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"
class AirStuck : public IModule {
public:

	AirStuck() : IModule(0x0, Category::MOVEMENT, "stuck in the air") {

	};
	~AirStuck(){};

	void onTick(C_GameMode* gm) {
		gm->player->velocity = vec3_t(0, 0, 0);
	}

	virtual const char* getModuleName() override {
		return "AirStuck";
	}
};