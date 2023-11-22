#include "HoleSwap.h"

using namespace std;
HoleSwap::HoleSwap() : IModule(0, Category::MOVEMENT, "Hole Swaps") {
}

const char* HoleSwap::getModuleName() {
	return ("Center");
}

void HoleSwap::onTick(C_GameMode* gm) {
	vec3_t yR = vec3_t(floor(g_Data.getLocalPlayer()->getPos()->x), g_Data.getLocalPlayer()->getPos()->y, floor(g_Data.getLocalPlayer()->getPos()->z));
	yR.x += 0.5f;
	yR.z += 0.5f;
	g_Data.getLocalPlayer()->setPos(yR);
	moduleMgr->getModule<HoleSwap>()->setEnabled(false);
}