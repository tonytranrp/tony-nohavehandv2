#include "AutoButton.h"

AutoButton::AutoButton() : IModule(70, Category::PLAYER, "AutoButton By REA, All Button Support") {
	registerBoolSetting("onClick", &onClick, onClick);
}

AutoButton::~AutoButton() {
}
/*/ StartButtonList /*/
/*/		143 /*/ 
/*/		4294967152 /*/ 
/*/		4294967155 /*/
/*/		4294967153 /*/
/*/		4294967156 /*/
/*/		4294967154 /*/
/*/		77 /*/
/*/		4294967036 /*/
/*/		4294967035 /*/
/*/		4294967000 /*/
/*/EndButtonList /*/
const char* AutoButton::getModuleName() {
	return ("AutoButton+");
}
void AutoButton::findButt() {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	for (int n = 0; n < 9; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr) {
			if (stack->getItem()->itemId == 143) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967152) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967155) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967153) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967156) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967154) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 77) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967036) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967035) {
				supplies->selectedHotbarSlot = n;
				break;
			}
			if (stack->getItem()->itemId == 4294967000) {
				supplies->selectedHotbarSlot = n;
				break;
			}

		}
	}
}
bool AutoButton_tryPlace1(vec3_t blkPlacement) {
	blkPlacement = blkPlacement.floor();

	C_Block* block = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(blkPlacement));
	C_BlockLegacy* blockLegacy = block->blockLegacy;
	if (blockLegacy->material->isReplaceable) {
		vec3_ti blok(blkPlacement);
		int i = 0;

		static const std::vector<vec3_ti> checklist = {
			//vec3_ti(1, -1, 0),
			vec3_ti(0, -1, 0),
			vec3_ti(0, 1, 0),
			vec3_ti(0, 0, -1),
			vec3_ti(0, 0, 1),
			vec3_ti(-1, 0, 0),
			vec3_ti(1, 0, 0)};

		bool foundCandidate = false;

		for (const auto& current : checklist) {
			vec3_ti calc = blok.sub(current);
			bool Y = (g_Data.getLocalPlayer()->region->getBlock(calc)->blockLegacy)->material->isReplaceable;
			if (!(g_Data.getLocalPlayer()->region->getBlock(calc)->blockLegacy)->material->isReplaceable) {
				// Found a solid block to click
				foundCandidate = true;
				blok = calc;
				break;
			}
			i++;
		}

		if (foundCandidate) {
			g_Data.getCGameMode()->buildBlock(&blok, i);  // if it breaks, then idk
			return true;
		}
	}

	return false;
}
bool AutoButton_placeBlockBelow1(const vec3_t& blockPos) {
	vec3_t blockPosBelow1(blockPos.x - 0, blockPos.y - 1, blockPos.z - 0);
	vec3_t blockPosBelow(blockPos.x - 0, blockPos.y - 2, blockPos.z - 0);
	if (AutoButton_tryPlace1(blockPosBelow1))
		return true;
	if (AutoButton_tryPlace1(blockPosBelow))
		return true;
	return false;
}
void AutoButton::onTick(C_GameMode* gm) {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	slotab = supplies->selectedHotbarSlot;
	findButt();
	vec3_t playerFeetPos = g_Data.getLocalPlayer()->getPos()->floor();
	vec3_t buttonPos = vec3_t(playerFeetPos.x, playerFeetPos.y - 1, playerFeetPos.z);
	if (onClick) {
		if (GameData::isRightClickDown()) {
			place = 0;
		} else {
			place = 1;
		}
	}

	if (!onClick) {
		place = 0;
	}
	if (place == 0)
		AutoButton_placeBlockBelow1(playerFeetPos);
	if (silentab) {
		supplies->selectedHotbarSlot = slotab;
	}
}