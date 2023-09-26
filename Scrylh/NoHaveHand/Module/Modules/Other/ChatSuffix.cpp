#include "../../ModuleManager.h"

#include "ChatSuffix.h"

#include "../../../../Utils/Utils.h"
ChatSuffix::ChatSuffix() : IModule(0, Category::OTHER, "Adds the client suffix") {
}

ChatSuffix::~ChatSuffix() {
}

const char* ChatSuffix::getModuleName() {
	return "ChatSuffix";
}

void ChatSuffix::onSendPacket(Packet* packet) {
	if (packet->isInstanceOf<C_TextPacket>()) {
		C_TextPacket* funy = reinterpret_cast<C_TextPacket*>(packet);
		std::string Sentence;
		std::string start;
		std::string end;
		//auto IDKModule = moduleMgr->getModule<IDK>(); if (!IDKModule->isEnabled()) { IDKModule->setEnabled(true); };
		std::string release;
		start = "> " + Utils::randomString(9) + " | ";
		end = " | " + getCustomMessage2() + " | " + Utils::randomString(9);
		Sentence = start + funy->message.getText() + end;
		funy->message.resetWithoutDelete();
		funy->message = Sentence;
	}
}