#include "Spammer.h"
#include "../pch.h"

Spammer::Spammer() : IModule(0, Category::OTHER, "Spams a message in a specified delay") {
	registerIntSetting("Delay", &delay, delay, 0, 40);
	registerIntSetting("Length", &length, length, 1, 40);
}

const char* Spammer::getModuleName() {
	return ("Spammer");
}

void Spammer::onTick(C_GameMode* gm) {
	Odelay++;
	if (Odelay > delay * 20) {
		C_TextPacket textPacket;
		textPacket.message.setText(message + " | " + Utils::randomString(length));
		textPacket.sourceName.setText(g_Data.getLocalPlayer()->getNameTag()->getText());
		textPacket.xboxUserId = std::to_string(g_Data.getLocalPlayer()->getUserId());
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&textPacket);
		Odelay = 0;
	}
}