#include "Animations.h"
#include "../pch.h"
#include "../../../../Memory/Hooks.h"

void* SmoothSwing = (void*)FindSignature("0F 84 ? ? ? ? 48 8B 56 ? 48 85 D2 74 ? 48 8B 02");
void* TapAddress = (void*)FindSignature("F3 0F 51 F0 0F 28 C8");

using namespace std;
Animations::Animations() : IModule(0, Category::VISUAL, "Changes the swing/hitting animation") {
	registerFloatSetting("X", &xPos, xPos, -32.f, 32.f);
	registerFloatSetting("Y", &yPos, yPos, -32.f, 32.f);
	registerFloatSetting("Z", &zPos, zPos, -32.f, 32.f);
	registerFloatSetting("SwingSpeed", &swingSpeed, swingSpeed, -16.f,16.f);
	registerBoolSetting("SmoothSwing", &smoothSwing, smoothSwing);
	registerBoolSetting("Swing", &swing, swing);
	registerBoolSetting("Reset", &reset, reset);
}

const char* Animations::getModuleName() {
	return "ItemModel";
}

void Animations::onEnable() {
}

void Animations::onTick(C_GameMode* gm) {
	auto killaura = moduleMgr->getModule<Killaura>();
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	auto slot = player->getSupplies()->inventory->getItemStack(player->getSupplies()->selectedHotbarSlot);
	
	

	if (smoothSwing) Utils::nopBytes((unsigned char*)SmoothSwing, 6);
	else Utils::patchBytes((unsigned char*)((uintptr_t)SmoothSwing), (unsigned char*)"\x0F\x84\x95\x02\x00\x00", 6);

	if (shouldBlock) {
		//if (mode.getSelectedValue() == 1) Utils::nopBytes((unsigned char*)SmoothSwing, 6);
		if (!swing) *player->getSwingState() = 0.f;
	}

	if (reset) { 
		xScale = 1.f; yScale = 1.f; zScale = 1.f; 
		xPos = 0.f; yPos = 0.f; zPos = 0.f; 
		reset = false;
	}
}

void Animations::onPlayerTick(C_Player* plr) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	/*/
	if (swingSpeed >= 2005) swingSpeed = 340;
	else swingSpeed += 1.f;
	if (swingSpeed <= 339) swingSpeed = 340;
	/*/
	if (isAttacking && shouldBlock && !g_Hooks.isThirdPerson) { float* speedAdr = reinterpret_cast<float*>(reinterpret_cast<__int64>(player) + 0x79C); *speedAdr = swingSpeed; }
}

void Animations::onDisable() {
	if (smoothSwing) Utils::patchBytes((unsigned char*)((uintptr_t)SmoothSwing), (unsigned char*)"\x0F\x84\x95\x02\x00\x00", 6);
}