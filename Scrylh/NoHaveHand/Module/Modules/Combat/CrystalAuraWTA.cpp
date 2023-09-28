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



// Function to check if a crystal can damage a target using advanced ray tracing




bool checkTargetCollision(const vec3_t& block, C_Entity* ent) {
    vec3_t entPos = ent->getPos()->floor();
    entPos.y -= 1;

    // Check if the target is inside the block
    if (block.x >= entPos.x && block.x <= entPos.x + 1 &&
        block.y >= entPos.y && block.y <= entPos.y + 2 &&
        block.z >= entPos.z && block.z <= entPos.z + 1) {
        return true;
    }

    std::vector<vec3_t> corners = {
        { ent->aabb.lower.x, ent->aabb.lower.y, ent->aabb.lower.z },
        { ent->aabb.lower.x, ent->aabb.lower.y, ent->aabb.upper.z },
        { ent->aabb.upper.x, ent->aabb.lower.y, ent->aabb.upper.z },
        { ent->aabb.upper.x, ent->aabb.lower.y, ent->aabb.lower.z }
    };

    std::vector<std::pair<vec3_t, float>> cornerScores;
    for (const auto& corner : corners) {
        float distance = corner.distanceTo(block);
        cornerScores.push_back(std::make_pair(corner, distance));
    }

    std::sort(cornerScores.begin(), cornerScores.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    for (const auto& scorePair : cornerScores) {
        const auto& corner = scorePair.first;

        // Check if the target is inside the block
        if (std::floor(corner.x) == std::floor(block.x) &&
            std::floor(corner.y) == std::floor(block.y) &&
            std::floor(corner.z) == std::floor(block.z)) {
            return true;
        }
    }

    return false;
}
bool isBlockInLineOfSight(C_Entity* ent, const vec3_t& blockCenter, const vec3_t& rayDir, float maxDistance) {
    vec3_t startPoint = ent->eyePos0; // Use the eye position vector
    float distance = 0.0f;

    while (distance <= maxDistance) {
        vec3_ti blockPos(static_cast<int>(startPoint.x), static_cast<int>(startPoint.y), static_cast<int>(startPoint.z));

        if (!isAirBlock(blockPos)) {
            return false; // A non-air block was hit before reaching the target block
        }

        // Check if the block is within the target's hitbox
        if (checkTargetCollision(blockPos.toVector3(), ent)) {
            return true;
        }

        startPoint = startPoint.add(rayDir);
        distance = startPoint.dist(ent->eyePos0); // Use the entity's eye position for distance calculation
    }

    return false; // No obstacles in the line of sight
}
bool canCrystalDamageTarget(const vec3_t& crystalPos, C_Entity* target, const vec3_t& rayDir, float maxDistance) {
    vec3_t targetPos = *target->getPos();
    vec3_t crystalEyePos = crystalPos;
    crystalEyePos.y += 1.5;

    vec3_t crystalToTarget = targetPos.sub(crystalEyePos);
    double distanceToTarget = crystalToTarget.length();

    vec3_t normalizedRayDir = rayDir; // Make a copy of rayDir
    normalizedRayDir.normalize(); // Normalize the copy


    vec3_t currentPos = crystalEyePos;
    for (double distance = 0; distance < distanceToTarget; distance += 0.5) {
        currentPos = currentPos.add(normalizedRayDir.mul(0.5));
        vec3_ti blockPos(static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z));
        if (!isAirBlock(blockPos)) {
            return false;
        }

        // Check if the distance exceeds the maximum allowed distance
        if (distance >= maxDistance) {
            return false;
        }
    }

    return true;
}

bool checkSurrounded2(C_Entity* ent) {
    vec3_t entPos = ent->getPos()->floor();
    entPos.y -= 1;

    // Add checks for surrounding blocks
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

    return true;
}
bool hasLineOfSight(const vec3_t& crystalPos, C_Entity* target) {
    vec3_t targetPos = *target->getPos();
    vec3_t crystalEyePos = crystalPos;
    crystalEyePos.y += 1.5;

    vec3_t crystalToTarget = targetPos.sub(crystalEyePos);
    double distanceToTarget = crystalToTarget.length();

    vec3_t rayDirection = crystalToTarget.normalize();

    return isBlockInLineOfSight(target, crystalEyePos, rayDirection, distanceToTarget);
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
// Function to calculate a list of suitable crystal placement positions
std::vector<vec3_t> getGucciPlacement(C_Entity* ent) {
    vec3_t entPos = ent->getPos()->floor();
    entPos.y -= 1;
    std::vector<vec3_t> finalBlocks;

    struct BlockWithScore {
        vec3_t position;
        float score;

        BlockWithScore(const vec3_t& pos, float s) : position(pos), score(s) {}
    };

    std::vector<BlockWithScore> blockScores;

    // Calculate the maximum distance to check for placement
    float maxDistance = 2.5f;  // Adjust this value as needed

    // Determine if the player is in the air
    bool playerInAir = entPos.y != g_Data.getLocalPlayer()->getPos()->y;

    // Calculate the ray direction
    vec3_t rayDir = ent->getPointingStruct()->rayHitVec - *ent->getPos(); // Calculate ray direction here

    // Check positions around the player in the air to find any suitable block
    if (playerInAir) {
        for (float x = -2.0f; x <= 2.0f; x++) {
            for (float z = -2.0f; z <= 2.0f; z++) {
                vec3_ti block(std::round(entPos.x + x), std::round(entPos.y), std::round(entPos.z + z));
                vec3_t blockCenter(static_cast<float>(block.x), static_cast<float>(block.y), static_cast<float>(block.z));

                // Perform advanced ray tracing to check if the crystal can damage the target
                if (canCrystalDamageTarget(blockCenter, ent, rayDir, maxDistance)) { // Pass rayDir and maxDistance
                    if (g_Data.getLocalPlayer()->region->getBlock(block)->toLegacy()->blockId == 0) {
                        blockCenter.y -= 1.0f; // Place on the block's top surface
                        float distanceToBlock = blockCenter.dist(*ent->getPos());
                        float damage = calculateDamage(blockCenter, ent);
                        float score = damage / distanceToBlock;
                        blockScores.push_back(BlockWithScore(blockCenter, score));
                    }
                }
            }
        }
    }

    // Check for placement on the floor
    for (float x = -2.0f; x <= 2.0f; x += 0.5f) {
        for (float z = -2.0f; z <= 2.0f; z += 0.5f) {
            vec3_ti block(std::round(entPos.x + x), std::round(entPos.y - 1), std::round(entPos.z + z));
            vec3_t blockCenter(static_cast<float>(block.x), static_cast<float>(block.y), static_cast<float>(block.z));

            if (g_Data.getLocalPlayer()->region->getBlock(block)->toLegacy()->blockId == 0) {
                if (hasEnoughAirBlocks(ent, blockCenter) && !checkTargetCollision(blockCenter, ent)) {
                    // Perform advanced ray tracing to check if the crystal can damage the target
                    if (canCrystalDamageTarget(blockCenter, ent, rayDir, maxDistance)) { // Pass rayDir and maxDistance
                        float distanceToBlock = blockCenter.dist(*ent->getPos());
                        float damage = calculateDamage(blockCenter, ent);
                        float score = damage / distanceToBlock;
                        blockScores.push_back(BlockWithScore(blockCenter, score));
                    }
                }
            }
        }
    }

    // Sort blocks by score in descending order
    std::sort(blockScores.begin(), blockScores.end(), [](const BlockWithScore& a, const BlockWithScore& b) {
        return a.score > b.score;
        });

    int maxPlacementCount = 5;
    float minDistanceThreshold = 1.0f; // Minimum distance allowed between placements

    for (size_t i = 0; i < blockScores.size() && finalBlocks.size() < maxPlacementCount; i++) {
        // Calculate a weighted score that combines damage and distance
        float weightedScore = blockScores[i].score;

        // Check proximity to already selected positions
        bool validPosition = true;
        for (auto& placed : finalBlocks) {
            float distanceToPlaced = blockScores[i].position.dist(placed);

            // Adjust the weighted score based on proximity (closer is better)
            if (distanceToPlaced < minDistanceThreshold) {
                weightedScore -= (minDistanceThreshold - distanceToPlaced) * 0.1f; // Adjust weight as needed
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
	for (int n = 0; n < 9; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr) {
			if (stack->getItem()->itemId == 637) {
				supplies->selectedHotbarSlot = n;

				return;
			}
		}
	}

}
vec3_t espPosLower;
vec3_t espPosUpper;
vec3_t crystalPos;
std::vector<vec3_t> placeArr;
void CrystalAuraWTA::onTick(C_GameMode* gm) {
    if (g_Data.getLocalPlayer() == nullptr) return;
    if (isClick && !g_Data.isRightClickDown()) return;

    targetList7.clear();

    g_Data.forEachEntity(findEntity3);

    if (autoplace && (crystalDelay >= this->delay) && !(targetList7.empty())) {
        crystalDelay = 0;

        std::sort(targetList7.begin(), targetList7.end(), CompareTargetEnArray());

        for (auto target : targetList7) {
            auto supplies = g_Data.getLocalPlayer()->getSupplies();
            auto inv = supplies->inventory;
            slotCA = supplies->selectedHotbarSlot;
            C_ItemStack* item = supplies->inventory->getItemStack(0);
            findCr();

            std::vector<vec3_t> placementPositions = getGucciPlacement(target);

            if (!placementPositions.empty()) {
                // Multiplace logic
                int numCrystalsToPlace = std::min(5, static_cast<int>(placementPositions.size()));

                // Check if there are enough positions to multiplace
                if (numCrystalsToPlace >= 1) {
                    for (int i = 0; i < numCrystalsToPlace; i++) {
                        vec3_t crystalPos = placementPositions[i];
                        float damage = calculateDamage(crystalPos, target);

                        // Additional checks and calculations
                        bool shouldPlace = true;

                        // Place the crystal if all conditions are met
                        if (shouldPlace) {
                            gm->buildBlock(&vec3_ti(crystalPos.x, crystalPos.y, crystalPos.z), 4);
                            placeArr.push_back(vec3_t(crystalPos.x, crystalPos.y, crystalPos.z));
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
                                });  //*/
                            hasPlaced = true;
                        }
                    }
                }
                else {
                    // If there are not enough positions for multiplace, place a single crystal
                    vec3_t crystalPos = placementPositions[0];
                    float damage = calculateDamage(crystalPos, target);

                    // Additional checks and calculations
                    bool shouldPlace = true;

                    // Place the crystal if all conditions are met
                    if (shouldPlace) {
                        gm->buildBlock(&vec3_ti(crystalPos.x, crystalPos.y, crystalPos.z), 4);
                        placeArr.push_back(vec3_t(crystalPos.x, crystalPos.y, crystalPos.z));
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
                            });  //*/
                        hasPlaced = true;
                    }
                }

                supplies->selectedHotbarSlot = slotCA;
                placementPositions.clear();

                break; 
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


/*
	bool isPlusXblock = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x + 1, entPos.y, entPos.z))->toLegacy()->blockId == 0;
	bool isPlusXblock1 = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x + 1, entPos.y + 1, entPos.z))->toLegacy()->blockId != 0;

	bool isairblockX = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x + 2, entPos.y, entPos.z))->toLegacy()->blockId == 0;
	bool isairblockX1 = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x + 2, entPos.y + 2, entPos.z))->toLegacy()->blockId == 0;

	bool isairblockminusX = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x - 2, entPos.y, entPos.z))->toLegacy()->blockId == 0;
	bool isairblockminnusX1 = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x - 2, entPos.y + 2, entPos.z))->toLegacy()->blockId == 0;

	bool isairblockZ = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x, entPos.y, entPos.z + 2))->toLegacy()->blockId == 0;
	bool isairblockZ1 = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x, entPos.y + 2, entPos.z + 2))->toLegacy()->blockId == 0;

	bool isairblockminusZ = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x, entPos.y, entPos.z - 2))->toLegacy()->blockId == 0;
	bool isairblockminusZ1 = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x, entPos.y + 2, entPos.z - 2))->toLegacy()->blockId == 0;

	bool isPlusXAnvil = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x + 1, entPos.y, entPos.z))->toLegacy()->blockId == 145;
	bool isMinusXAnvil = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x - 1, entPos.y, entPos.z))->toLegacy()->blockId == 145;
	bool isPlusZAnvil = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x, entPos.y, entPos.z + 1))->toLegacy()->blockId == 145;
	bool isMinusZAnvil = g_Data.getLocalPlayer()->region->getBlock(vec3_ti(entPos.x, entPos.y, entPos.z - 1))->toLegacy()->blockId == 145;

	if (isPlusXAnvil) {
		if (isairblockX && isairblockX1) {
			finalBlocks.push_back(new vec3_t(entPos.x + 2, entPos.y, entPos.z));
		}
	}
	if (isMinusXAnvil) {
		if (isairblockminusX && isairblockminnusX1) {
			finalBlocks.push_back(new vec3_t(entPos.x - 2, entPos.y, entPos.z));
		}

	}
	if (isPlusZAnvil) {
		if (isairblockZ && isairblockZ1) {
			finalBlocks.push_back(new vec3_t(entPos.x, entPos.y, entPos.z + 2));
		}

	}
	if (isMinusZAnvil) {
		if (isairblockminusZ && isairblockminusZ1) {
			finalBlocks.push_back(new vec3_t(entPos.x, entPos.y, entPos.z - 2));
		}

	}

	if (isPlusXblock && isPlusXblock1) {
		if (isairblockX && isairblockX1) {
			finalBlocks.push_back(new vec3_t(entPos.x + 2, entPos.y, entPos.z));
		}
	}*/