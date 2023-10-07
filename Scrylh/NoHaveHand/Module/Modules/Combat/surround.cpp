#include <chrono>
#include <cmath>  // Include the <cmath> library for fading calculations
#include <thread>

#include "Surround.h"

//#include "../../DrawUtils.h"

Surround::Surround() : IModule(0, Category::COMBAT, "Protect yourself from crystals :):  dev by tony") {
    enum1 = SettingEnum(this).addEntry(EnumEntry("Anti-City", 1));

    registerBoolSetting("City-Switch", &this->citySwitch, true);
    registerBoolSetting("Show placements", &this->renderSurround, true);
    registerBoolSetting("AirPlace", &this->doAirplace, false);
    registerBoolSetting("Center", &this->center, this->center);
    registerBoolSetting("buttonplace", &this->buttonplace, this->buttonplace);
    registerBoolSetting("selftrap", &selftrap, selftrap);
    registerEnumSetting("Button type", &buttonidchoser, 0);
    buttonidchoser = SettingEnum(this).addEntry(EnumEntry("stone button", 0)).addEntry(EnumEntry("oak wood button", 1)).addEntry(EnumEntry("birch button", 2)).addEntry(EnumEntry("spuce button", 3));
}

Surround::~Surround() {}

const char* Surround::getModuleName() { return "Surround"; }
/*
bool Surround::isHoldMode() {
        return false;
}
*/
int slot = 0;
void getObby() {
    C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
    C_Inventory* inv = supplies->inventory;
    auto prevSlot = supplies->selectedHotbarSlot;
    for (int n = 0; n < 9; n++) {
        C_ItemStack* stack = inv->getItemStack(n);
        if (stack->item != nullptr) {
            if (stack->getItem()->itemId == 49) {
                if (prevSlot != n)
                    supplies->selectedHotbarSlot = n;
                break;
            }
        }
    }
}

bool Surround::isSideEmpty() {
    bool sendBack = false;

    for (auto checkThis : sidesBlocksOnly) {
        auto playerOn = g_Data.getLocalPlayer()->getPos();
        auto rndOffPlayerOn = playerOn->floor();
        rndOffPlayerOn.y -= 1.f;

        auto calcSides = rndOffPlayerOn.add(checkThis);
        if (g_Data.getLocalPlayer()->region->getBlock(vec3_ti(calcSides))->toLegacy()->blockId == 0) {
            sendBack = true;
        }
    }
    if (sendBack == true) {
        return true;
    }
    else {
        return false;
    }
}

void Surround::onMove(C_MoveInputHandler* hand) {
    if (g_Data.getLocalPlayer() == nullptr) return;

    if (jumpDisable && hand->isJumping) this->setEnabled(false);
}

bool tryPlace(vec3_t blkPlacement) {
    blkPlacement = blkPlacement.floor();

    C_Block* block = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(blkPlacement));
    C_BlockLegacy* blockLegacy = block->blockLegacy;
    if (blockLegacy->material->isReplaceable) {
        vec3_ti blok(blkPlacement);
        int i = 0;

        static const std::vector<vec3_ti> checklist = {// vec3_ti(1, -1, 0),
                                                       vec3_ti(0, -1, 0), vec3_ti(0, 1, 0), vec3_ti(0, 0, -1), vec3_ti(0, 0, 1), vec3_ti(-1, 0, 0), vec3_ti(1, 0, 0) };

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
            // Check if there is a button below the player's feet


            // Call buildBlock directly to place the block silently
            g_Data.getCGameMode()->buildBlock(&blok, i);
            return true;
        }
    }

    return false;
}

void Surround::onEnable() {
    C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
    slot = supplies->selectedHotbarSlot;
    if (g_Data.getLocalPlayer() == nullptr) return;
    origSlot = g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot;
    if (!citySwitch) supplies->selectedHotbarSlot = slot;
}

bool doReset = true;
std::vector<vec3_t> surrArr;
bool stopSwitch = false;
void placeBlockAbove(const vec3_t& blockPos) {
    vec3_t blockPosAbove1 = blockPos.add(vec3_t(0, 2, 0));
    vec3_t blockPosAbove = blockPos.add(vec3_t(0, 1, 0));
    tryPlace(blockPosAbove1);
    tryPlace(blockPosAbove);
}


void Surround::onTick(C_GameMode* gm) {

    if (g_Data.getLocalPlayer() == nullptr) return;

    pn = false;

    if (citySwitch && stopSwitch && g_Data.getLocalPlayer()->getSelectedItemId() != 49) {  // make sure that YOU actually have an obsidian in your hand before placing -> allows you to multitask eat, crystal, mining etc..
        getObby();
        return;
    }

    if (citySwitch && isSideEmpty() && !stopSwitch) {  // auto grab blocks when side empty/broken
        origSlot = g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot;

        if (citySwitch) {
            // Silent Code
            int slotab = 0;
            C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
            C_Inventory* inv = supplies->inventory;
            slotab = supplies->selectedHotbarSlot;
            vec3_t playerPos = *g_Data.getLocalPlayer()->getPos();
            if (selftrap) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 49) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                // place code

                vec3_t targetPos1 = playerPos.add(vec3_t(1, -1, 0));
                vec3_t targetPos2 = playerPos.add(vec3_t(1, 0, 0));
                vec3_t targetPos3 = playerPos.add(vec3_t(0, 0, 0));
                vec3_t targetPos4 = playerPos.add(vec3_t(-1, 0, 0));

                placeBlockAbove(targetPos1);
                placeBlockAbove(targetPos2);
                placeBlockAbove(targetPos3);
                placeBlockAbove(targetPos4);
                supplies->selectedHotbarSlot = slotab;
            }

            if (buttonplace) {
                if (buttonidchoser.selected == 0) {
                    for (int n = 0; n < 9; n++) {
                        C_ItemStack* stack = inv->getItemStack(n);
                        if (stack->item != nullptr) {
                            if (stack->getItem()->itemId == 77) {  // itemid == 146
                                supplies->selectedHotbarSlot = n;
                                break;
                            }
                        }
                    }
                    vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                    placeBlockAbove(targetPoshehe);

                    supplies->selectedHotbarSlot = slotab;
                }
                if (buttonidchoser.selected == 1) {
                    for (int n = 0; n < 9; n++) {
                        C_ItemStack* stack = inv->getItemStack(n);
                        if (stack->item != nullptr) {
                            if (stack->getItem()->itemId == 143) {  // itemid == 146
                                supplies->selectedHotbarSlot = n;
                                break;
                            }
                        }
                    }
                    vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                    placeBlockAbove(targetPoshehe);

                    supplies->selectedHotbarSlot = slotab;
                }
                if (buttonidchoser.selected == 2) {
                    for (int n = 0; n < 9; n++) {
                        C_ItemStack* stack = inv->getItemStack(n);
                        if (stack->item != nullptr) {
                            if (stack->getItem()->itemId == 4294967155) {  // itemid == 146
                                supplies->selectedHotbarSlot = n;
                                break;
                            }
                        }
                    }
                    vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                    placeBlockAbove(targetPoshehe);

                    supplies->selectedHotbarSlot = slotab;
                }
                if (buttonidchoser.selected == 3) {
                    for (int n = 0; n < 9; n++) {
                        C_ItemStack* stack = inv->getItemStack(n);
                        if (stack->item != nullptr) {
                            if (stack->getItem()->itemId == 4294967152) {  // itemid == 146
                                supplies->selectedHotbarSlot = n;
                                break;
                            }
                        }
                    }
                    vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                    placeBlockAbove(targetPoshehe);

                    supplies->selectedHotbarSlot = slotab;
                }
            }
        }

        stopSwitch = true;
        return;

        // Place blocks extending in each direction from the surrounding hole
        // int extensionRadius = 1; // Number of blocks to extend in each direction
        // placeBlocksInRadius(targetPos1, extensionRadius);

        stopSwitch = true;
        return;
    }

    if (surrArr.empty()) {
        // Silent Code
        int slotab = 0;
        C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
        C_Inventory* inv = supplies->inventory;
        slotab = supplies->selectedHotbarSlot;
        vec3_t playerPos = *g_Data.getLocalPlayer()->getPos();
        if (selftrap) {
            for (int n = 0; n < 9; n++) {
                C_ItemStack* stack = inv->getItemStack(n);
                if (stack->item != nullptr) {
                    if (stack->getItem()->itemId == 49) {  // itemid == 146
                        supplies->selectedHotbarSlot = n;
                        break;
                    }
                }
            }
            // place code

            vec3_t targetPos1 = playerPos.add(vec3_t(1, -1, 0));
            vec3_t targetPos2 = playerPos.add(vec3_t(1, 0, 0));
            vec3_t targetPos3 = playerPos.add(vec3_t(0, 0, 0));
            vec3_t targetPos4 = playerPos.add(vec3_t(-1, 0, 0));

            placeBlockAbove(targetPos1);
            placeBlockAbove(targetPos2);
            placeBlockAbove(targetPos3);
            placeBlockAbove(targetPos4);
            supplies->selectedHotbarSlot = slotab;
        }

        if (buttonplace) {
            if (buttonidchoser.selected == 0) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 77) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
            if (buttonidchoser.selected == 1) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 143) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
            if (buttonidchoser.selected == 2) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 4294967155) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
            if (buttonidchoser.selected == 3) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 4294967152) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
        }
        supplies->selectedHotbarSlot = slotab;

        doReset = true;

    }
    else {
        doReset = false;
    }

    if (doReset == true) {
        surrArr.clear();
        for (auto surroundBlock : regularBlocks) {
            auto playerPosition = g_Data.getLocalPlayer()->getPos();
            auto flooredPos = playerPosition->floor();
            flooredPos.y -= 1.f;
            auto mathsBlockPos = flooredPos.add(surroundBlock);

            if (g_Data.getLocalPlayer()->region->getBlock(mathsBlockPos)->toLegacy()->blockId == 0)  // if that *block* empty, don't place blok
                surrArr.push_back(surroundBlock);
        }

        if (enum1.selected == 1)
            for (auto RenSurroundBlock : regularBlocks) {
                auto playerPosition = g_Data.getLocalPlayer()->getPos();
                auto flooredPos = playerPosition->floor();
                flooredPos.y -= 1.f;
                auto mathsBlockPos = flooredPos.add(RenSurroundBlock);

                if (g_Data.getLocalPlayer()->region->getBlock(mathsBlockPos)->toLegacy()->blockId == 0) surrArr.push_back(RenSurroundBlock);
            }

        // Add two obsidian blocks upward
    }

    // LOCK PLAYER POS
    vec3_t yR = vec3_t(floor(g_Data.getLocalPlayer()->getPos()->x), g_Data.getLocalPlayer()->getPos()->y, floor(g_Data.getLocalPlayer()->getPos()->z));
    yR.x += 0.5f;
    yR.z += 0.5f;
    if (center) {
        g_Data.getLocalPlayer()->setPos(yR);
    }

    // GET BUILD BLOCK
    if (isSideEmpty()) {
        pn = true;
        for (vec3_t surroundBlock : surrArr) {
            auto playerPosition = g_Data.getLocalPlayer()->getPos();
            auto flooredPos = playerPosition->floor();
            flooredPos.y -= 1.f;
            auto mathsBlockPos = flooredPos.add(surroundBlock);
            /*
            if (rotate.GetSelectedEntry().GetValue() == 1 || rotate.GetSelectedEntry().GetValue() == 2) {  // regular rotations
                    rotAngle = g_Data.getLocalPlayer()->getHumanPos().CalcAngle(mathsBlockPos);
                    if (rotate.getSelectedValue() == 1) {
                            gm->player->yawUnused1 = rotAngle.y;
                            gm->player->pitch = rotAngle.x;
                            gm->player->bodyYaw = rotAngle.y;
                    }
            }
            */
            if (doAirplace) {
                gm->buildBlock(&vec3_ti(mathsBlockPos), 0);  // ah so this causes the airplace
            }
            else if (!doAirplace) {
                tryPlace(mathsBlockPos);
            }
            surrArr.erase(surrArr.begin());
            break;
        }
        int slotab = 0;
        C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
        C_Inventory* inv = supplies->inventory;
        slotab = supplies->selectedHotbarSlot;
        vec3_t playerPos = *g_Data.getLocalPlayer()->getPos();
        if (selftrap) {
            for (int n = 0; n < 9; n++) {
                C_ItemStack* stack = inv->getItemStack(n);
                if (stack->item != nullptr) {
                    if (stack->getItem()->itemId == 49) {  // itemid == 146
                        supplies->selectedHotbarSlot = n;
                        break;
                    }
                }
            }
            // place code

            vec3_t targetPos1 = playerPos.add(vec3_t(1, -1, 0));
            vec3_t targetPos2 = playerPos.add(vec3_t(1, 0, 0));
            vec3_t targetPos3 = playerPos.add(vec3_t(0, 0, 0));
            vec3_t targetPos4 = playerPos.add(vec3_t(-1, 0, 0));

            placeBlockAbove(targetPos1);
            placeBlockAbove(targetPos2);
            placeBlockAbove(targetPos3);
            placeBlockAbove(targetPos4);
            supplies->selectedHotbarSlot = slotab;
        }

        if (buttonplace) {
            if (buttonidchoser.selected == 0) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 77) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
            if (buttonidchoser.selected == 1) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 143) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
            if (buttonidchoser.selected == 2) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 4294967155) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
            if (buttonidchoser.selected == 3) {
                for (int n = 0; n < 9; n++) {
                    C_ItemStack* stack = inv->getItemStack(n);
                    if (stack->item != nullptr) {
                        if (stack->getItem()->itemId == 4294967152) {  // itemid == 146
                            supplies->selectedHotbarSlot = n;
                            break;
                        }
                    }
                }
                vec3_t targetPoshehe = g_Data.getLocalPlayer()->getPos()->floor();
                placeBlockAbove(targetPoshehe);

                supplies->selectedHotbarSlot = slotab;
            }
        }
        supplies->selectedHotbarSlot = slotab;
    }
    if (citySwitch && !isSideEmpty() && stopSwitch) {
        stopSwitch = false;
        g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot = origSlot;
    }
}

void Surround::onPreRender(C_MinecraftUIRenderContext* renderCtx) {
    if (g_Data.getLocalPlayer() == nullptr || !renderSurround) return;

    float fadeDistance = 10.f;  // Define the fade distance here (adjust as needed)
    auto playerPosition = g_Data.getLocalPlayer()->getPos();
    auto flooredPos = playerPosition->floor();
    flooredPos.y -= 1.f;

    for (auto surroundBlock : regularBlocks) {
        auto mathsBlockPos = flooredPos.add(surroundBlock);

        if (g_Data.getLocalPlayer()->region->getBlock(mathsBlockPos)->toLegacy()->blockId == 0) {
            // Calculate fading factor based on distance from player
            float distance = std::abs(mathsBlockPos.dist(*playerPosition));
            float fadeFactor = 1.f - (distance / fadeDistance);  // fadeDistance is a predefined threshold distance

            // Set the color with fading effect
            int brightness = static_cast<int>(255 * std::pow(fadeFactor, 4));  // Adjust fading curve and brightness level as needed
            int alpha = static_cast<int>(255 * fadeFactor);

            DrawUtils::setColor(brightness, brightness, brightness, alpha);  // Set brighter color with transparency
            DrawUtils::drawBox(mathsBlockPos, mathsBlockPos.add(1.f), 0.75f, true);
        }
    }
}

/*
void Surround::onSendPacket(Packet* packet) {
        if (packet->isInstanceOf<C_MovePlayerPacket>() && g_Data.getLocalPlayer() != nullptr && rotate.GetSelectedEntry().GetValue() == 2) {  // silent rotations
                if (pn) {
                        auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
                        vec2_t angle = rotAngle;
                        //clientMessageF("Rotating now lol %f %f", angle.x, angle.y);
                        movePacket->pitch = angle.x;
                        movePacket->headYaw = angle.y;
                        movePacket->yaw = angle.y;
                }
        }
}
*/