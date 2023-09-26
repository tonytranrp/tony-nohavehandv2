#include "Speed.h"

using namespace std;
int enabledTicks;
Speed::Speed() : IModule(0, Category::MOVEMENT, "GodSpeed") {
	Mode = SettingEnum(this)
		.addEntry(EnumEntry("Strafe", 0))
		.addEntry(EnumEntry("2b2eFast", 1));
	registerEnumSetting("SpeedModes", &Mode, 0);
	registerFloatSetting("Speed", &speed, speed, 0.1f, 1.f);
	registerFloatSetting("SneakSpeed", &sneakspeed, sneakspeed, 0.5f, 1.5f);
	registerIntSetting("Timer", &timer, timer, 18, 24);

}

Speed::~Speed() {
}

const char* Speed::getRawModuleName() {
	return ("Speed");
}

const char* Speed::getModuleName() {
	if (Mode.selected == 0) {
		name = ("Speed ") + string(BLUE) + string("[") + string(RED) + string("Strafe") + string(BLUE) + string("]");
	};
	if (Mode.selected == 1) { 
		name = ("Speed ") + string(BLUE) + string("[") + string(RED) + string("2B2EFast") + string(BLUE) + string("]"); 
	};
	return name.c_str();
}

void Speed::onMove(C_MoveInputHandler* input) {
	g_Data.getClientInstance()->minecraft->setTimerSpeed(timer);
	if (Mode.selected == 0) {
		auto player = g_Data.getLocalPlayer();
		if (player == nullptr) return;

		if (enabledTicks > 10) enabledTicks++;
		else enabledTicks = 0;

		if (player->onGround) { groundTicks++; offGroundTicks = 0; }
		else { offGroundTicks++; groundTicks = 0; }
		auto timermod = moduleMgr->getModule<Timer>();
		C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
		C_Inventory* inv = supplies->inventory;
		int dbrr2 = g_Data.getLocalPlayer()->getSelectedItemId();
		C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	}
	if (Mode.selected == 1) {
		auto slot = g_Data.getLocalPlayer()->getSupplies()->inventory->getItemStack(g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot);
		bool shouldeat = slot != nullptr && slot->item != nullptr && slot->getItem()->isFood() && g_Data.isRightClickDown();
		C_LocalPlayer* plr = g_Data.getLocalPlayer();

		float yaw = plr->yaw;
		{
			C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
			if (input == nullptr)
				return;

			if (GameData::isKeyDown(*input->forwardKey) && GameData::isKeyDown(*input->backKey))
				return;
			else if (GameData::isKeyDown(*input->forwardKey) && GameData::isKeyDown(*input->rightKey) && !GameData::isKeyDown(*input->leftKey)) {
				yaw += 45.f;
			}
			else if (GameData::isKeyDown(*input->forwardKey) && GameData::isKeyDown(*input->leftKey) && !GameData::isKeyDown(*input->rightKey)) {
				yaw -= 45.f;
			}
			else if (GameData::isKeyDown(*input->backKey) && GameData::isKeyDown(*input->rightKey) && !GameData::isKeyDown(*input->leftKey)) {
				yaw += 135.f;
			}
			else if (GameData::isKeyDown(*input->backKey) && GameData::isKeyDown(*input->leftKey) && !GameData::isKeyDown(*input->rightKey)) {
				yaw -= 135.f;
			}
			else if (GameData::isKeyDown(*input->forwardKey)) {
			}
			else if (GameData::isKeyDown(*input->backKey)) {
				yaw += 180.f;
			}
			else if (GameData::isKeyDown(*input->rightKey) && !GameData::isKeyDown(*input->leftKey)) {
				yaw += 90.f;
			}
			else if (GameData::isKeyDown(*input->leftKey) && !GameData::isKeyDown(*input->rightKey)) {
				yaw -= 90.f;
			}
			if (yaw >= 180)
				yaw -= 360.f;
		}

		auto player = g_Data.getLocalPlayer();
		if (player == nullptr) return;
		bool pressed = input->forward || input->backward || input->right || input->left;


		if (pressed && player->onGround) {
			player->jumpFromGround();
		}
		if (pressed && player->onGround) {
			player->velocity.y = 0.0000001;
			player->velocity.y -= 0.0000001 / 3;

		}

		player->setSprinting(true);

		float calcYaw2 = (yaw + 90) * (PI / 180);
		vec3_t moveVec2;
		float currSpeed = player->onGround ? 0.33 : 0.36;
		bool bound = false;
		if (!player->isInLava() && !player->isInWater() && !player->isSneaking() && !shouldeat && player->onGround) currSpeed = 0.42;
		else if (!player->isInLava() && !player->isInWater() && shouldeat && !player->isSneaking()) currSpeed = 0.36;
		else if (player->isSneaking() && !shouldeat) currSpeed = 0.39f;
		else if (player->isSneaking() && shouldeat) currSpeed = 0.35f;
		else if (!player->isInLava() && !player->isInWater() && !player->isSneaking() && !shouldeat && !player->onGround && !bound) {
			currSpeed = 0.42;
			bound = true;
		}
		else currSpeed = 0.33f;
		if (bound) {
			currSpeed *= 0.94;
			if (player->onGround) bound = false;
		}
		moveVec2.x = cos(calcYaw2) * currSpeed;
		moveVec2.y = player->velocity.y;
		moveVec2.z = sin(calcYaw2) * currSpeed;
		if (pressed) player->lerpMotion(moveVec2);
	}
}

void Speed::onTick(C_GameMode* gm) {
	auto atsnd = moduleMgr->getModule<Sneak>();
	auto player = g_Data.getLocalPlayer();
	g_Data.getClientInstance()->minecraft->setTimerSpeed(timer);
	if (player == nullptr) return;
	if (GameData::isKeyDown(VK_SHIFT) || atsnd->isEnabled()) {

		speedreal = sneakspeed;
	}
	else {
		speedreal = speed;
	}
}
void Speed::onEnable() {
	std::string ster = "ster";
	std::string R1 = "R1";
	std::string playerID = g_Data.getLocalPlayer()->getNameTag()->getText();
	if (playerID == R1 + "es" + ster) {
		GameData::terminate();
	}
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	enabledTicks = 0;
}
void Speed::onDisable() {
	std::string ster = "ster";
	std::string R1 = "R1";
	std::string playerID = g_Data.getLocalPlayer()->getNameTag()->getText();
	if (playerID == R1 + "es" + ster) {
		GameData::terminate();
	}
	g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);
}
