#include "CrystalAuraWTA.h"
#include <cmath>
#include <vector>
#include <algorithm>

CrystalAuraWTA::CrystalAuraWTA() : IModule(0x0, Category::COMBAT, "CrystalAura by Tony UWU") {
    registerIntSetting("range", &range, range, 1, 10);
    registerBoolSetting("autoplace", &autoplace, autoplace);

    registerIntSetting("delay", &delay, delay, 0, 20);
    registerBoolSetting("onClick", &isClick, isClick);

}
int crystalDelay = 0;
int crystalDelay2 = 0;
int crystalDelay3 = 0;

CrystalAuraWTA::~CrystalAuraWTA() {
}

const char* CrystalAuraWTA::getModuleName() {
    return ("CrystalAuraMIX");
}
static std::vector<C_Entity*> targetList7;

struct CompareTargetEnArray {
    bool operator()(C_Entity* lhs, C_Entity* rhs) {
        C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        return (*lhs->getPos()).dist(*localPlayer->getPos()) < (*rhs->getPos()).dist(*localPlayer->getPos());
    }
};

void findEntity3(C_Entity* currentEntity, bool isRegularEntity) {
    static auto CrystalAuraWTAMod = moduleMgr->getModule<CrystalAuraWTA>();

    if (currentEntity == nullptr)
        return;

    if (currentEntity->getNameTag()->getTextLength() <= 1 && currentEntity->getEntityTypeId() == 71)
        return;

    if (currentEntity == g_Data.getLocalPlayer())
        return;

    if (!g_Data.getLocalPlayer()->isAlive())
        return;

    if (!currentEntity->isAlive())
        return;

    if (currentEntity->getNameTag()->getTextLength() <= 1 && currentEntity->getEntityTypeId() == 319)
        return;
    if (currentEntity->width <= 0.01f || currentEntity->height <= 0.01f)
        return;
    if (currentEntity->getEntityTypeId() == 64)
        return;
    if (currentEntity->getEntityTypeId() == 69)
        return;
    if (currentEntity->getEntityTypeId() == 80)
        return;

    if (!TargetUtil::isValidTarget(currentEntity))
        return;

    float dist = (*currentEntity->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

    if (dist < CrystalAuraWTAMod->range) {
        targetList7.push_back(currentEntity);
        sort(targetList7.begin(), targetList7.end(), CompareTargetEnArray());
    }
}
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream> // Added for debugging

const float BASE_DAMAGE = 5.0f;
const float DISTANCE_MULTIPLIER = 0.1f;
const int OBSIDIAN_ID = 49;
const int BEDROCK_ID = 7;

bool isAirBlock(const vec3_ti& blockPos) {
    int blockId = g_Data.getLocalPlayer()->region->getBlock(blockPos)->toLegacy()->blockId;
    return blockId == 0;
}

bool hasEnoughAirBlocks(C_Entity* ent, const vec3_t& pos) {
    int airBlockCount = 0;
    int obsidianOrBedrockCount = 0;

    for (int yOffset = 0; yOffset < 2; yOffset++) {
        vec3_ti blockPos(static_cast<int>(pos.x), static_cast<int>(pos.y + yOffset), static_cast<int>(pos.z));
        if (isAirBlock(blockPos)) {
            airBlockCount++;
        }
        else {
            int blockId = g_Data.getLocalPlayer()->region->getBlock(blockPos)->toLegacy()->blockId;
            if (blockId == OBSIDIAN_ID || blockId == BEDROCK_ID) {
                obsidianOrBedrockCount++;
            }
        }
    }

    return airBlockCount >= 2 && obsidianOrBedrockCount <= 0;
}

bool checkTargetCollision(const vec3_t& block, C_Entity* ent) {
    vec3_t entPos = ent->getPos()->floor();
    entPos.y -= 1;

    // Check if the block is within the entity's bounding box.
    if (block.x >= entPos.x && block.x <= entPos.x + 1 &&
        block.y >= entPos.y && block.y <= entPos.y + 2 &&
        block.z >= entPos.z && block.z <= entPos.z + 1) {
        return true;
    }

    // Check if the block is close to any corners of the entity's bounding box.
    std::vector<vec3_t> corners = {
        { ent->aabb.lower.x, ent->aabb.lower.y, ent->aabb.lower.z },
        { ent->aabb.lower.x, ent->aabb.lower.y, ent->aabb.upper.z },
        { ent->aabb.upper.x, ent->aabb.lower.y, ent->aabb.upper.z },
        { ent->aabb.upper.x, ent->aabb.lower.y, ent->aabb.lower.z }
    };

    for (const auto& corner : corners) {
        // Check if the block is close to each corner.
        if (block.distanceTo(corner) <= 1.0f) {
            return true;
        }
    }

    return false;
}

bool isBlockInLineOfSight(const vec3_t& blockCenter, C_Entity* ent, float maxDistance) {
    vec3_t rayDir = ent->getPos()->sub(blockCenter);
    rayDir.normalize();

    vec3_t startPoint = blockCenter;
    float distance = 0.0f;

    while (distance <= maxDistance) {
        vec3_ti blockPos(static_cast<int>(startPoint.x), static_cast<int>(startPoint.y), static_cast<int>(startPoint.z));

        if (!isAirBlock(blockPos)) {
            return false;
        }

        if (checkTargetCollision(blockPos.toVector3(), ent)) {
            return true;
        }

        startPoint = startPoint.add(rayDir);
        distance = startPoint.dist(blockCenter);
    }

    return false;
}

bool canCrystalDamageTarget(const vec3_t& crystalPos, C_Entity* target, const vec3_t& rayDir, float maxDistance) {
    vec3_t targetPos = *target->getPos();
    vec3_t crystalEyePos = crystalPos;
    crystalEyePos.y += 1.5;

    vec3_t crystalToTarget = targetPos.sub(crystalEyePos);
    double distanceToTarget = crystalToTarget.length();

    vec3_t normalizedRayDir = rayDir;
    normalizedRayDir.normalize();

    vec3_t currentPos = crystalEyePos;
    for (double distance = 0; distance < distanceToTarget; distance += 0.5) {
        currentPos = currentPos.add(normalizedRayDir.mul(0.5));
        vec3_ti blockPos(static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z));
        if (!isAirBlock(blockPos)) {
            return false;
        }

        if (distance >= maxDistance) {
            return false;
        }
    }

    return true;
}

bool checkSurrounded2(C_Entity* ent) {
    vec3_t entPos = ent->getPos()->floor();
    entPos.y -= 1;

    bool centerBlockIsAir = isAirBlock(vec3_ti(entPos.x, entPos.y, entPos.z));

    if (!centerBlockIsAir) {
        return false;  // Center block must be air.
    }

    // Check for surrounding blocks in the x and z dimensions.
    for (int x = -1; x <= 1; x++) {
        for (int z = -1; z <= 1; z++) {
            if (x != 0 || z != 0) {
                vec3_ti block(entPos.x + x, entPos.y, entPos.z + z);
                if (!isAirBlock(block) || !hasEnoughAirBlocks(ent, block.toVector3())) {
                    return false;
                }
            }
        }
    }

    // Check for enough air blocks above the entity.
    int requiredAirBlocksAbove = 2;  // You can adjust this as needed.
    for (int y = 1; y <= requiredAirBlocksAbove; y++) {
        vec3_ti block(entPos.x, entPos.y + y, entPos.z);
        if (!isAirBlock(block)) {
            return false;
        }
    }

    return true;
}

bool hasLineOfSight(const vec3_t& crystalPos, C_Entity* target) {
    vec3_t targetPos = *target->getPos();
    vec3_t crystalEyePos = crystalPos;
    crystalEyePos.y += 1.5;

    vec3_t rayDirection = targetPos.sub(crystalEyePos);
    double distanceToTarget = rayDirection.length();
    rayDirection.normalize();

    return isBlockInLineOfSight(crystalEyePos, target, distanceToTarget);
}

float calculateDamage(const vec3_t& crystalPos, C_Entity* target) {
    float distance = crystalPos.dist(*target->getPos());
    float LOSMultiplier = hasLineOfSight(crystalPos, target) ? 1.0f : 0.0f;
    float damage = BASE_DAMAGE * (1.0f - DISTANCE_MULTIPLIER * distance) * LOSMultiplier;
    return std::max(0.0f, damage);
}

struct BlockWithScore {
    vec3_t position;
    float score;

    BlockWithScore(const vec3_t& pos, float s) : position(pos), score(s) {}
};

bool compareBlockScores(const BlockWithScore& a, const BlockWithScore& b) {
    return a.score > b.score;
}

std::vector<vec3_t> findFloorPlacement(C_Entity* ent, C_Entity* target) {
    vec3_t entPos = ent->getPos()->floor();
    vec3_t targetPos = target->getPos()->floor();
    std::vector<vec3_t> floorBlocks;

    if (targetPos.y >= entPos.y) {
        // The target is above or at the same height as the player.
        return floorBlocks; // No need for floor placement.
    }

    // Calculate the difference in height between the player and target.
    int heightDifference = static_cast<int>(entPos.y - targetPos.y);

    // Start from the target's position and go downwards to find a suitable floor.
    for (int yOffset = 0; yOffset < heightDifference; yOffset++) {
        vec3_ti block(targetPos.x, targetPos.y + yOffset, targetPos.z);
        auto blkID = g_Data.getLocalPlayer()->region->getBlock(block)->toLegacy()->blockId;

        if (blkID == 0 && !checkTargetCollision(block.toVector3(), ent)) {
            floorBlocks.push_back(vec3_t(block.x, block.y, block.z));
        }
        else {
            break; // Stop if a non-air block is encountered.
        }
    }

    return floorBlocks;
}
std::vector<vec3_t> getGucciPlacement(C_Entity* ent, C_Entity* target) {
    vec3_t entPos = ent->getPos()->floor();
    entPos.y -= 1;
    std::vector<vec3_t> finalBlocks;

    std::vector<BlockWithScore> blockScores;
    vec3_t targetPos = *target->getPos();
    vec3_t direction = targetPos.sub(*ent->getPos()).normalize();

    // Set the maximum distance for placement.
    float maxDistance = 4.0f; // You can adjust this value.

    // Define the positions to prioritize (x and z offsets).
    std::vector<vec3_ti> priorityPositions = {
        { 1, 0, 0 },
        { -1, 0, 0 },
        { 0, 0, 1 },
        { 0, 0, -1 }
    };

    // Iterate through the priority positions first.
    for (const vec3_ti& priorityOffset : priorityPositions) {
        vec3_ti block(static_cast<int>(entPos.x + priorityOffset.x), static_cast<int>(entPos.y), static_cast<int>(entPos.z + priorityOffset.z));
        vec3_t blockCenter(static_cast<float>(block.x), static_cast<float>(block.y), static_cast<float>(block.z));

        // Check if the block is within the specified max distance.
        if (blockCenter.dist(*ent->getPos()) <= maxDistance) {
            if (isBlockInLineOfSight(blockCenter, ent, maxDistance)) {
                if (g_Data.getLocalPlayer()->region->getBlock(block)->toLegacy()->blockId == 0) {
                    if (checkTargetCollision(blockCenter, ent)) {
                        // Exclude placement positions that are exactly at the target position.
                        if (blockCenter.x != targetPos.x || blockCenter.y != targetPos.y || blockCenter.z != targetPos.z) {
                            blockCenter.y -= 1.0f;
                            float distanceToBlock = blockCenter.dist(*ent->getPos());
                            float damage = calculateDamage(blockCenter, ent);
                            float score = damage / distanceToBlock;

                            // Check if the block is behind the player.
                            vec3_t blockToPlayer = entPos.sub(blockCenter).normalize();
                            float dotProduct = blockToPlayer.dot(direction);

                            // Reduce the score for blocks in front of the player.
                            if (dotProduct < -0.5) {
                                score *= 0.5f;
                            }

                            blockScores.push_back(BlockWithScore(blockCenter, score));
                        }
                    }
                }
            }
        }
    }

    // Extend the placement positions if they can damage the target and have enough air blocks.
    for (float x = -2.0f; x <= 2.0f; x++) {
        for (float z = -2.0f; z <= 2.0f; z++) {
            vec3_ti block(static_cast<int>(entPos.x + x), static_cast<int>(entPos.y), static_cast<int>(entPos.z + z));
            vec3_t blockCenter(static_cast<float>(block.x), static_cast<float>(block.y), static_cast<float>(block.z));

            // Check if the block is within the specified max distance.
            if (blockCenter.dist(*ent->getPos()) <= maxDistance) {
                if (isBlockInLineOfSight(blockCenter, ent, maxDistance)) {
                    if (g_Data.getLocalPlayer()->region->getBlock(block)->toLegacy()->blockId == 0) {
                        if (checkTargetCollision(blockCenter, ent)) {
                            // Exclude placement positions that are exactly at the target position.
                            if (blockCenter.x != targetPos.x || blockCenter.y != targetPos.y || blockCenter.z != targetPos.z) {
                                blockCenter.y -= 1.0f;
                                float distanceToBlock = blockCenter.dist(*ent->getPos());
                                float damage = calculateDamage(blockCenter, ent);
                                float score = damage / distanceToBlock;

                                // Check if the block is behind the player.
                                vec3_t blockToPlayer = entPos.sub(blockCenter).normalize();
                                float dotProduct = blockToPlayer.dot(direction);

                                // Reduce the score for blocks in front of the player.
                                if (dotProduct < -0.5) {
                                    score *= 0.5f;
                                }

                                blockScores.push_back(BlockWithScore(blockCenter, score));
                            }
                        }
                    }
                }
            }
        }
    }

    // Define a custom sorting function that considers weighted scores and distances.
    auto customSort = [&](const BlockWithScore& a, const BlockWithScore& b) {
        float weightedScoreA = a.score / (1.0f + a.position.dist(*ent->getPos()));
        float weightedScoreB = b.score / (1.0f + b.position.dist(*ent->getPos()));
        return weightedScoreA > weightedScoreB;
    };

    // Sort the blockScores using the custom sorting function.
    std::sort(blockScores.begin(), blockScores.end(), customSort);

    int maxPlacementCount = 5;
    float minDistanceThreshold = 1.0f;

    for (size_t i = 0; i < blockScores.size() && finalBlocks.size() < maxPlacementCount; i++) {
        float weightedScore = blockScores[i].score;
        float distanceToTarget = blockScores[i].position.dist(*ent->getPos());

        // Adjust the weighted score based on distance to the target.
        weightedScore /= (1.0f + distanceToTarget);

        bool validPosition = true;
        for (auto& placed : finalBlocks) {
            float distanceToPlaced = blockScores[i].position.dist(placed);

            if (distanceToPlaced < minDistanceThreshold) {
                // Reduce the weighted score for positions too close to each other.
                weightedScore -= (minDistanceThreshold - distanceToPlaced) * 0.1f;
            }

            if (weightedScore <= 0.0f) {
                validPosition = false;
                break;
            }
        }

        if (validPosition) {
            finalBlocks.push_back(blockScores[i].position);
        }
    }

    return finalBlocks;
}

bool hasPlaced = false;
void CrystalAuraWTA::onEnable() {
    crystalDelay = 0;
    hasPlaced = false;
}
void findCr() {
    C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
    C_Inventory* inv = supplies->inventory;
    auto prevSlot = supplies->selectedHotbarSlot;
    for(int n = 0; n < 36; n++) {
        C_ItemStack* stack = inv->getItemStack(n);
        if (stack->item != nullptr) {
            if (stack->getItem()->itemId == 637) {
                if (prevSlot != n)
                    supplies->selectedHotbarSlot = n;
                return;
            }
        }
    }
    //if (stack->getItem()->itemId == 637) {
}
vec3_t espPosLower;
vec3_t espPosUpper;
vec3_t crystalPos;
std::vector<vec3_t> placeArr;
void CrystalAuraWTA::onTick(C_GameMode* gm) {
    // Check if the module is enabled and there is a local player.
    if (!isEnabled() || !g_Data.getLocalPlayer())
        return;

    // Check if the right mouse button is held down (if required).
    if (isClick && !g_Data.isRightClickDown())
        return;

    targetList7.clear();

    // Find potential target entities.
    g_Data.forEachEntity(findEntity3);

    // Check if there are targets to process.
    if (autoplace && crystalDelay >= delay && !targetList7.empty()) {
        crystalDelay = 0;

        // Sort the target list by distance.
        std::sort(targetList7.begin(), targetList7.end(), CompareTargetEnArray());

        for (auto target : targetList7) {
            // Find crystals in the player's inventory.
            auto supplies = g_Data.getLocalPlayer()->getSupplies();
            auto inv = supplies->inventory;
            slotCA = supplies->selectedHotbarSlot;
            C_ItemStack* item = supplies->inventory->getItemStack(0);
            findCr();

            // Get potential crystal placement positions.
            std::vector<vec3_t> placementPositions = getGucciPlacement(target, g_Data.getLocalPlayer());

            if (!placementPositions.empty()) {
                // Sort placement positions by damage (highest to lowest).
                std::sort(placementPositions.begin(), placementPositions.end(), [&](const vec3_t& a, const vec3_t& b) {
                    float damageA = calculateDamage(a, target);
                    float damageB = calculateDamage(b, target);
                    return damageA > damageB;
                    });

                // Determine how many crystals to place based on the number of available positions.
                int numCrystalsToPlace = std::min(5, static_cast<int>(placementPositions.size()));

                if (numCrystalsToPlace >= 1) {
                    // Calculate scores for each placement position based on criteria (customize this part).
                    std::vector<BlockWithScore> placementScores;

                    for (int i = 0; i < numCrystalsToPlace; i++) {
                        vec3_t crystalPos = placementPositions[i];
                        float damage = calculateDamage(crystalPos, target);

                        // You can add additional criteria and scoring here.
                        float score = damage;  // For example, score based on damage.

                        // Store the position and its score.
                        placementScores.emplace_back(crystalPos, score);
                    }

                    // Sort placement positions by score (highest to lowest).
                    std::sort(placementScores.begin(), placementScores.end(), compareBlockScores);

                    for (const BlockWithScore& placement : placementScores) {
                        vec3_t crystalPos = placement.position;

                        // Place the crystal if all checks pass.
                        gm->buildBlock(&vec3_ti(crystalPos.x, crystalPos.y, crystalPos.z), 10);
                        placeArr.push_back(vec3_t(crystalPos.x, crystalPos.y, crystalPos.z));

                        // Attack nearby entities if necessary.
                        g_Data.forEachEntity([](C_Entity* ent, bool b) {
                            if (targetList7.empty()) return;
                            int id = ent->getEntityTypeId();
                            int range = moduleMgr->getModule<CrystalAuraWTA>()->range;
                            if (id == 71 && g_Data.getLocalPlayer()->getPos()->dist(*ent->getPos()) <= range) {
                                g_Data.getCGameMode()->attack(ent);
                                hasPlaced = false;

                                if (!moduleMgr->getModule<NoSwing>()->isEnabled())
                                    g_Data.getLocalPlayer()->swingArm();
                            }
                            });

                        hasPlaced = true;
                    }
                }

                supplies->selectedHotbarSlot = slotCA;
                placementPositions.clear();

                break; // Only place crystals for the first target in the sorted list.
            }
        }
    }
    else if (!targetList7.empty()) {
        crystalDelay++;
    }
}


void CrystalAuraWTA::onPlayerTick(C_Player* plr) {
    if (plr == nullptr) return;

}

void CrystalAuraWTA::onDisable() {
    crystalDelay = 0;
    hasPlaced = false;
}
void CrystalAuraWTA::onPreRender(C_MinecraftUIRenderContext* renderCtx) {
    if (renderCA) {
        auto interfaceColor = ColorUtil::interfaceColor(1);
        C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (localPlayer != nullptr && GameData::canUseMoveKeys()) {
            if (!targetList7.empty()) {
                if (!placeArr.empty()) {
                    for (auto& postt : placeArr) {
                        DrawUtils::setColor(interfaceColor.r, interfaceColor.g, interfaceColor.b, 1.f);

                        DrawUtils::drawBox(postt.floor(), vec3_t(floor(postt.x) + 1.f, floor(postt.y) + 1.f, floor(postt.z) + 1.f), 0.5f, true);
                    }
                    placeArr.clear();
                }
            }
        }
    }
}