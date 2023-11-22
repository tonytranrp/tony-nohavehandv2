#include "ElytraFly.h"
#include "../pch.h"
#include <cmath>



using namespace std;
ElytraFly::ElytraFly() : IModule(0, Category::MOVEMENT, "Allows you to with elytra while in the sky, use CTRL to go up and shift to go down") {
	registerEnumSetting("Mode", &mode, 0);
	mode.addEntry("Vanilla", 0);
	
	//mode.addEntry("Jetpack", 2);
	//mode.addEntry("Jump", 3);
	//mode.addEntry("Hive", 4);
	//mode.addEntry("Cubecraft", 5);
	//mode.addEntry("Test", 6);
	
	
	
	registerFloatSetting("X", &speedx, speedx, 0.2f, 5.f);
	registerFloatSetting("Y", &speed, speed, 0.2f, 5.f);
}

const char* ElytraFly::getRawModuleName() {
	return "ElytraFly";
}

const char* ElytraFly::getModuleName() {
	if (mode.getSelectedValue() == 0)
		name = string("ElytraFly ") + string(GRAY) + string("[") + string(WHITE) + string("Vanila") + string(GRAY) + string("]");
	if (mode.getSelectedValue() == 1)
		name = string("ElytraFly ") + string(GRAY) + string("[") + string(WHITE) + string("Airwalk") + string(GRAY) + string("]");
	return name.c_str();
}

float hiveSpeedArray1[15] = {
0.910000,
0.615559,
0.583347,
0.554032,
0.527356,
0.503081,
0.480991,
0.460887,
0.442595,
0.425948,
0.410799,
0.397015,
0.384470,
0.373055,
0.362665
};

float hiveSafeSpeedArray1[12] = {
0.40000,
0.38000,
0.36000,
0.34000,
0.32000,
0.30000,
0.29000,
0.28000,
0.26000,
0.24000,
0.22000,
0.20347
};

void ElytraFly::onEnable() {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	// Variables
	auto speedMod = moduleMgr->getModule<Speed>();
	auto blinkMod = moduleMgr->getModule<Blink>();
	if (speedMod->isEnabled()) speedWasEnabled = true;
	PointingStruct* level = player->pointingStruct;
	vec3_t pos2 = *player->getPos();
	vec3_t* pos = player->getPos();
	
	tick = 0;

	
	

	switch (mode.getSelectedValue()) {
	case 3: // Jump
		enabledHeight = (int)floorf(pos->y);
		break;
	case 4: // Hive
		enabledTick++;
		player->jumpFromGround();
		if (enabledTick >= 1) {
			//pos2.y += 0.1f;
			//player->setPos(pos2);
		}
		break;
	case 6: // Test
		testfloat = speed;
		timerVariable = 5;
		g_Data.getClientInstance()->minecraft->setTimerSpeed(timerVariable);
		blinkMod->setEnabled(true);
		break;
	}
}
/*
bool ElytraFly::isHoldMode() {
	return mode.getSelectedValue() == 2;
}
*/

void ElytraFly::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	tick++;

	// Math
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	if (mode.getSelectedValue() != 5)
		g_Data.getClientInstance()->getMoveTurnInput()->isSneakDown = false;

	auto targetStrafe = moduleMgr->getModule<TargetStrafe>();
	auto speedMod = moduleMgr->getModule<Speed>();
	auto blinkMod = moduleMgr->getModule<Blink>();
	auto blink = moduleMgr->getModule<Blink>();

	float yaw = player->yaw; float pitch = player->pitch;
	float velocity = g_Data.getLocalPlayer()->velocity.magnitude();
	float velocityxz = g_Data.getLocalPlayer()->velocity.magnitudexz();
	float velocityxy = g_Data.getLocalPlayer()->velocity.magnitudexy();
	bool jumping = GameData::isKeyDown(VK_CONTROL);
	bool sneaking = GameData::isKeyDown(*input->sneakKey);
	vec3_t* pos = g_Data.getLocalPlayer()->getPos();
	float calcYaw = (yaw + 90) * (PI / 180);
	float calcPitch = (pitch) * -(PI / 180);
	vec3_t moveVec;

	if (speedMod->isEnabled()) {
		//auto notification = g_Data.addNotification("ElytraFly:", "Disabled Speed"); notification->duration = 5;
		speedMod->setEnabled(false);
	}

	// Vanilla
	if (mode.getSelectedValue() == 0) {
		if (g_Data.canUseMoveKeys()) {
			if (jumping && targetStrafe->targetListEmpty)
				effectiveValue += speed;
			if (sneaking)
				effectiveValue -= speed;
		}
		if (input->forwardKey && input->backKey && input->rightKey && input->leftKey) MoveUtil::fullStop(false);
		gm->player->velocity.y = effectiveValue;
		effectiveValue = 0.0f;
	}

	

	

	
}

void ElytraFly::onPlayerTick(C_Player* plr) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
}

void ElytraFly::onMove(C_MoveInputHandler* input) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	bool pressed = MoveUtil::isMoving();
	vec3_t pos = *player->getPos();

	
	

	

	// Vanilla, Jump
	if (mode.getSelectedValue() == 0 || mode.getSelectedValue() == 3)
		MoveUtil::setSpeed(speedx);

	float calcYaw = (player->yaw + 90) * (PI / 180);
	float c = cos(calcYaw);
	float s = sin(calcYaw);

	vec2_t moveVec2D = { input->forwardMovement, -input->sideMovement };
	moveVec2D = { moveVec2D.x * c - moveVec2D.y * s, moveVec2D.x * s + moveVec2D.y * c };
	vec3_t moveVec;

	
}

void ElytraFly::onSendPacket(Packet* packet) {
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	vec3_t pos;
	pos.x = player->getPos()->x;
	pos.y = player->getPos()->y;
	pos.z = player->getPos()->z;
	static int tick = 1; tick++;

	if (packet->isInstanceOf<C_MovePlayerPacket>()) {
		auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
	}

	if (packet->isInstanceOf<NetworkLatencyPacket>()) {
		auto* LatencyPacket = reinterpret_cast<NetworkLatencyPacket*>(packet);
	}
}

void ElytraFly::onDisable() {
	
	g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);
	
}