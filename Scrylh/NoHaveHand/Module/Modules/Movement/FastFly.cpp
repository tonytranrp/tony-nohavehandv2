#include "FastFly.h"
#include "../pch.h"

FastFly::FastFly() : IModule(0x0, Category::MOVEMENT, "TRY IT TO FLY LIKE AKENO X SONGOKUUU.") {
	registerFloatSetting("Horizontal Speed", &this->speed, this->speed, 1.f, 10.f);
	registerFloatSetting("Vertical Speed", &this->upanddown, this->upanddown, 1.f, 10.f);
	this->registerBoolSetting("Super Speed", &this->superMan, this->superMan);
	registerIntSetting("ticks per second", &timer, timer, 20, 30);
	registerFloatSetting("Value", &glideMod, glideMod, -1, 1);
	registerBoolSetting("Sneak", &isSneaking, isSneaking);
	
}

FastFly::~FastFly() {
}

const char* FastFly::getModuleName() {
	return ("Ecpe-Fly");
}
uint8_t* sneakopcode1 = 0;
int tony = 0;
void FastFly::onTick(C_GameMode* gm) {
	
	gm->player->velocity = vec3_t(0, 0, 0);
	glideModEffective = glideMod;
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	

	g_Data.getClientInstance()->minecraft->setTimerSpeed(static_cast<float>(timer));

	if (!superMan) {
		if (g_Data.canUseMoveKeys()) {
			if (GameData::isKeyDown(*input->spaceBarKey))
				glideModEffective += upanddown;
			if (GameData::isKeyDown(*input->sneakKey))
				glideModEffective -= upanddown;
		}
		gm->player->velocity.y = glideModEffective;
	}
	
	
	
	else {
		if (g_Data.canUseMoveKeys()) {
			if (GameData::isKeyDown(*input->spaceBarKey))
				glideModEffective += fastSpeed;
			if (GameData::isKeyDown(*input->sneakKey))
				glideModEffective -= fastSpeed;

			if (input->forwardKey && input->backKey && input->rightKey && input->leftKey) {
				gm->player->velocity = vec3_t(0, 0, 0);
			}
		}
		gm->player->velocity.y = glideModEffective;

		g_Data.getClientInstance()->minecraft->setTimerSpeed(static_cast<float>(timer));

		
		

		
		if (!gm->player->onGround) {
			C_MovePlayerPacket pLifeboat(g_Data.getLocalPlayer(), *g_Data.getLocalPlayer()->getPos());
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&pLifeboat);
			
			if (isSneaking) {
				auto player = g_Data.getLocalPlayer();
				if (player == nullptr) return;
				g_Data.getClientInstance()->getMoveTurnInput()->isSneakDown = true;
				if (sneakopcode1 == 0) {
					sneakopcode1 = reinterpret_cast<uint8_t*>(FindSignature("80 7B 48 00 74 2E 48 8B 07 48 8B CF"));
				}
				DWORD oldProtect = 0;
				if (!VirtualProtect(sneakopcode1, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				}
				else {
					sneakopcode1[0] = 0x90;
					sneakopcode1[1] = 0x90;
					sneakopcode1[2] = 0x90;
					sneakopcode1[3] = 0x90;
					VirtualProtect(sneakopcode1, 5, oldProtect, &oldProtect);
				}
			}
		}
	}
	
}

void FastFly::onMove(C_MoveInputHandler* input) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	float yaw = player->yaw;

	bool pressed = input->forward || input->backward || input->right || input->left;

	if (input->forward && input->backward)
		return;

	if (input->right) {
		yaw += 90.f;
		if (input->forward)
			yaw -= 45.f;
		else if (input->backward)
			yaw += 45.f;
	}
	if (input->left) {
		yaw -= 90.f;
		if (input->forward)
			yaw += 45.f;
		else if (input->backward)
			yaw -= 45.f;
	}

	if (input->backward && !input->left && !input->right)
		yaw += 180.f;

	if (!superMan) {
		float calcYaw = (yaw + 90) * (PI / 180);
		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * speed;
		moveVec.y = player->velocity.y;
		moveVec.z = sin(calcYaw) * speed;
		if (pressed) player->lerpMotion(moveVec);
	} else {
		float calcYaw = (yaw + 90) * (PI / 180);
		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * speed;
		moveVec.y = player->velocity.y;
		moveVec.z = sin(calcYaw) * speed;
		if (pressed) player->lerpMotion(moveVec);
	}
}

void FastFly::onSendPacket(Packet* packet) {
	if (packet->isInstanceOf<C_MovePlayerPacket>()) {
		auto player = g_Data.getLocalPlayer();
		if (!player->onGround) {
			C_MovePlayerPacket* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
			movePacket->onGround = true;
		}
	}
	if (packet->isInstanceOf<C_MovePlayerPacket>()) {
		auto player = g_Data.getLocalPlayer();
		if (!player->onGround) {
			C_MovePlayerPacket* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
			movePacket->onGround = true;
		}
	}
}

void onDisable() {
	g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);

	if (g_Data.getLocalPlayer() != nullptr) {
		g_Data.getClientInstance()->getMoveTurnInput()->isSneakDown = false;
	}
}