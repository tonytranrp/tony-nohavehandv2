#include "AutoFallBlock.h"
#include "../../ModuleManager.h"

AutoFallBlock::AutoFallBlock() : IModule(0x0, Category::COMBAT, "AutoFallBlock (Height Selector)") {
	registerEnumSetting("Block", &BlockID, 1);
	BlockID.addEntry("Anvil", 1);
	BlockID.addEntry("Sand", 2);
	BlockID.addEntry("Gravel", 3);
	BlockID.addEntry("Powder", 4);
	this->registerIntSetting("Obsidian Height", &this->obsiheight, this->obsiheight, 3, 8);
	this->registerIntSetting("Fall Block Height", &this->fallblockheight, this->fallblockheight, 3, 8);
	this->registerIntSetting("range", &this->range, this->range, 3, 12);
	registerBoolSetting("Multi Place", &this->multiplace, this->multiplace);
	registerBoolSetting("onClick", &this->onClick, this->onClick);
	registerBoolSetting("2b2e", &this->Bypass, this->Bypass);
	
}

AutoFallBlock::~AutoFallBlock() {
}

const char* AutoFallBlock::getModuleName() {
	return "AutoFallBlock+";
}

static std::vector<C_Entity*> targetList17;

void findEntity17(C_Entity* currentEntity17, bool isRegularEntity) {
	static auto AutoFallBlockMod = moduleMgr->getModule<AutoFallBlock>();

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
		targetList17.push_back(currentEntity17);

		float dist = (*currentEntity17->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

		if (dist < AutoFallBlockMod->range) {
			targetList17.push_back(currentEntity17);
		}
	}
}

bool AutoFallBlock::tryAutoFallBlock(vec3_t AutoFallBlock) {
	AutoFallBlock = AutoFallBlock.floor();

	C_Block* block = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(AutoFallBlock));
	C_BlockLegacy* blockLegacy = (block->blockLegacy);
	if (blockLegacy->material->isReplaceable) {
		vec3_ti blok(AutoFallBlock);

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

void AutoFallBlock::onTick(C_GameMode* gm) {
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	if (g_Data.getLocalPlayer() == nullptr)
		return;
	if (!g_Data.canUseMoveKeys())
		return;
	if (obsiheight < fallblockheight) {
		obsiheight = fallblockheight;
	}
	if (Bypass) {
		if (fallblockheight > 7) {
			fallblockheight = 6;
		}
		if (obsiheight > 7) {
			obsiheight = 6;
		}
	}

	

	targetList17.clear();
	g_Data.forEachEntity(findEntity17);

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
	
	if (!targetList17.empty()) {
		vec3_t blockAbove0 = targetList17[0]->eyePos0;
		vec3_t blockAbove1 = targetList17[0]->eyePos0;
		vec3_t blockAbove2 = targetList17[0]->eyePos0;
		vec3_t blockAbove3 = targetList17[0]->eyePos0;
		vec3_t blockAbove4 = targetList17[0]->eyePos0;
		vec3_t blockAbove5 = targetList17[0]->eyePos0;;
		vec3_t blockAboveH0 = targetList17[0]->eyePos0;
		vec3_t blockAboveH1 = targetList17[0]->eyePos0;
		vec3_t blockAboveH2 = targetList17[0]->eyePos0;
		vec3_t blockAboveH3 = targetList17[0]->eyePos0;
		vec3_t blockAboveH4 = targetList17[0]->eyePos0;
		vec3_t blockAboveH5 = targetList17[0]->eyePos0;
		vec3_t blockAboveH6 = targetList17[0]->eyePos0;
		vec3_t blockAboveH7 = targetList17[0]->eyePos0;
		vec3_t blockAboveH8 = targetList17[0]->eyePos0;
		//head 0 
		blockAboveH8.y -= targetList17[0]->height - 0;
		blockAboveH8.x -= targetList17[0]->height - 1;
		blockAboveH8.z -= targetList17[0]->height - 2;
		//head 1
		blockAboveH0.y -= targetList17[0]->height - 1;
		blockAboveH0.x -= targetList17[0]->height - 1;
		blockAboveH0.z -= targetList17[0]->height - 2;
		//head 2
		blockAboveH1.y -= targetList17[0]->height - 2;
		blockAboveH1.x -= targetList17[0]->height - 1;
		blockAboveH1.z -= targetList17[0]->height - 2;
		//head 3
		blockAboveH2.y -= targetList17[0]->height - 3;
		blockAboveH2.x -= targetList17[0]->height - 1;
		blockAboveH2.z -= targetList17[0]->height - 2;
		//head 4
		blockAboveH3.y -= targetList17[0]->height - 4;
		blockAboveH3.x -= targetList17[0]->height - 1;
		blockAboveH3.z -= targetList17[0]->height - 2;
		//head 5
		blockAboveH4.y -= targetList17[0]->height - 5;
		blockAboveH4.x -= targetList17[0]->height - 1;
		blockAboveH4.z -= targetList17[0]->height - 2;
		//head 6
		blockAboveH5.y -= targetList17[0]->height - 6;
		blockAboveH5.x -= targetList17[0]->height - 1;
		blockAboveH5.z -= targetList17[0]->height - 2;
		//head 7
		blockAboveH6.y -= targetList17[0]->height - 7;
		blockAboveH6.x -= targetList17[0]->height - 1;
		blockAboveH6.z -= targetList17[0]->height - 2;
		//head 8
		blockAboveH7.y -= targetList17[0]->height - 8;
		blockAboveH7.x -= targetList17[0]->height - 1;
		blockAboveH7.z -= targetList17[0]->height - 2;
		//Anvil 3
		blockAbove0.y -= targetList17[0]->height - 3;
		//Anvil 4
		blockAbove1.y -= targetList17[0]->height - 4;
		//Anvil 5
		blockAbove2.y -= targetList17[0]->height - 5;
		//Anvil 6
		blockAbove3.y -= targetList17[0]->height - 6;
		//Anvil 7
		blockAbove4.y -= targetList17[0]->height - 7;
		//Anvil 8
		blockAbove5.y -= targetList17[0]->height - 8;

		if (place == 0) {
			// Silent Code
			int slotab = 0;
			C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
			C_Inventory* inv = supplies->inventory;
			slotab = supplies->selectedHotbarSlot;
			for (int n = 0; n < 9; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					if (stack->getItem()->itemId == 49) {
						supplies->selectedHotbarSlot = n;
						break;
					}
				}
			}
			if (obsiheight == 3) {
				if (!tryAutoFallBlock(blockAboveH8)) {
					blockAboveH8.x -= 0;
					blockAboveH8.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH0)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH1)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH2)) {
					blockAboveH2.x -= 0;
					blockAboveH2.z -= 0;
				}
			}
			if (obsiheight == 4) {
				if (!tryAutoFallBlock(blockAboveH8)) {
					blockAboveH8.x -= 0;
					blockAboveH8.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH0)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH1)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH2)) {
					blockAboveH2.x -= 0;
					blockAboveH2.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH3)) {
					blockAboveH3.x -= 0;
					blockAboveH3.z -= 0;
				}
			}
			if (obsiheight == 5) {
				if (!tryAutoFallBlock(blockAboveH8)) {
					blockAboveH8.x -= 0;
					blockAboveH8.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH0)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH1)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH2)) {
					blockAboveH2.x -= 0;
					blockAboveH2.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH3)) {
					blockAboveH3.x -= 0;
					blockAboveH3.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH4)) {
					blockAboveH4.x -= 0;
					blockAboveH4.z -= 0;
				}
			}
			if (obsiheight == 6) {
				if (!tryAutoFallBlock(blockAboveH8)) {
					blockAboveH8.x -= 0;
					blockAboveH8.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH0)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH1)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH2)) {
					blockAboveH2.x -= 0;
					blockAboveH2.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH3)) {
					blockAboveH3.x -= 0;
					blockAboveH3.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH4)) {
					blockAboveH4.x -= 0;
					blockAboveH4.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH5)) {
					blockAboveH5.x -= 0;
					blockAboveH5.z -= 0;
				}
			}
			if (obsiheight == 7) {
				if (!tryAutoFallBlock(blockAboveH8)) {
					blockAboveH8.x -= 0;
					blockAboveH8.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH0)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH1)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH2)) {
					blockAboveH2.x -= 0;
					blockAboveH2.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH3)) {
					blockAboveH3.x -= 0;
					blockAboveH3.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH4)) {
					blockAboveH4.x -= 0;
					blockAboveH4.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH5)) {
					blockAboveH5.x -= 0;
					blockAboveH5.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH6)) {
					blockAboveH6.x -= 0;
					blockAboveH6.z -= 0;
				}
			}
			if (obsiheight == 8) {
				if (!tryAutoFallBlock(blockAboveH8)) {
					blockAboveH8.x -= 0;
					blockAboveH8.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH0)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH1)) {
					blockAboveH1.x -= 0;
					blockAboveH1.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH2)) {
					blockAboveH2.x -= 0;
					blockAboveH2.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH3)) {
					blockAboveH3.x -= 0;
					blockAboveH3.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH4)) {
					blockAboveH4.x -= 0;
					blockAboveH4.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH5)) {
					blockAboveH5.x -= 0;
					blockAboveH5.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH6)) {
					blockAboveH6.x -= 0;
					blockAboveH6.z -= 0;
				}
				if (!tryAutoFallBlock(blockAboveH7)) {
					blockAboveH7.x -= 0;
					blockAboveH7.z -= 0;
				}
			}
			supplies->selectedHotbarSlot = slotab;
			slotab = 0;
			slotab = supplies->selectedHotbarSlot;
			for (int n = 0; n < 9; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					if (BlockID.getSelectedValue() == 1) {
						if (stack->getItem()->itemId == 145) {
							supplies->selectedHotbarSlot = n;
							break;
						}
					}
					if (BlockID.getSelectedValue() == 2) {
						if (stack->getItem()->itemId == 12) {
							supplies->selectedHotbarSlot = n;
							break;
						}
					}
					if (BlockID.getSelectedValue() == 3) {
						if (stack->getItem()->itemId == 13) {
							supplies->selectedHotbarSlot = n;
							break;
						}
					}
					if (BlockID.getSelectedValue() == 4) {
						if (stack->getItem()->itemId == 252) {
							supplies->selectedHotbarSlot = n;
							break;
						}
					}
				}
			}
			if (fallblockheight == 3) {
				if (!tryAutoFallBlock(blockAbove0)) {
					blockAbove0.y -= 0;
				}
			}
			if (fallblockheight == 4) {
				if (!tryAutoFallBlock(blockAbove1)) {
					blockAbove1.y -= 0;
				}
				if (multiplace) {
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
				}
			}
			if (fallblockheight == 5) {
				if (!tryAutoFallBlock(blockAbove2)) {
					blockAbove2.y -= 0;
				}
				if (multiplace) {
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
				}
			}
			if (fallblockheight == 6) {
				if (!tryAutoFallBlock(blockAbove3)) {
					blockAbove3.y -= 0;
				}
				if (multiplace) {
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove2)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove2)) {
						blockAbove1.y -= 0;
					}
				}
			}
			if (fallblockheight == 7) {
				if (!tryAutoFallBlock(blockAbove4)) {
					blockAbove4.y -= 0;
				}
				if (multiplace) {
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove2)) {
						blockAbove2.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove3)) {
						blockAbove2.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove2)) {
						blockAbove2.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove3)) {
						blockAbove2.y -= 0;
					}
				}
			}
			if (fallblockheight == 8) {
				if (!tryAutoFallBlock(blockAbove5)) {
					blockAbove5.y -= 0;
				}
				if (multiplace) {
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove2)) {
						blockAbove2.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove3)) {
						blockAbove3.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove4)) {
						blockAbove4.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove0)) {
						blockAbove0.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove1)) {
						blockAbove1.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove2)) {
						blockAbove2.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove3)) {
						blockAbove3.y -= 0;
					}
					if (!tryAutoFallBlock(blockAbove4)) {
						blockAbove4.y -= 0;
					}
				}
			}
			supplies->selectedHotbarSlot = slotab;
		}
    }
}