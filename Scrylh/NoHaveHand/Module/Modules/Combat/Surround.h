
#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class Surround : public IModule {
private:
	int delay = 0;
	bool anticity = true;
	bool citySwitch = true;

public:
	int origSlot;
	SettingEnum enum1;

	SettingEnum type;
	bool renderSurround = true;
	bool buttonplace = true;
	SettingEnum buttonidchoser;
	bool doAirplace = false;
	bool selftrap = false;
	//SettingEnum rotate;  // Rotations (none->obvious
						 //			  normal->clientside rotate
						 //			  silent->sends packet to go look at the crystal
						 //			  pitchUp->so that shit works on APVP

	Surround();
	~Surround();
	bool blockSwap = true;
	int prevSlot3 = -1;
	vec2_t rotAngle;
	bool pn = false;

	bool jumpDisable = false;
	bool disableOnComplete2 = true;
	bool center = true;
	bool spoof = true;

	std::vector<vec3_t> regularBlocks = {

		vec3_t(0, -1, 0),

		vec3_t(1, -1, 0),
		vec3_t(1, 0, 0),

		vec3_t(0, -1, 1),
		vec3_t(0, 0, 1),

		vec3_t(-1, -1, 0),
		vec3_t(-1, 0, 0),

		vec3_t(0, -1, -1),
		vec3_t(0, 0, -1),

	};

	std::vector<vec3_t> sidesBlocksOnly = {
		vec3_t(1, 0, 0),
		vec3_t(0, 0, 1),
		vec3_t(-1, 0, 0),
		vec3_t(0, 0, -1),
		vec3_t(0, -1, 0),
	};


	//void getObby();
	bool isSideEmpty();
	void onMove(C_MoveInputHandler* hand);
	//vec3_t playerPosWithFC();
	// Inherited via IModule
	virtual const char* getModuleName() override;
	//virtual bool isHoldMode() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
	//void onSendPacket(Packet* packet);
	//virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onEnable();
	//virtual void onMove(C_MoveInputHandler* hand) override;
};