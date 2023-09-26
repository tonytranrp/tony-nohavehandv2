#include "Killaura.h"
#include "../pch.h"

using namespace std;
Killaura::Killaura() : IModule(0, Category::COMBAT, "Attacks entities") {
	registerEnumSetting("Rotations", &rotations, 0);
	rotations.addEntry("None", 0);
	rotations.addEntry("Silent", 1);
	rotations.addEntry("Client", 2);
	rotations.addEntry("Actual", 3);
	registerEnumSetting("Mode", &mode, 0);
	mode.addEntry("Single", 0);
	mode.addEntry("Multi", 1);
	registerEnumSetting("AutoWeapon", &AutoWeapon, 0);
	AutoWeapon.addEntry("None", 0);
	AutoWeapon.addEntry("Full", 1);
	AutoWeapon.addEntry("Silent", 2);
	registerBoolSetting("MobAura", &mobs, mobs);
	registerBoolSetting("Visualize", &visualize, visualize);
	registerBoolSetting("Hold", &hold, hold);
	registerFloatSetting("Range", &range, range, 3.f, 20.f);
	registerIntSetting("Attack delay", &delayka, delayka, 0, 20);
	registerBoolSetting("Swing", &swingKA, swingKA);
	registerBoolSetting("hurttime", &hurttimeka, hurttimeka);
}

void Killaura::findWeapon() {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	float damage = 0;
	int slot = supplies->selectedHotbarSlot;
	for (int n = 0; n < 9; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr) {
			float currentDamage = stack->getAttackingDamageWithEnchants();
			if (currentDamage > damage) {
				damage = currentDamage;
				slot = n;
			}
		}
	}
	supplies->selectedHotbarSlot = slot;
}

const char* Killaura::getRawModuleName() {
	return "Killaura";
}

const char* Killaura::getModuleName() {
	switch (mode.getSelectedValue()) {
	case 0: name = string("Killaura ") + string(GRAY) + string("[") + string(WHITE) + string("Single") + string(GRAY) + string("]"); break;
	case 1: name = string("Killaura ") + string(GRAY) + string("[") + string(WHITE) + string("Multi") + string(GRAY) + string("]"); break;
	}
	return name.c_str();
}

struct CompareTargetEnArray {
	bool operator()(C_Entity* lhs, C_Entity* rhs) {
		C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
		return (*lhs->getPos()).dist(*localPlayer->getPos()) < (*rhs->getPos()).dist(*localPlayer->getPos());
	}
};

static vector<C_Entity*> targetList;
void findEntity(C_Entity* currentEntity, bool isRegularEntity) {
	static auto killaura = moduleMgr->getModule<Killaura>();

	if (currentEntity == nullptr) return;
	if (currentEntity == g_Data.getLocalPlayer()) return;
	if (!g_Data.getLocalPlayer()->canAttack(currentEntity, false)) return;
	if (!g_Data.getLocalPlayer()->isAlive()) return;
	if (!currentEntity->isAlive()) return;
	if (currentEntity->getEntityTypeId() == 66) // falling block
		return;
	if (currentEntity->getEntityTypeId() == 69)  // XP
		return;
	if (currentEntity->getEntityTypeId() == 64) // item
		return;
	if (currentEntity->getEntityTypeId() == 80 || currentEntity->getEntityTypeId() == 69) return;

	if (killaura->mobs) {
		if (currentEntity->getNameTag()->getTextLength() <= 1 && currentEntity->getEntityTypeId() == 319) return;
		if (currentEntity->width <= 0.01f || currentEntity->height <= 0.01f) return;
		//if (currentEntity->getEntityTypeId() == 64) return;
		if (currentEntity->getEntityTypeId() == 0xC00050)  // Arrows - new item id
			return;
		if (currentEntity->getEntityTypeId() == 307) // NPC
			return;
	}
	else {
		if (!TargetUtil::isValidTarget(currentEntity)) return;
		if (currentEntity->getEntityTypeId() == 51 || currentEntity->getEntityTypeId() == 1677999) return;
	}

	float dist = (*currentEntity->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
	if (dist < killaura->range) {
		targetList.push_back(currentEntity);
		sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
	}
}

void Killaura::onEnable() {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	targetListEmpty = true;
	targetList.clear();
}

void Killaura::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	int dbrr = g_Data.getLocalPlayer()->getSelectedItemId();
	if (player == nullptr) return;

	PointingStruct* pointing = g_Data.getLocalPlayer()->pointingStruct;
	//auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	//if (clickGUI->isEnabled()) targetListEmpty = true;
	targetListEmpty = targetList.empty();

	targetList.clear();

	g_Data.forEachEntity(findEntity);

	Odelay++;
	slotKA = supplies->selectedHotbarSlot;
	if (hold && !g_Data.isLeftClickDown())
		return;
	//if (wtfdb && (dbrr == 259 || dbrr == 426) && g_Data.isRightClickDown()) return;
	if (AutoWeapon.getSelectedValue() != 0 && !targetList.empty()) findWeapon();
	if (!targetList.empty() && Odelay >= delayka) {
		if (mode.getSelectedValue() == 0) {
			for (auto& i : targetList) {
				if (!(i->damageTime > 1 && hurttimeka)) {
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					gm->attack(targetList[0]);
					if (swingKA) {
						player->swing();
					}
				}
			}
		}
		if (mode.getSelectedValue() == 1) {
			for (auto& i2 : targetList) {
				if (!(i2->damageTime > 1 && hurttimeka)) {
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					gm->attack(i2);
					if (swingKA) {
					player->swing();
					}
				}
			}
		}
		if (AutoWeapon.getSelectedValue() == 2) supplies->selectedHotbarSlot = slotKA;
		if (rotations.getSelectedValue() == 3) {
			tro = g_Data.getLocalPlayer()->getPos()->CalcAngle(*targetList[0]->getPos());
		}
		Odelay = 0;
	}
	else targetListEmpty = true;
	//gm->destroyBlock(&vec3_ti(277070, 69, -254355), 1);
}

void Killaura::onPlayerTick(C_Player* plr) {
	if (plr == nullptr) return;

	if (!targetList.empty()) {
		if (hold && !g_Data.isLeftClickDown())
			return;

		for (auto& i : targetList) {
			vec2_t angle = g_Data.getLocalPlayer()->getPos()->CalcAngle(*targetList[0]->getPos());
			vec2_t pos = g_Data.getLocalPlayer()->getPos()->CalcAngle(*i->getPos());

			if (rotations.getSelectedValue() == 2) {
				plr->yawUnused1 = angle.y;
				plr->pitch = angle.x;
				plr->bodyYaw = angle.y;
			}

			// Strafe
			if (rotations.getSelectedValue() == 3) {
				plr->setRot(tro);
				plr->yawUnused1 = angle.y;
			}
		}
	}
}

void Killaura::onSendPacket(Packet* packet) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	if (!targetList.empty()) {
		if (hold && !g_Data.isLeftClickDown())
			return;

		if (packet->isInstanceOf<C_MovePlayerPacket>() && (rotations.getSelectedValue() == 1 || rotations.getSelectedValue() == 2) && !targetList.empty()) {
			vec2_t anglefd = g_Data.getLocalPlayer()->getPos()->CalcAngle(targetList[0]->eyePos0);
			auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);

			movePacket->pitch = anglefd.x;
			movePacket->headYaw = anglefd.y;
			movePacket->yaw = anglefd.y;
		}
	}
	/*
	if (packet->isInstanceOf<C_TextPacket>()) {
		auto* textPacket = reinterpret_cast<C_TextPacket*>(packet);
		textPacket->message = textPacket->message.getText() + std::string("| Test");
	}
	*/
}

void Killaura::onLevelRender() {
	auto targetStrafe = moduleMgr->getModule<TargetStrafe>();
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr || targetStrafe->isEnabled() || !visualize) return;

	static float n = 0;
	static float anim = 0;
	if (g_Data.canUseMoveKeys() && !targetList.empty()) {
		if (hold && !g_Data.isLeftClickDown())
			return;
		if (!targetList.empty()) {
			anim += 2; // Increase animation speed
			DrawUtils::setColor(0, 0, 1, 0.9f); // Blue color for the bottom part of the circle

			int numCircles = 5; // Number of circles to draw
			float circleSpacing = 0.05f; // Spacing between circles
			float baseRadius = 0.4f; // Base radius of the circle

			vector<vec3_t> posList; // Declare posList outside the loop
			posList.reserve(56 * numCircles);

			for (int j = 0; j < numCircles; j++) {
				vec3_t permutations[56];
				for (int i = 0; i < 56; i++) {
					float angle = (i * 10.f + anim) * (PI / 180);
					float radius = baseRadius + j * circleSpacing;
					permutations[i] = { sinf(angle) * radius, -0.4f * sinf((anim / 15.f) + (i * 0.15f)), cosf(angle) * radius };
				}

				vec3_t* start = targetList[0]->getPosOld();
				vec3_t* end = targetList[0]->getPos();
				auto te = DrawUtils::getLerpTime();
				vec3_t pos = start->lerp(end, te);

				for (auto& perm : permutations) {
					vec3_t curPos(pos.x, pos.y + perm.y, pos.z);
					posList.push_back(curPos.add(perm));
				}

				// Draw the circles with increasing radii
				DrawUtils::setColor(0, 0, 1, 0.9f);
				DrawUtils::drawLineStrip3D(posList, 5);
			}

			// Draw the top part of the cylinder in fading red
			DrawUtils::setColor(1, 0, 0, 0.2f);
			DrawUtils::drawLineStrip3D(posList, 5);
		}
	}
}

void Killaura::onDisable() {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	targetListEmpty = true;
	targetList.clear();
}