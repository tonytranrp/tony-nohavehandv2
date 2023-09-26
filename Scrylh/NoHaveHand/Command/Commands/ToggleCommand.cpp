#include "ToggleCommand.h"
#include "pch.h"

#include "../../Module/ModuleManager.h"

ToggleCommand::ToggleCommand() : IMCCommand("toggle", "Toggles a module", "<module>") {
	registerAlias("t");
}

bool ToggleCommand::execute(std::vector<std::string>* args) {
	assertTrue(args->size() >= 2);
	std::string moduleName = args->at(1);

	assertTrue(moduleName.size() > 0);

	auto modOpt = moduleMgr->getModuleByName(moduleName);
	if (!modOpt.has_value()) {
		clientMessageF("[NoHaveHand] %sCould not find Module with name: %s", RED, moduleName.c_str());
		return true;
	} 

	auto mod = modOpt.value();
	if (mod->isHoldMode()) 
		clientMessageF("[NoHaveHand] %sModule cannot be toggled!", RED);
	else {
		mod->toggle();
		clientMessageF("[NoHaveHand] %s%s is now %s", GREEN, mod->getRawModuleName(), mod->isEnabled() ? "enabled" : "disabled");
	}
		
	return true;
}
