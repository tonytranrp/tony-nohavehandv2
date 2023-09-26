#include "Crystalbreak.h"
#include <chrono>
#include <thread>
#include <queue>

Crystalbreak::Crystalbreak() : IModule(0x0, Category::COMBAT, "Destroys nearby Crystals") {
	
}
Crystalbreak::~Crystalbreak() {
}
const char* Crystalbreak::getModuleName() {
	return ("CrystalBreak");
}

static std::vector<C_Entity*> targetList20;

void findEntity20(C_Entity* currentEntity20, bool isRegularEntity) {
	

	if (currentEntity20 == nullptr)
		return;

	if (currentEntity20 == g_Data.getLocalPlayer())  // Skip Local player
		return;
	if (currentEntity20->getNameTag()->getTextLength() <= 1 && currentEntity20->getEntityTypeId() == 71)  // crystal
		return;
	


	if (!TargetUtil::isValidTarget(currentEntity20))
		return;

	float dist = (*currentEntity20->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
	if (dist < 6) {
		targetList20.push_back(currentEntity20);

		float dist = (*currentEntity20->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

		if (dist < 6) {
			targetList20.push_back(currentEntity20);
		}
	}
}

void Crystalbreak::onTick(C_GameMode* gm) {
	static std::queue<C_Entity*> targetQueue;
	
	g_Data.forEachEntity([](C_Entity* ent, bool b) {
		int id = ent->getEntityTypeId();
		
		if (id == 71 && g_Data.getLocalPlayer()->getPos()->dist(*ent->getPos()) <= 6) {
			//
			g_Data.getCGameMode()->attack(ent);
			
		}

		});
	
	
	
}