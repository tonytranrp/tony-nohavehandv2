#include "AutoLogCommand.h"

#include "../../Module/ModuleManager.h"
#include "pch.h"

AutoLogCommand::AutoLogCommand() : IMCCommand("AutoLog", "Edit AutoLog text", "<string>") {
}

bool AutoLogCommand::execute(std::vector<std::string>* args) {
	assertTrue(g_Data.getLocalPlayer() != nullptr);
	auto AutoLogModule = moduleMgr->getModule<AutoLog>();
	std::string text = args->at(1);
	AutoLogModule->getCustomMessage() = text;
	clientMessageF("[NoHaveHand] %sAutoLog message set to %s%s%s!", GREEN, GRAY, text.c_str(), GREEN);
	return true;
}