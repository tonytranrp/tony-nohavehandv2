#pragma once
#include "../Module.h"
#include "../../ModuleManager.h"

class FastFly : public IModule {
private:
	float speed = 9.f;
	float upanddown = 2.f;
	float glideModEffective = 100;
	float glideMod = -0.02f;
	bool superMan = false;
	int fastSpeed = 100;
	int timer = 50;
	bool isSneaking = true;
	bool airjump = true;
	

public:
	int hasJumped1 = 0;
	FastFly();
	~FastFly();

	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onMove(C_MoveInputHandler* input) override;
	virtual void onSendPacket(Packet* packet) override;
	//virtual void onEnable();
	virtual void onDisable() override { g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f); }
};
