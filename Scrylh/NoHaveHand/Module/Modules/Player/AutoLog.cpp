#include "AutoLog.h"
#include "../pch.h"

AutoLog::AutoLog() : IModule(0, Category::PLAYER, "Spams so many packets server kicks you") {
	registerIntSetting("Health", &health, health, 1, 20);
	registerEnumSetting("Text Modes", &ByeByeMessage, 0);
	registerBoolSetting("Log in Message", &message, message);
	registerBoolSetting("Custom Message", &CustomMsg, CustomMsg);
	ByeByeMessage = SettingEnum(this)
		.addEntry(EnumEntry("ByeBye!!", 0))
		.addEntry(EnumEntry("Good Bye!!", 1))
		.addEntry(EnumEntry("You Are an idiot!!", 2))
		.addEntry(EnumEntry("NoHaveHand ON TOP # Buy NoHaveHand", 3));
}
AutoLog::~AutoLog() {
}

const char* AutoLog::getRawModuleName() {
	return ("AutoLog");
}

const char* AutoLog::getModuleName() {
	if (CustomMsg == true) { name = std::string("AutoLog ") + std::string(BLUE) + std::string("[") + std::string(RED) + std::string("CUSTOM") + std::string(BLUE) + std::string("]"); };
	return name.c_str();
}

void AutoLog::onTick(C_GameMode* gm) {
	int healthy = g_Data.getLocalPlayer()->getHealth();
	if (healthy < health) {
		if (message == true) {
			C_TextPacket textPacket;
			if (ByeByeMessage.selected == 0) {
				textPacket.message.setText("ByeBye!!");
			}
			if (ByeByeMessage.selected == 1) {
				textPacket.message.setText("Good Bye!!");
			}
			if (ByeByeMessage.selected == 2) {
				textPacket.message.setText("You Are an idiot!!");
			}
			if (ByeByeMessage.selected == 3) {
				textPacket.message.setText("NoHaveHand ON TOP # Buy NoHaveHand");
			}
			if (CustomMsg) {
				textPacket.message.setText(getCustomMessage());
			}
			textPacket.sourceName.setText(g_Data.getLocalPlayer()->getNameTag()->getText());
			textPacket.xboxUserId = std::to_string(g_Data.getLocalPlayer()->getUserId());
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&textPacket);
		};
		for (int pp = 0; pp < 1000; pp++) {
			C_MovePlayerPacket movePacket(g_Data.getLocalPlayer(), *g_Data.getLocalPlayer()->getPos());
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&movePacket);
		}
	}
}