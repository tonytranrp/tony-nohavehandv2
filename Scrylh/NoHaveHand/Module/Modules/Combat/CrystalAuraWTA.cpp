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

bool hasLineOfSight(const vec3_t& crystalPos, C_Entity* target) {
	vec3_t targetPos = *target->getPos();
	vec3_t crystalEyePos = crystalPos;
	crystalEyePos.y += 1.5;

	vec3_t crystalToTarget = targetPos.sub(crystalEyePos);
	double distanceToTarget = crystalToTarget.length();

	vec3_t rayDirection = crystalToTarget.normalize();

	vec3_t currentPos = crystalEyePos;
	for (double distance = 0; distance < distanceToTarget; distance += 0.5) {
		currentPos = currentPos.add(rayDirection.mul(0.5));
		vec3_ti blockPos(static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z));
		if (!isAirBlock(blockPos)) {
			return false;
		}
	}

	return true;
}

float calculateDamage(const vec3_t& crystalPos, C_Entity* target) {
	float distance = crystalPos.dist(*target->getPos());
	float LOSMultiplier = hasLineOfSight(crystalPos, target) ? 1.0f : 0.0f;
	float damage = BASE_DAMAGE * (1.0f - DISTANCE_MULTIPLIER * distance) * LOSMultiplier;
	return std::max(0.0f, damage);
}

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

struct BlockWithScore {
	vec3_t* position;
	float score;

	BlockWithScore(vec3_t* pos, float s) : position(pos), score(s) {}
};

bool compareBlockScores(const BlockWithScore& a, const BlockWithScore& b) {
	return a.score > b.score;
}

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

	for (float x = -2.0f; x <= 2.0f; x += 0.5f) {
		for (float z = -2.0f; z <= 2.0f; z += 0.5f) {
			for (float y = 0.0f; y >= -2.0f; y -= 0.5f) {  // Check positions below target's Y level
				vec3_ti block(std::round(entPos.x + x), std::round(entPos.y + y), std::round(entPos.z + z));

				// Calculate the center of the block for precise placement
				vec3_t blockCenter(block.x + 0.5f, block.y + 0.5f, block.z + 0.5f);

				// Check if the block is air and has line of sight to the entity
				if (g_Data.getLocalPlayer()->region->getBlock(block)->toLegacy()->blockId == 0) {
					if (hasEnoughAirBlocks(ent, blockCenter) && !checkTargetCollision(blockCenter, ent)) {
						// Adjust placement slightly downwards
						blockCenter.y -= 0.5f;

						float distanceToBlock = blockCenter.dist(*ent->getPos());
						float damage = calculateDamage(blockCenter, ent);
						float score = damage / distanceToBlock;
						blockScores.push_back(BlockWithScore(blockCenter, score));
					}
				}
				else if (g_Data.getLocalPlayer()->region->getBlock(vec3_ti(block.x + 1, block.y, block.z))->toLegacy()->blockId == 145) {
					vec3_ti adjacentBlock(block.x + 1, block.y, block.z);

					// Calculate the center of the adjacent block for precise placement
					vec3_t adjacentBlockCenter(adjacentBlock.x + 0.5f, adjacentBlock.y + 0.5f, adjacentBlock.z + 0.5f);

					if (g_Data.getLocalPlayer()->region->getBlock(adjacentBlock)->toLegacy()->blockId == 0 &&
						hasLineOfSight(adjacentBlockCenter, ent) && !checkTargetCollision(adjacentBlockCenter, ent)) {

						// Adjust placement slightly downwards
						adjacentBlockCenter.y -= 0.5f;

						float distanceToBlock = adjacentBlockCenter.dist(*ent->getPos());
						float damage = calculateDamage(adjacentBlockCenter, ent);
						float score = damage / distanceToBlock;
						blockScores.push_back(BlockWithScore(adjacentBlockCenter, score));
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
	for (size_t i = 0; i < blockScores.size() && finalBlocks.size() < maxPlacementCount; i++) {
		bool validPosition = true;

		for (auto& placed : finalBlocks) {
			if (blockScores[i].position.dist(placed) < 1.0f) {
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

				for (int i = 0; i < numCrystalsToPlace; i++) {
					float damage = calculateDamage(placementPositions[i], target);
					if (damage > 0.0f) {
						gm->buildBlock(&vec3_ti(placementPositions[i].x, placementPositions[i].y - 1, placementPositions[i].z), 4);
						placeArr.push_back(vec3_t(placementPositions[i].x, placementPositions[i].y - 1, placementPositions[i].z));
						hasPlaced = true;
					}
				}

				g_Data.forEachEntity([](C_Entity* ent, bool b) {
					int id = ent->getEntityTypeId();
					if (id == 71 && g_Data.getLocalPlayer()->getPos()->dist(*ent->getPos()) <= 6) {
						g_Data.getCGameMode()->attack(ent);
					}
					});

				supplies->selectedHotbarSlot = slotCA;
				placementPositions.clear();

				break; // Exit the loop after placing crystals for one target
			}
		}
	}
	else if (!targetList7.empty()) {
		crystalDelay++;
	}
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

						DrawUtils::drawBox(postt.add(0.f, 0.999f, 0.f), vec3_t(postt.x + 1.f, postt.y + 1.f, postt.z + 1.f), 0.5f, true);
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