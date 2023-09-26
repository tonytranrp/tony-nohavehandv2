#pragma once
#include "../Module.h"
class ChatSuffix : public IModule {
public:
	ChatSuffix();
	~ChatSuffix();
	std::string Suffix = "NoHaveHand V2";
	inline std::string& getCustomMessage2() { return Suffix; };

	virtual const char* getModuleName() override;
	virtual void onSendPacket(Packet* packet) override;
};