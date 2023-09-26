#include "HoleFill.h"

HoleFill::HoleFill() : IModule(0x0, Category::COMBAT, "Locate a good place to holecamp ;)") {
	registerEnumSetting("Block", &mode, 0);
	mode.addEntry("Obsidian", 0);
	mode.addEntry("Cobweb", 1);
	registerBoolSetting("Obsidian Hole", &this->obsidian, this->obsidian);
	registerBoolSetting("Bedrock Hole", &this->bedrock, this->bedrock);
	registerBoolSetting("OnClick", &this->onclick, this->onclick);
	registerIntSetting("Range", &this->range, 1, 3.f, 15.f);
}
int place = 1;
HoleFill::~HoleFill() {
}

const char* HoleFill::getModuleName() {
	return ("HoleFill");
}
bool HoleFill::canpla(vec3_ti blockPos) {
	if (g_Data.getLocalPlayer()->getPos()->dist(blockPos.toVector3()) >= 7) {
		return false;
	}
	else return true;
}

static std::vector<C_Entity*> targetList19;
void findEntity19(C_Entity* currentEntity19, bool isRegularEntity) {
	static auto AutoTrapMod = moduleMgr->getModule<AutoTrap>();

	if (currentEntity19 == nullptr)
		return;

	if (currentEntity19 == g_Data.getLocalPlayer())  // Skip Local player
		return;

	if (!g_Data.getLocalPlayer()->canAttack(currentEntity19, false))
		return;

	if (!g_Data.getLocalPlayer()->isAlive())
		return;

	if (!currentEntity19->isAlive())
		return;


	if (!TargetUtil::isValidTarget(currentEntity19))
		return;

	float dist = (*currentEntity19->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
	if (dist < AutoTrapMod->range) {
		targetList19.push_back(currentEntity19);

		float dist = (*currentEntity19->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

		if (dist < AutoTrapMod->range) {
			targetList19.push_back(currentEntity19);
		}
	}
}

bool HoleFill::tryHoleFill(vec3_ti blkPlacement) {
	C_Block* block = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(blkPlacement));
	C_BlockLegacy* blockLegacy = (block->blockLegacy);
	if (blockLegacy->material->isReplaceable) {
		vec3_ti blok(blkPlacement);
		int i = 0;

		static std::vector<vec3_ti*> checklist;
		bool foundCandidate = false;
		if (checklist.empty()) {
			checklist.push_back(new vec3_ti(0, -1, 0));
			checklist.push_back(new vec3_ti(0, 1, 0));

			checklist.push_back(new vec3_ti(0, 0, -1));
			checklist.push_back(new vec3_ti(0, 0, 1));

			checklist.push_back(new vec3_ti(-1, 0, 0));
			checklist.push_back(new vec3_ti(1, 0, 0));
		}

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
void HoleFill::onTick(C_GameMode* gm) {
	if (!(g_Data.getLocalPlayer() == nullptr || !GameData::canUseMoveKeys())) {
		if (g_Data.isInGame()) {
			targetList19.clear();
			g_Data.forEachEntity(findEntity19);
			if (!targetList19.empty()) {
				vec3_t* pos = targetList19[0]->getPos();
				for (int x = (int)pos->x - range; x < pos->x + range; x++) {
					for (int z = (int)pos->z - range; z < pos->z + range; z++) {
						for (int y = (int)pos->y - range; y < pos->y + range; y++) {
							vec4_t underline;
							vec3_t blockPos = vec3_t(x, y, z);
							vec3_t air = vec3_t(x, y + 1, z);
							vec3_t air2 = vec3_t(x, y + 2, z);
							vec3_t blockPos2 = vec3_t(x + 1, y, z);
							vec3_t blockPos3 = vec3_t(x, y, z + 1);
							vec3_t blockPos4 = vec3_t(x, y - 1, z);
							vec3_t blockPos5 = vec3_t(x - 1, y, z);
							vec3_t blockPos6 = vec3_t(x, y, z - 1);
							vec3_t blockPos22 = vec3_t(x + 1, y + 1, z);
							vec3_t blockPos32 = vec3_t(x, y + 1, z + 1);
							vec3_t blockPos52 = vec3_t(x - 1, y + 1, z);
							vec3_t blockPos62 = vec3_t(x, y + 1, z - 1);
							C_Block* block = g_Data.getLocalPlayer()->region->getBlock(blockPos);
							C_Block* airpos = g_Data.getLocalPlayer()->region->getBlock(air);
							C_Block* airpos2 = g_Data.getLocalPlayer()->region->getBlock(air2);
							C_Block* block2 = g_Data.getLocalPlayer()->region->getBlock(blockPos2);
							C_Block* block3 = g_Data.getLocalPlayer()->region->getBlock(blockPos3);
							C_Block* block4 = g_Data.getLocalPlayer()->region->getBlock(blockPos4);
							C_Block* block5 = g_Data.getLocalPlayer()->region->getBlock(blockPos5);
							C_Block* block6 = g_Data.getLocalPlayer()->region->getBlock(blockPos6);
							C_Block* block22 = g_Data.getLocalPlayer()->region->getBlock(blockPos22);
							C_Block* block32 = g_Data.getLocalPlayer()->region->getBlock(blockPos32);
							C_Block* block52 = g_Data.getLocalPlayer()->region->getBlock(blockPos52);
							C_Block* block62 = g_Data.getLocalPlayer()->region->getBlock(blockPos62);
							if (block != nullptr && block2 != nullptr && block3 != nullptr && block4 != nullptr) {
								int blockId = block->toLegacy()->blockId;
								int aircheck = airpos->toLegacy()->blockId;
								int aircheck2 = airpos2->toLegacy()->blockId;
								int blockId2 = block2->toLegacy()->blockId;
								int blockId3 = block3->toLegacy()->blockId;
								int blockId4 = block4->toLegacy()->blockId;
								int blockId5 = block5->toLegacy()->blockId;
								int blockId6 = block6->toLegacy()->blockId;
								int blockId22 = block22->toLegacy()->blockId;
								int blockId32 = block32->toLegacy()->blockId;
								int blockId52 = block52->toLegacy()->blockId;
								int blockId62 = block62->toLegacy()->blockId;
								bool obs = false;
								C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
								C_Inventory* inv = supplies->inventory;
								int slotab = 0;
								if (blockId == 0) obs = true;
								if (bedrock) {
									if ((blockId2 == 7 && blockId3 == 7 && blockId4 == 7 && blockId5 == 7 && blockId6 == 7 && blockId == 0) && (aircheck == 0)) {
										slotab = 0;
										slotab = supplies->selectedHotbarSlot;
										for (int n = 0; n < 9; n++) {
											C_ItemStack* stack = inv->getItemStack(n);
											if (stack->item != nullptr) {
												if (mode.getSelectedValue() == 0) {
													if (stack->getItem()->itemId == 49) {
														supplies->selectedHotbarSlot = n;
														break;
													}
												}
												if (mode.getSelectedValue() == 1) {
													if (stack->getItem()->itemId == 30) {
														supplies->selectedHotbarSlot = n;
														break;
													}
												}
											}
										}
										if (!onclick) {
											place = 0;
										}
										if (onclick) {
											if (GameData::isRightClickDown()) {
												place = 0;
											}
											else {
												place = 1;
											}
										}
										if (place == 0) {
											if (canpla(blockPos)) {
												tryHoleFill(blockPos);
											}
										}
										supplies->selectedHotbarSlot = slotab;
									}
								}
								if (obsidian) {
									if ((blockId2 == 49 || blockId2 == 7) && (blockId3 == 49 || blockId3 == 7) && (blockId4 == 49 || blockId4 == 7) && (blockId5 == 49 || blockId5 == 7) && (blockId6 == 49 || blockId6 == 7) && (blockId == 0) && (aircheck == 0) && !((blockId2 == 7 && blockId3 == 7 && blockId4 == 7 && blockId5 == 7 && blockId6 == 7 && blockId == 0) && (aircheck == 0))) {
										slotab = 0;
										slotab = supplies->selectedHotbarSlot;
										for (int n = 0; n < 9; n++) {
											C_ItemStack* stack = inv->getItemStack(n);
											if (stack->item != nullptr) {
												if (mode.getSelectedValue() == 0) {
													if (stack->getItem()->itemId == 49) {
														supplies->selectedHotbarSlot = n;
														break;
													}
												}
												if (mode.getSelectedValue() == 1) {
													if (stack->getItem()->itemId == 30) {
														supplies->selectedHotbarSlot = n;
														break;
													}
												}
											}
										}
										if (!onclick) {
											place = 0;
										}
										if (onclick) {
											if (GameData::isRightClickDown()) {
												place = 0;
											}
											else {
												place = 1;
											}
										}
										if (place == 0) {
											if (canpla(blockPos)) {
												tryHoleFill(blockPos);
											}
										}
										supplies->selectedHotbarSlot = slotab;;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}