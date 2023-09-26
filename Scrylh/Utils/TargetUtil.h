#pragma once
#include "../NoHaveHand/Module/ModuleManager.h"
#include "../SDK/CEntity.h"
#include <vector>

class TargetUtil {
private:
	static bool containsOnlyASCII(const std::string& string);
public:
	static void init(C_LocalPlayer** lc);
	static bool isValidTarget(C_Entity* ent);
};
