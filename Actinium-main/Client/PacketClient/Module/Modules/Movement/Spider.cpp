#include "Spider.h"
#include "../pch.h"

using namespace std;
Spider::Spider() : IModule(0, Category::MOVEMENT, "Allows you to climb walls") {
	registerEnumSetting("Bypass", &mode, 0);
	mode.addEntry("Vanilla", 0);
	mode.addEntry("HiveJump", 1);
	//mode.addEntry("Hive", 2);
	registerFloatSetting("Speed", &speed, 0.6f, 0.1f, 1.f);
}

const char* Spider::getModuleName() {
	return "Spider";
}

void Spider::onMove(C_MoveInputHandler* input) 
{
	auto speedMod = moduleMgr->getModule<Speed>();
	
}
