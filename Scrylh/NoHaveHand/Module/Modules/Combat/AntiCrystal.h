#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"
class AntiCrystal : public IModule {
public:
	float Height = 0.1f;

	AntiCrystal() : IModule(0x0, Category::COMBAT, "AntiCrystal") {
		registerFloatSetting("Height", &Height, Height, 0.f, 1.f);
	};
	~AntiCrystal(){};

	void onSendPacket(Packet* packet) {
		if (g_Data.getLocalPlayer() == nullptr) return;
		vec3_t* pos = g_Data.getLocalPlayer()->getPos();
		if (packet->isInstanceOf<PlayerAuthInputPacket>()) {
			PlayerAuthInputPacket* InputPacket = reinterpret_cast<PlayerAuthInputPacket*>(packet);
			InputPacket->pos.y = static_cast<float>(pos->y - Height);
		} else if (packet->isInstanceOf<C_MovePlayerPacket>()) {
			C_MovePlayerPacket* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
			movePacket->Position.y = static_cast<float>(pos->y - Height);
		}
	}

	virtual const char* getModuleName() override {
		return "AntiCrystal";
	}
};
#pragma once
