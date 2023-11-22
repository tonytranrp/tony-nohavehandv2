#include "AntiAnvil.h"


AntiAnvil::AntiAnvil() : IModule(0x0, Category::PLAYER, "No FallBlock By tony!") {
}

const char* AntiAnvil::getModuleName() {
    return ("AntiFallBlock+");
}
void AntiAnvil::onEnable() {
    auto player = g_Data.getLocalPlayer();
    if (player == nullptr) return;
}

bool isFallingAnvil(C_Block* block) {
    int blockID = (int)block->toLegacy()->blockId;
    return (blockID == 145);
}
bool isFallingConcretePowder(C_Block* block) {
    int blockID = (int)block->toLegacy()->blockId;
    return (blockID == 252);
}
bool isFallingGravel(C_Block* block) {
    int blockID = (int)block->toLegacy()->blockId;
    return (blockID == 13);
}
bool isFallingSand(C_Block* block) {
    int blockID = (int)block->toLegacy()->blockId;
    return (blockID == 12);
}

bool AntiAnvil::isFallingAnvilAbovePlayer(C_Player* plr) {
    if (plr == nullptr) return false;

    vec3_ti blockpos3 = plr->getPos()->floor();
    blockpos3.y = (int)blockpos3.y - 1;

    // Check for anvils falling above the player's head
    for (int yOffset = 0; yOffset <= (int)height; yOffset++) {  // Use the height value from the slider
        vec3_ti blockposAbove = blockpos3;
        blockposAbove.y += yOffset;

        C_Block* blockAbove = g_Data.getLocalPlayer()->region->getBlock(blockposAbove);
        int blockAboveID = (int)blockAbove->toLegacy()->blockId;

        if (blockAboveID == 145 && isFallingAnvil(blockAbove)) {
            return true;
        }
        if (blockAboveID == 252 && isFallingConcretePowder(blockAbove)) {
            return true;
        }
        if (blockAboveID == 13 && isFallingGravel(blockAbove)) {
            return true;
        }
        if (blockAboveID == 12 && isFallingSand(blockAbove)) {
            return true;
        }
    }

    return false;
}

bool isItemAbovePlayer(C_Player* plr, int itemID) {
    // Get the player's position
    vec3_t playerPos = plr->getPos()->floor();

    // Check the block above the player's head
    vec3_t blockAbovePos = vec3_t(playerPos.x, playerPos.y + 1, playerPos.z);
    C_Block* blockAbovePlayer = g_Data.getLocalPlayer()->region->getBlock(blockAbovePos);

    // Check if the block above has the specified item ID
    if (blockAbovePlayer->toLegacy()->blockId == itemID) {
        return true;  // Item ID detected above the player's head
    }

    return false;  // Item ID not detected
}

bool getObby1() {
    int slotab = 0;
    C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
    C_Inventory* inv = supplies->inventory;
    slotab = supplies->selectedHotbarSlot;
    for (int n = 0; n < 9; n++) {
        C_ItemStack* stack = inv->getItemStack(n);
        if (stack->item != nullptr) {
            if (stack->getItem()->itemId == 77) { // itemid == 146
                supplies->selectedHotbarSlot = n;
                break;
            }
        }
    }

// place code
    supplies->selectedHotbarSlot = slotab;

    return false;  // Button not found
}
void AntiAnvil::findButt() {
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
bool AntiAnvil_tryPlace1(vec3_t blkPlacement) {
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
            vec3_ti(1, 0, 0) };

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
bool AntiAnvil_placeBlockBelow1(const vec3_t& blockPos) {
    vec3_t blockPosBelow1(blockPos.x - 0, blockPos.y - 1, blockPos.z - 0);
    vec3_t blockPosBelow(blockPos.x - 0, blockPos.y - 2, blockPos.z - 0);
    if (AntiAnvil_tryPlace1(blockPosBelow1))
        return true;
    if (AntiAnvil_tryPlace1(blockPosBelow))
        return true;
    return false;
}
void AntiAnvil::onTick(C_GameMode* gm) {
	auto plr = g_Data.getLocalPlayer();
    if (plr == nullptr) return;

    C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
    int slottd2 = supplies->selectedHotbarSlot;

    bool fallingAnvilsDetected = isFallingAnvilAbovePlayer(plr);
    bool item12Detected = isItemAbovePlayer(plr, 12);  // Check if item ID 12 (sand) is detected above the player's head

    // Turn on Phase module if falling anvils or item ID 12 are detected
    auto phaseModule = moduleMgr->getModule<PhaseForAntiAnvil>();

    if (fallingAnvilsDetected || item12Detected) {
        caidelljthe = true;
        supplies->selectedHotbarSlot = slottd2;

        // Check if there is already a button beneath the player's feet
        vec3_t playerFeetPos = plr->getPos()->floor();
        vec3_t buttonPos = vec3_t(playerFeetPos.x, playerFeetPos.y - 1, playerFeetPos.z);
        C_Block* blockBelowPlayer = g_Data.getLocalPlayer()->region->getBlock(buttonPos);

        if (blockBelowPlayer->toLegacy()->blockId != 77) {
            // Button is not present, get obsidian (item ID 77) instead
            bool buttonObtained = getObby1();

            // Place the button immediately under the player's feet if obtained successfully
            if (buttonObtained) {

                // Silent Code
                int slotab = 0;
                C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
                C_Inventory* inv = supplies->inventory;
                slotab = supplies->selectedHotbarSlot;
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 77) { // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                AntiAnvil_placeBlockBelow1(playerFeetPos);
                // place code
                supplies->selectedHotbarSlot = slotab;
                
            }
        }

        // Turn on the Phase module if it's not enabled
        if (phaseModule != nullptr && !phaseModule->isEnabled()) {
            phaseModule->toggle();
        }
    }
    else {
        // No falling anvils or item ID 12 detected, turn off the Phase module if it's enabled
        if (phaseModule != nullptr && phaseModule->isEnabled()) {
            phaseModule->toggle();
        }
    }
}



void AntiAnvil::onPreRender(C_MinecraftUIRenderContext* renderCtx) {
    auto player = g_Data.getLocalPlayer();
    if (player == nullptr) return;

    if (GameData::canUseMoveKeys()) {
        if (!moduleMgr->getModule<ClickGUIMod>()->isEnabled()) {
            if (caidelljthe) {
                DrawUtils::setColor(1.f, 0.f, 0.f, 1.f);
                DrawUtils::drawBox(vec3_t(blockpos3.toVector3().floor()), vec3_t(blockpos3.toFloatVector().floor().add(1.f)), 0.5f, false);
                caidelljthe = false;
            }
        }
    }
}


void AntiAnvil::onDisable() {
    auto player = g_Data.getLocalPlayer();
    if (player == nullptr) return;

    vec3_t pos;
    vec3_t pPos = g_Data.getLocalPlayer()->eyePos0;
    player->aabb.upper.y += 1.8f;
}