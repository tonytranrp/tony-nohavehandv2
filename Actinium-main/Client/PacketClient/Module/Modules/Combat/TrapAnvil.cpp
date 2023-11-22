#include "TrapAnvil.h"

TrapAnvil::TrapAnvil() : IModule(0x0, Category::COMBAT, "Automatically traps the nearest player") {
	registerEnumSetting("Block", &mode, 0);
	mode.addEntry("Obsidian", 0);
	mode.addEntry("tnt", 1);
	registerIntSetting("range", &this->range, this->range, 3, 12);
	registerBoolSetting("onClick", &this->onClick, this->onClick);
}

TrapAnvil::~TrapAnvil() {
}

const char* TrapAnvil::getModuleName() {
	return "TrapAnvil";
}

static std::vector<C_Entity*> targetList152;

void findEntity152(C_Entity* currentEntity15, bool isRegularEntity) {
	static auto AutoTrapMod = moduleMgr->getModule<TrapAnvil>();

	if (currentEntity15 == nullptr)
		return;

	if (currentEntity15 == g_Data.getLocalPlayer())  // Skip Local player
		return;

	if (!g_Data.getLocalPlayer()->canAttack(currentEntity15, false))
		return;

	if (!g_Data.getLocalPlayer()->isAlive())
		return;

	if (!currentEntity15->isAlive())
		return;


	if (!TargetUtil::isValidTarget(currentEntity15))
		return;

	float dist = (*currentEntity15->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
	if (dist < AutoTrapMod->range) {
		targetList152.push_back(currentEntity15);

		float dist = (*currentEntity15->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

		if (dist < AutoTrapMod->range) {
			targetList152.push_back(currentEntity15);
		}
	}
}

bool TrapAnvil::tryAutoTrap123(vec3_t TrapAnvil) {
	TrapAnvil = TrapAnvil.floor();

	C_Block* block = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(TrapAnvil));
	C_BlockLegacy* blockLegacy = (block->blockLegacy);
	if (blockLegacy->material->isReplaceable) {
		vec3_ti blok(TrapAnvil);

		// Find neighbour
		static std::vector<vec3_ti*> checklist;
		if (checklist.empty()) {
			checklist.push_back(new vec3_ti(0, -1, 0));
			checklist.push_back(new vec3_ti(0, 1, 0));

			checklist.push_back(new vec3_ti(0, 0, -1));
			checklist.push_back(new vec3_ti(0, 0, 1));

			checklist.push_back(new vec3_ti(-1, 0, 0));
			checklist.push_back(new vec3_ti(1, 0, 0));
		}

		bool foundCandidate = false;
		int i = 0;
		for (auto current : checklist) {
			vec3_ti calc = blok.sub(*current);
			bool Y = ((g_Data.getLocalPlayer()->region->getBlock(calc)->blockLegacy))->material->isReplaceable;
			if (!((g_Data.getLocalPlayer()->region->getBlock(calc)->blockLegacy))->material->isReplaceable) {
				// Found a solid block to click
				foundCandidate = true;
				blok = calc;
				break;
			}
			i++;
		}
		if (foundCandidate) {
			g_Data.getCGameMode()->buildBlock(&blok, i);

			return true;
		}
	}
	return false;
}

void TrapAnvil::onTick(C_GameMode* gm) {
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	if (g_Data.getLocalPlayer() == nullptr)
		return;
	if (!g_Data.canUseMoveKeys())
		return;



	targetList152.clear();
	g_Data.forEachEntity(findEntity152);

	int place = 1;

	if (onClick) {
		if (GameData::isRightClickDown()) {
			place = 0;
		}
		else {
			place = 1;
		}
	}


	if (!onClick) {
		place = 0;
	}

	if (!targetList152.empty()) {
		
		vec3_t blockAbove = targetList152[0]->eyePos0;
		vec3_t blockAbove1 = targetList152[0]->eyePos0;
		vec3_t blockAbove2 = targetList152[0]->eyePos0;
		vec3_t blockAbove3 = targetList152[0]->eyePos0;
		vec3_t blockAbove4 = targetList152[0]->eyePos0;


		

		//above players head
		blockAbove.y -= targetList152[0]->height - 1 ;

		blockAbove1.y -= targetList152[0]->height - 1;
		blockAbove1.x -= targetList152[0]->height - 1;

		blockAbove2.y -= targetList152[0]->height - 1;
		blockAbove2.x += targetList152[0]->height - 1;

		blockAbove3.y -= targetList152[0]->height - 1;
		blockAbove3.z -= targetList152[0]->height - 1;

		blockAbove4.y -= targetList152[0]->height - 1;
		blockAbove4.z += targetList152[0]->height - 1;

		

		if (place == 0) {
			int slotab = 0;
			C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
			C_Inventory* inv = supplies->inventory;
			slotab = 0;
			slotab = supplies->selectedHotbarSlot;
			for (int n = 0; n < 9; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					if (stack->getItem()->itemId == 145) {
						supplies->selectedHotbarSlot = n;
						break;
					}
				}
			}
			if (!tryAutoTrap123(blockAbove)) {
				blockAbove.y -= 0;
			}
			if (!tryAutoTrap123(blockAbove1)) {
				blockAbove1.y -= 0;
			}
			if (!tryAutoTrap123(blockAbove2)) {
				blockAbove2.y -= 0;
			}

			if (!tryAutoTrap123(blockAbove3)) {
				blockAbove3.y -= 0;
			}
			if (!tryAutoTrap123(blockAbove4)) {
				blockAbove4.y -= 0;
			}
			supplies->selectedHotbarSlot = slotab;
			
			
		}
	}
}