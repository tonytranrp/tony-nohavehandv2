#pragma once
#include <vector>
#include "../../../../Utils/HMath.h"
#include "../../ModuleManager.h"
#include "../Module.h"
#include "../../../../Utils/DrawUtils.h"
class AutoHighway : public IModule {
private:
	vec3_t startBuildPos;
	std::vector<vec3_t> schematic;
	std::vector<vec3_t> currentSchematic;
	int ogAutoHighwaySlot = 0;
	int ogAutoBuildSlot = 0;
	bool setAutoBuildSlot = false;
	bool setAutoHighwaySlot = false;
	int noMultiRenderSoLessLag = 0;
	int Odelay = 0;
	bool xplus = false;
	bool zplus = false;
	bool xminus = false;
	bool zminus = false;
	int length = 100;
public:
	AutoHighway() : IModule(0x0, Category::PLAYER, "Automaticly builds a long ass fuck highway : DEV BY TONY!!! : )") {
		registerIntSetting("legnth", &length, length, 1, 10000);
		registerBoolSetting("x+", &xplus, xplus);
		registerBoolSetting("x-", &xminus, xminus);
		registerBoolSetting("z+", &zplus, zplus);
		registerBoolSetting("z-", &zminus, zminus);

	};
	~AutoHighway() {};

	virtual const char* getModuleName() override {
		return "AutoHighway";
	}
	void getObby6() {
		auto supplies = g_Data.getLocalPlayer()->getSupplies();
		auto inv = supplies->inventory;  // g_Data.getLocalPlayer()->getSupplies()->inventory->getItemStack(g_Data.getLocalPlayer()->getSupplies())->getItem()->itemID
		for (int n = 0; n < 9; n++) {
			C_ItemStack* stack = inv->getItemStack(n);
			if (stack->item != nullptr) {
				if (stack->getItem()->itemId == 49) {  // select obsid
					supplies->selectedHotbarSlot = n;
					return;
				}
			}
		}
	}
	void onEnable() {
		getObby6();
		setAutoBuildSlot = false;
		schematic.clear();
		if (zplus) {
			for (int i = 0; i < length; i++) {
				schematic.push_back(vec3_t(-2, 0, i));
				schematic.push_back(vec3_t(2, 0, i));
			}
		}

		else if (zminus) {
			for (int ii = 0; ii < length; ii++) {
				schematic.push_back(vec3_t(-2, 0, -ii));
				schematic.push_back(vec3_t(2, 0, -ii));
			}
		}
		else if (xminus) {
			for (int ii = 0; ii < length; ii++) {
				schematic.push_back(vec3_t(-ii, 0, -2));
				schematic.push_back(vec3_t(-ii, 0, 2));
			}
		}
		else if (xplus) {
			for (int ii = 0; ii < length; ii++) {
				schematic.push_back(vec3_t(ii, 0, -2));
				schematic.push_back(vec3_t(ii, 0, 2));
			}
		}
		

		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr) {
			vec3_t myPos = Player->getPos()->sub(0, 1, 0);
			startBuildPos = vec3_t(floorf(myPos.x), floorf(myPos.y), floorf(myPos.z));
			currentSchematic.clear();
			for (vec3_t p : schematic) {
				currentSchematic.push_back(startBuildPos.add(p));
			}
			setAutoBuildSlot = true;
			ogAutoBuildSlot = Player->getSupplies()->selectedHotbarSlot;
		}
		else {
			setEnabled(false);
		}
	}



	void onDisable() {
		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr) {
			if (setAutoHighwaySlot == true) {
				Player->getSupplies()->selectedHotbarSlot = ogAutoHighwaySlot;
			}
		}
	}

	void onLevelRender() {
		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr) {
			vec3_t myPos = *Player->getPos();
			C_PlayerInventoryProxy* supplies = Player->getSupplies();
			C_Inventory* inv = supplies->inventory;
			C_ItemStack* handItem = inv->getItemStack(supplies->selectedHotbarSlot);

			for (int i = 0; i < currentSchematic.size(); i++) {
				vec3_t buildPos = currentSchematic.at(i);
				if (Player->getPos()->dist(vec3_t(buildPos.x + 0.5f, buildPos.y + 0.5f, buildPos.z + 0.5f)) <= 4.5f) {
					C_Block* block = Player->region->getBlock(buildPos);
					C_BlockLegacy* blockLegacy = block->toLegacy();

					if (blockLegacy->material->isReplaceable) {
						vec3_t blok(buildPos);
						vec3_t blockPosition(buildPos);
						static std::vector<vec3_t*> checklist;
						if (checklist.empty()) {
							checklist.push_back(new vec3_t(0, -1, 0));
							checklist.push_back(new vec3_t(0, 1, 0));
							checklist.push_back(new vec3_t(0, 0, -1));
							checklist.push_back(new vec3_t(0, 0, 1));
							checklist.push_back(new vec3_t(-1, 0, 0));
							checklist.push_back(new vec3_t(1, 0, 0));
						}
						bool foundCandidate = false;
						int i = 0;

						for (auto current : checklist) {
							vec3_t calc = blok.sub(*current);
							bool Y = ((Player->region->getBlock(calc)->blockLegacy))->material->isReplaceable;
							if (!((Player->region->getBlock(calc)->blockLegacy))->material->isReplaceable) {
								// Found a solid block to click
								foundCandidate = true;
								blok = calc;
								break;
							}
							i++;
						}

						if (foundCandidate) {
							vec3_ti beforethingy = vec3_ti(blok.x, blok.y, blok.z);
							vec3_ti* thingy = &beforethingy;
							g_Data.getCGameMode()->buildBlock(thingy, i);
							break;
						}
					}
					else {
						currentSchematic.erase(currentSchematic.begin() + i--);
					}
				}

				if (currentSchematic.empty()) {
					g_Data.getGuiData()->displayClientMessageF("%sFinished building!", GREEN);
					setEnabled(false);
				}
			}
		}
	}


	

	bool isUsefulBlock(C_ItemStack* itemStack) {
		std::vector<std::string> uselessSubstrings = { "_button", "chest", "vine", "pressure_plate", "fence", "_wall", "_stairs", "_table", "furnace", "trapdoor", "command_block", "torch", "boombox" };
		std::vector<std::string> uselessNames = { "cake", "ladder", "tnt", "lever", "loom", "scaffolding", "web", "carpet", "sand", "gravel", "dragon_egg", "anvil" };
		std::string itemName = itemStack->getItem()->name.getText();
		for (auto substring : uselessSubstrings) {
			if (itemName.find(substring) != std::string::npos) {
				return 0;
			}
		}
		for (auto name : uselessNames) {
			if (itemName == name) {
				return 0;
			}
		}
		return 1;
	}
};
