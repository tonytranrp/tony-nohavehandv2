#include "AutoHotbar.h"


using namespace std;
AutoHotbar::AutoHotbar() : IModule(0, Category::PLAYER, "Auto put crystal in hotbar") {
	registerBoolSetting("Sword", &Swordhb, Swordhb);
	registerBoolSetting("Pickaxe", &Pickaxehb, Pickaxehb);
	registerBoolSetting("Obsidian", &obsidianhb, obsidianhb);
	registerBoolSetting("Anvil", &anvilhb, anvilhb);
	registerBoolSetting("Sand", &Sandhb, Sandhb);
	registerBoolSetting("Gravel", &Gravelhb, Gravelhb);
	registerBoolSetting("Powder", &concPowhb, concPowhb);
	registerBoolSetting("Crystal", &crystalhb, crystalhb);
	registerBoolSetting("Gapple", &gapplehb, gapplehb);
	registerIntSetting("Sword slot", &SwordSlot, SwordSlot, 1, 9);
	registerIntSetting("Pickaxe slot", &PickaxeSlot, PickaxeSlot, 1, 9);
	registerIntSetting("Obsidian slot", &obsidianSlot, obsidianSlot, 1, 9);
	registerIntSetting("Anvil slot", &anvilSlot, anvilSlot, 1, 9);
	registerIntSetting("Sand slot", &SandSlot, SandSlot, 1, 9);
	registerIntSetting("Gravel slot", &GravelSlot, GravelSlot, 1, 9);
	registerIntSetting("Powder slot", &concPowSlot, concPowSlot, 1, 9);
	registerIntSetting("Crystal slot", &crystalSlot, crystalSlot, 1, 9);
	registerIntSetting("Gapple slot", &gappleSlot, gappleSlot, 1, 9);
}
const char* AutoHotbar::getModuleName() {
	return "AutoHotbar";
}


void AutoHotbar::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	int realgappleSlot = gappleSlot - 1;
	int realobsidianSlot = obsidianSlot - 1;
	int realcrystalSlot = crystalSlot - 1;
	int realanvilSlot = anvilSlot - 1;
	int realSandSlot = SandSlot - 1;
	int realGravelSlot = GravelSlot - 1;
	int realconcPowSlot = concPowSlot - 1;
	int realswordSlot = SwordSlot - 1;
	int realpickaxeSlot = PickaxeSlot - 1;

	dasword = false;
	dapickaxe = false;
	daob = false;
	daanvil = false;
	daSand = false;
	daGravel = false;
	daconcPow = false;
	dacry = false;
	dagapple = false;


	for (int n = 0; n < 36; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (Swordhb) {
			if (!dasword) {
				C_ItemStack* sworditem = inv->getItemStack(realswordSlot);
				if (sworditem->item != nullptr) {
					if (sworditem->getItem()->itemId != 316) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 316) {
								if (!Nodelay) {
									inv->swapSlots(n, realswordSlot);
								}
								else
								{
									inv->setItem(realswordSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								dasword = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 316) {
							if (!Nodelay) {
								inv->swapSlots(n, realswordSlot);
							}
							else
							{
								inv->setItem(realswordSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							dasword = true;
						}
					}
				}
			}
		}
		if (Pickaxehb) {
			if (!dapickaxe) {
				C_ItemStack* Pickaxeitem = inv->getItemStack(realpickaxeSlot);
				if (Pickaxeitem->item != nullptr) {
					if (Pickaxeitem->getItem()->itemId != 318) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 318) {
								if (!Nodelay) {
									inv->swapSlots(n, realpickaxeSlot);
								}
								else
								{
									inv->setItem(realpickaxeSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								dapickaxe = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 318) {
							if (!Nodelay) {
								inv->swapSlots(n, realpickaxeSlot);
							}
							else
							{
								inv->setItem(realpickaxeSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							dapickaxe = true;
						}
					}
				}
			}
		}
		if (obsidianhb) {
			if (!daob) {
				C_ItemStack* obsidianitem = inv->getItemStack(realobsidianSlot);
				if (obsidianitem->item != nullptr) {
					if (obsidianitem->getItem()->itemId != 49) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 49) {
								if (!Nodelay) {
									inv->swapSlots(n, realobsidianSlot);
								}
								else
								{
									inv->setItem(realobsidianSlot, *stack);
									inv->removeItem(n, stack->count);
								}

								
								daob = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 49) {
							if (!Nodelay) {
								inv->swapSlots(n, realobsidianSlot);
							}
							else
							{
								inv->setItem(realobsidianSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							daob = true;
						}
					}
				}
			}
		}
		if (anvilhb) {
			if (!daanvil) {
				C_ItemStack* anvilitem = inv->getItemStack(realanvilSlot);
				if (anvilitem->item != nullptr) {
					if (anvilitem->getItem()->itemId != 145) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 145) {
								if (!Nodelay) {
									inv->swapSlots(n, realanvilSlot);
								}
								else
								{
									inv->setItem(realanvilSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								daanvil = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 145) {
							if (!Nodelay) {
								inv->swapSlots(n, realanvilSlot);
							}
							else
							{
								inv->setItem(realanvilSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							daanvil = true;
						}
					}
				}
			}
		}
		if (Sandhb) {
			if (!daSand) {
				C_ItemStack* Sanditem = inv->getItemStack(realSandSlot);
				if (Sanditem->item != nullptr) {
					if (Sanditem->getItem()->itemId != 12) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 12) {
								if (!Nodelay) {
									inv->swapSlots(n, realSandSlot);
								}
								else
								{
									inv->setItem(realanvilSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								daSand = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 12) {
							if (!Nodelay) {
								inv->swapSlots(n, realSandSlot);
							}
							else
							{
								inv->setItem(realSandSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							daSand = true;
						}
					}
				}
			}
		}
		if (Gravelhb) {
			if (!daGravel) {
				C_ItemStack* Gravelitem = inv->getItemStack(realGravelSlot);
				if (Gravelitem->item != nullptr) {
					if (Gravelitem->getItem()->itemId != 13) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 13) {
								if (!Nodelay) {
									inv->swapSlots(n, realGravelSlot);
								}
								else
								{
									inv->setItem(realGravelSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								daGravel = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 13) {
							if (!Nodelay) {
								inv->swapSlots(n, realGravelSlot);
							}
							else
							{
								inv->setItem(realGravelSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							daGravel = true;
						}
					}
				}
			}
		}
		if (concPowhb) {
			if (!daconcPow) {
				C_ItemStack* concPowitem = inv->getItemStack(realconcPowSlot);
				if (concPowitem->item != nullptr) {
					if (concPowitem->getItem()->itemId != 252) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 252) {
								if (!Nodelay) {
									inv->swapSlots(n, realconcPowSlot);
								}
								else
								{
									inv->setItem(realconcPowSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								daconcPow = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 252) {
							if (!Nodelay) {
								inv->swapSlots(n, realconcPowSlot);
							}
							else
							{
								inv->setItem(realconcPowSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							daconcPow = true;
						}
					}
				}
			}
		}
		if (crystalhb) {
			if (!dacry) {
				C_ItemStack* crystalitem = inv->getItemStack(realcrystalSlot);
				if (crystalitem->item != nullptr) {
					if (crystalitem->getItem()->itemId != 637) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 637) {
								if (!Nodelay) {
									inv->swapSlots(n, realcrystalSlot);
								}
								else
								{
									inv->setItem(realcrystalSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								dacry = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 637) {
							if (!Nodelay) {
								inv->swapSlots(n, realcrystalSlot);
							}
							else
							{
								inv->setItem(realcrystalSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							dacry = true;
						}
					}
				}
			}
		}
		if (gapplehb) {
			if (!dagapple) {
				C_ItemStack* gappleitem = inv->getItemStack(realgappleSlot);
				if (gappleitem->item != nullptr) {
					if (gappleitem->getItem()->itemId != 259) {
						if (stack->item != nullptr) {
							if (stack->getItem()->itemId == 259) {
								if (!Nodelay) {
									inv->swapSlots(n, realgappleSlot);
								}
								else
								{
									inv->setItem(realgappleSlot, *stack);
									inv->removeItem(n, stack->count);
								}
								dagapple = true;
							}
						}
					}
				}
				else
				{
					if (stack->item != nullptr) {
						if (stack->getItem()->itemId == 259) {
							if (!Nodelay) {
								inv->swapSlots(n, realgappleSlot);
							}
							else
							{
								inv->setItem(realgappleSlot, *stack);
								inv->removeItem(n, stack->count);
							}
							dagapple = true;
						}
					}
				}
			}
		}
	}
}