#include "ClientNameCommand.h"

#include "../../Module/ModuleManager.h"
#include "pch.h"

ClientNameCommand::ClientNameCommand() : IMCCommand("clientname", "", "clientname <set/reset> <string>") {
}

bool ClientNameCommand::execute(std::vector<std::string>* args) {
	assertTrue(g_Data.getLocalPlayer() != nullptr);
	if (args->at(1) == "set") {
		moduleMgr->getModule<ChatSuffix>()->getCustomMessage2() = args->at(2);
		moduleMgr->getModule<Watermark>()->getCustomWatermarkMessage() = args->at(2);
		clientMessageF("[NoHaveHand] %sClientName set to %s%s%s!", GREEN, GRAY, args->at(2).c_str(), GREEN);
	}
	if (args->at(1) == "reset") {
		moduleMgr->getModule<ChatSuffix>()->getCustomMessage2() = "NoHaveHand V0";
		moduleMgr->getModule<Watermark>()->getCustomWatermarkMessage() = "NoHaveHand V0";
		clientMessageF("[NoHaveHand] %sClientName set to %s%s%s!", GREEN, GRAY, "NoHaveHand V0", GREEN);
	}
	return true;
}