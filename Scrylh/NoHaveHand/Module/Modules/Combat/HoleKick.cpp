#include "HoleKick.h"
#include "../../ModuleManager.h"

HoleKick::HoleKick() : IModule(0x0, Category::COMBAT, "HoleKick") {
	
	
	this->registerIntSetting("range", &this->range, this->range, 3, 12);
}

HoleKick::~HoleKick() {
}

const char* HoleKick::getModuleName() {
	return "HoleKick";
}

static std::vector<C_Entity*> targetList172;

void findEntity172(C_Entity* currentEntity17, bool isRegularEntity) {
	static auto AutoFallBlockMod = moduleMgr->getModule<HoleKick>();

	if (currentEntity17 == nullptr)
		return;

	if (currentEntity17 == g_Data.getLocalPlayer())  // Skip Local player
		return;

	if (!g_Data.getLocalPlayer()->canAttack(currentEntity17, false))
		return;

	if (!g_Data.getLocalPlayer()->isAlive())
		return;

	if (!currentEntity17->isAlive())
		return;


	if (!TargetUtil::isValidTarget(currentEntity17))
		return;

	float dist = (*currentEntity17->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
	if (dist < AutoFallBlockMod->range) {
		targetList172.push_back(currentEntity17);

		float dist = (*currentEntity17->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

		if (dist < AutoFallBlockMod->range) {
			targetList172.push_back(currentEntity17);
		}
	}
}

bool HoleKick::tryAutoFallBlock1(vec3_t HoleKick) {
	HoleKick = HoleKick.floor();

	C_Block* block = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(HoleKick));
	C_BlockLegacy* blockLegacy = (block->blockLegacy);
	if (blockLegacy->material->isReplaceable) {
		vec3_ti blok(HoleKick);

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

void HoleKick::onTick(C_GameMode* gm) {
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	if (g_Data.getLocalPlayer() == nullptr)
		return;
	if (!g_Data.canUseMoveKeys())
		return;
	



	targetList172.clear();
	g_Data.forEachEntity(findEntity172);

	int place = 1;

	


	

	if (!targetList172.empty()) {
		vec3_t blockAboveH0 = targetList172[0]->eyePos0;
		vec3_t blockAboveH1 = targetList172[0]->eyePos0;
		vec3_t blockAboveH2 = targetList172[0]->eyePos0;


		//head 1
		blockAboveH0.y -= targetList172[0]->height - 1;
		blockAboveH0.x -= targetList172[0]->height - 2;
		blockAboveH0.z -= targetList172[0]->height - 0;
		//head 2
		blockAboveH1.y -= targetList172[0]->height - 2;
		blockAboveH1.x -= targetList172[0]->height - 2;
		blockAboveH1.z -= targetList172[0]->height - 0;
		//head 3 
		blockAboveH2.y -= targetList172[0]->height - 2;
		blockAboveH2.x -= targetList172[0]->height - 1;
		blockAboveH2.z -= targetList172[0]->height - 0;


		if (place == 0) {
			// Silent Code
			int slotab = 0;
			C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
			C_Inventory* inv = supplies->inventory;
			slotab = supplies->selectedHotbarSlot;
			for (int n = 0; n < 9; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					if (stack->getItem()->itemId == 152) {
						supplies->selectedHotbarSlot = n;
						break;
					}
				}
			}



			if (!tryAutoFallBlock1(blockAboveH0)) {
				blockAboveH0.x -= 0;
				blockAboveH0.z -= 0;
			}
			if (!tryAutoFallBlock1(blockAboveH1)) {
				blockAboveH1.x -= 0;
				blockAboveH1.z -= 0;
			}


			supplies->selectedHotbarSlot = slotab;
			slotab = 0;
			slotab = supplies->selectedHotbarSlot;
			for (int n = 0; n < 9; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					if (stack->getItem()->itemId == 33) {
						supplies->selectedHotbarSlot = n;
							break;
					}
				}
			}

			if (!tryAutoFallBlock1(blockAboveH2)) {
				blockAboveH2.x -= 0;
				blockAboveH2.z -= 0;
			}





			supplies->selectedHotbarSlot = slotab;
		}
	}
}