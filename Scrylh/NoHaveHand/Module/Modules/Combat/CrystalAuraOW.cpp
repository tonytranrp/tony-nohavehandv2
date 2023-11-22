#define _USE_MATH_DEFINES // Required for defining M_PI

#include <unordered_set>
#include <algorithm> // Include this header for std::remove_if and std::sort
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "CrystalAuraOW.h"
#include <chrono> // Include this header for std::chrono
#include <vector>
#include <cmath>
#include <unordered_map>
#include <queue>




CrystalAuraOW::CrystalAuraOW() : IModule(0x0, Category::COMBAT, "CrystalAura by Tony UWU") {
	registerIntSetting("range", &range, range, 1, 10);
	registerBoolSetting("autoplace", &autoplace, autoplace);
	//registerBoolSetting("onlyCrystal", &crystalCheck, crystalCheck);
	//registerBoolSetting("LockY", &yLock, yLock);
	registerIntSetting("delay", &delay, delay, 0, 20);
	registerBoolSetting("onClick", &isClick, isClick);
	//	registerBoolSetting("Multi", &doMultiple, doMultiple);
	registerBoolSetting("Silent", &silent, silent);
	registerBoolSetting("Render", &renderCA, renderCA);
	registerBoolSetting("Old render", &oldrdca, &oldrdca);
}
int crystalDelay1 = 0;
int crystalDelay21 = 0;
int crystalDelay31 = 0;

CrystalAuraOW::~CrystalAuraOW() {
}

const char* CrystalAuraOW::getModuleName() {
	return ("CrystalAuraAnvils");
}
static std::vector<C_Entity*> targetList7;

struct CompareTargetEnArray1 {
	bool operator()(C_Entity* lhs, C_Entity* rhs) {
		C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
		return (*lhs->getPos()).dist(*localPlayer->getPos()) < (*rhs->getPos()).dist(*localPlayer->getPos());
	}
};

