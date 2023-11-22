#pragma once
#include "../Module.h"
class ChatSuffix : public IModule {
private:
	bool bypass = true;
public:
	ChatSuffix();
	~ChatSuffix();

	virtual const char* getModuleName() override;
	virtual void onSendPacket(Packet* packet) override;
};