#include "CrystalAuraWTA.h"
#include "../../../../Utils/TargetUtil.h"

#include "../../ModuleManager.h"
bool mobs = false;
static uintptr_t HiveRotations1 = Utils::getBase() + 0x8F3895;
static uintptr_t HiveRotations2 = Utils::getBase() + 0x8F87C7;
CrystalAuraWTA::CrystalAuraWTA() : IModule(0, Category::COMBAT, "p100 java CA by JohnTWD // FatMC#1209") {
	registerIntSetting("Delay (ticks)", &this->delay, this->delay, 0, 25);
	registerIntSetting("Max Proximity", &this->maxProximity, this->maxProximity, 1, 8);
	registerFloatSetting("Enemy Range", &this->range, this->range, 5, 15);
	registerFloatSetting("Place Range", &this->placeRange, this->placeRange, 0.f, 12.f);
	priority = SettingEnum(this)
		.addEntry(EnumEntry("Distance", 0))
		.addEntry(EnumEntry("Health", 1));
	registerEnumSetting("Priority", &this->priority, 0);
	registerFloatSetting("Thru Walls", &this->thruWallsR, this->thruWallsR, 0, 10);
	registerFloatSetting("Post Walls", &this->postWallsR, this->postWallsR, 0.f, 10.f);

	calcDmgType = SettingEnum(this)
		.addEntry(EnumEntry("Java", 0))
		.addEntry(EnumEntry("Bedrock", 1));
	registerEnumSetting("calcDmgType", &this->calcDmgType, 0);

	registerBoolSetting("Safety First!", &this->safetyFirst, this->safetyFirst);
	registerFloatSetting("Minimum Health", &this->minHealth, this->minHealth, 0.f, 20.f);
	registerFloatSetting("Max Self Damage", &this->maxSelfDmg, this->maxSelfDmg, 0.f, 20.f);
	registerFloatSetting("Min Enem Damage", &this->minEnemDmg, this->minEnemDmg, 0.f, 20.f);

	facePlaceType = SettingEnum(this)
		.addEntry(EnumEntry("Smart", 0))
		.addEntry(EnumEntry("None", 1));
	registerEnumSetting("Faceplace Type", &this->facePlaceType, 0);
	registerFloatSetting("FP Threshold", &this->fpThresh, this->fpThresh, 0.f, 20.f);
	registerFloatSetting("FP MinimumDmg", &this->dmgAtThresh, this->dmgAtThresh, 0.f, 20.f);

	registerBoolSetting("Render", &this->renderPlacing, this->renderPlacing);
	registerBoolSetting("1.13+ ", &this->noCheckUpper, this->noCheckUpper);
	registerBoolSetting("Multi-Attack", &this->attackMulti, this->attackMulti);

	rotate = SettingEnum(this)
		.addEntry(EnumEntry("none", 0))
		.addEntry(EnumEntry("normal", 1))
		.addEntry(EnumEntry("silent", 2))
		.addEntry(EnumEntry("pitchUp", 3));
	registerEnumSetting("Rotations", &this->rotate, 0);
}

CrystalAuraWTA::~CrystalAuraWTA() {};

const char* CrystalAuraWTA::getModuleName() {
	return ("CrystalPlaceJTWD");
}
int slotCA = 0;

void findCr() {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	auto prevSlot = supplies->selectedHotbarSlot;
	for (int n = 0; n < 36; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr) {
			if (stack->getItem()->itemId == 637) {
				if (prevSlot != n)
					supplies->selectedHotbarSlot = n;
				return;
			}
		}
	}

}
/*
int findRelativeAngle(vec3_t playerVec, vec3_t enemyVec) {

	vec3_t deltaVec = playerVec.floor().sub(enemyVec.floor());

	if (deltaVec == vec3_t(0, 0, 0))
		return -1;
	if (deltaVec.x == 0 && deltaVec.z > 0)
		return 0;
	if (deltaVec.x > 0 && deltaVec.z == 0)
		return 90;
	if (deltaVec.x == 0 && deltaVec.z < 0)
		return 180;
	if (deltaVec.x < 0 && deltaVec.z == 0)
		return 270;

	float degreeOffset = atan2(deltaVec.x, deltaVec.z);
	degreeOffset = degreeOffset * (180 / PI);

	if ((deltaVec.x > 0 && deltaVec.z > 0) || (deltaVec.x > 0 && deltaVec.z < 0))
		return degreeOffset;
	else if ((deltaVec.x < 0 && deltaVec.z < 0) || (deltaVec.x < 0 && deltaVec.z > 0))
		return degreeOffset + 360;
}
*/

static std::vector<C_Entity*> tgtList;
bool locateEntCrPl(C_Entity* curEnt, bool isRegularEntity) {
	if (curEnt == nullptr) return false;
	if (curEnt == g_Data.getLocalPlayer()) return false;
	if (!curEnt->isAlive()) return false;
	if (!g_Data.getLocalPlayer()->isAlive()) return false;
	if (curEnt->getEntityTypeId() == 71) return false;
	if (curEnt->getEntityTypeId() == 66) return false;
	if (curEnt->getEntityTypeId() == 64) return false;
	if (curEnt->getEntityTypeId() == 69) return false;
	if (curEnt->width <= 0.01f || curEnt->height <= 0.01f) return false;
	if (!TargetUtil::isValidTarget(curEnt)) return false;

	float dist = (curEnt->getHumanPos()).dist(g_Data.getLocalPlayer()->getHumanPos());
	if (dist <= moduleMgr->getModule<CrystalAuraWTA>()->range) {
		tgtList.push_back(curEnt);
		return true;
	}
	return false;
}

static std::vector<C_Entity*> entLocator120;
bool processEntForentLocator120(C_Entity* curEnt, bool isRegularEntity) {
	if (curEnt == nullptr) return false;
	if (!curEnt->isAlive()) return false;
	if (!g_Data.getLocalPlayer()->isAlive()) return false;
	if (curEnt->getEntityTypeId() == 71) return false;
	if (curEnt->getEntityTypeId() == 69) return false;
	if (curEnt->width <= 0.01f || curEnt->height <= 0.01f) return false;

	float dist = (curEnt->getHumanPos()).dist(g_Data.getLocalPlayer()->getHumanPos());
	if (dist <= moduleMgr->getModule<CrystalAuraWTA>()->range) {
		entLocator120.push_back(curEnt);
		return true;
	}
	return false;
}

void CrystalAuraWTA::onEnable() {
	if (g_Data.getLocalPlayer() == nullptr) return;

	CJTWDPlaceArr.clear();
	tgtList.clear();
	entLocator120.clear();
	return;
}

bool CrystalAuraWTA::isPlaceValid(vec3_t location, C_Entity* atkObj) {
	auto* localPlayer = g_Data.getLocalPlayer();
	auto* region = localPlayer->region;
	auto& aabb = localPlayer->aabb;

	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(
		FindSignature("40 53 55 41 56 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24"));

	static std::unordered_map<vec3_ti, int, Vec3Hash> cachedBlockTypes;

	auto getCachedBlockType = [&](const vec3_ti& pos) -> int {
		auto it = cachedBlockTypes.find(pos);
		if (it != cachedBlockTypes.end()) {
			return it->second;
		}

		int blockType = region->getBlock(pos)->toLegacy()->blockId;
		cachedBlockTypes[pos] = blockType;
		return blockType;
	};

	auto blockType = getCachedBlockType(location);
	if (blockType != 0)
		return false;

	if (!noCheckUpper) {
		auto upperPos = location.add(0, 1, 0);
		auto upperBlockType = getCachedBlockType(upperPos);
		if (upperBlockType != 0)
			return false;

		if (checkCornerHitboxCollision(upperPos, atkObj))
			return false;
	}

	if (checkCornerHitboxCollision(location, atkObj))
		return false;

	auto lowerPos = location.sub(0, 1, 0);
	auto lowerBlockType = getCachedBlockType(lowerPos);
	if (lowerBlockType != 49 && lowerBlockType != 7)
		return false;

	float distToLoc = location.dist(localPlayer->getHumanPos());
	if (distToLoc > placeRange)
		return false;

	if (localPlayer->getHealth() < minHealth)
		return false;

	float possibleDmg = computeExplosionDamage(location, localPlayer, region);
	if (possibleDmg > maxSelfDmg && maxSelfDmg != 20.0f)
		return false;

	if (atkObj->getHumanPos().floor().y >= location.y) {
		float targetSeenPercent = getSeenPercent(atkObj->region, location.add(0.5f, 0.0f, 0.5f), atkObj->aabb);
		float enemyDmg = computeExplosionDamage(location, atkObj, atkObj->region);

		if (enemyDmg < minEnemDmg && minEnemDmg != 0.0f && targetSeenPercent == 0.0f)
			return false;
	}

	if ((atkObj->getHumanPos().floor().y + 1) <= location.y && facePlaceType.GetSelectedEntry().GetValue() == 0) {
		if ((fpThresh < atkObj->getHealth()) && fpThresh != 20.0f)
			return false;

		float enemyDmg = computeExplosionDamage(location, atkObj, atkObj->region);
		if (enemyDmg < dmgAtThresh && enemyDmg != 0.0f)
			return false;
	}
	else if ((atkObj->getHumanPos().floor().y + 1) <= location.y && facePlaceType.GetSelectedEntry().GetValue() == 1)
		return false;

	g_Data.forEachEntity(processEntForentLocator120);
	for (auto i : entLocator120) {
		if (i == atkObj)
			continue;
		if (i->getHumanPos().floor() == location)
			return false;
	}

	if (localPlayer->getHumanPos().floor() == location)
		return false;

	return true;
}

#include <unordered_map>

float CrystalAuraWTA::computeExplosionDamage(vec3_t crystalPos, C_Entity* target, C_BlockSource* reg) {
	constexpr float explosionRadius = 12.0f;
	float dist = target->getHumanPos().dist(crystalPos) / explosionRadius;

	if (dist > 1.0f)
		return 0.0f;

	float exposure = 0.0f;
	float impact = 0.0f;
	int armorPoints = 0;
	int epf = 0;

	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(
		FindSignature("40 53 55 41 56 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4"));

	static std::unordered_map<int, int> armorMapping = {
		{573, 2}, {335, 1}, {336, 3}, {337, 2}, {338, 1}, {351, 2}, {352, 5}, {353, 3}, {354, 1},
		{339, 2}, {340, 5}, {341, 4}, {342, 1}, {343, 2}, {344, 6}, {345, 5}, {346, 2}, {347, 3},
		{609, 3}, {350, 3}, {612, 3}, {348, 8}, {610, 8}, {349, 6}, {611, 6}
	};

	if (calcDmgType.GetSelectedEntry().GetValue() == 0) {  // calcDmgType == JAVA
		float toughness = 0.0f;
		float distedsize = target->getHumanPos().dist(crystalPos) / explosionRadius;
		double blockDensity = getSeenPercent(reg, crystalPos.add(0.5f, 0.0f, 0.5f), target->aabb);
		double v = (1.0 - distedsize) * blockDensity;

		float damage = static_cast<float>(static_cast<int>((v * v + v) / 2.0 * 7.0 * explosionRadius + 1.0));
		damage = damage * 3.0 / 2.0;

		bool hasArmor = false;
		for (int i = 0; i < 4; ++i) {
			C_ItemStack* armor = target->getArmor(i);

			if (armor->item == nullptr)
				continue;

			int armorId = armor->getItem()->itemId;
			armorPoints += armorMapping[armorId];
			hasArmor = true;
			epf += static_cast<int>(getBlastDamageEnchantReduction(armor));
		}

		if (hasArmor) {
			float F_epf = fminf(static_cast<float>(epf), 15.0f) / 25.0f;
			damage = damage * fminf(static_cast<float>(armorPoints) / 5, 1 / (static_cast<float>(armorPoints) / 5));
			damage = damage * (1 - F_epf);
		}
		return damage;
	}
	else if (calcDmgType.GetSelectedEntry().GetValue() == 1) {
		exposure = getSeenPercent(reg, crystalPos.add(0.5f, 0.0f, 0.5f), target->aabb);
		impact = (1 - dist) * exposure;
		int damage = static_cast<int>((impact * impact * 3.5f + impact * 0.5f * 7.0f) * explosionRadius + 1.0f);

		float finalDamage = static_cast<float>(damage);

		for (int i = 0; i < 4; ++i) {
			C_ItemStack* armor = target->getArmor(i);

			if (armor->item == nullptr)
				continue;

			armorPoints += armor->getItem()->getArmorValue();
			epf += static_cast<int>(getBlastDamageEnchantReduction(armor));
		}

		finalDamage -= finalDamage * armorPoints * 0.04f;
		finalDamage -= finalDamage * std::min(ceilf(std::min(epf, 25) * 75.0f / 100), 20.0f) * 0.04f;
		return finalDamage;
	}

	return 0.0f;
}



float CrystalAuraWTA::getBlastDamageEnchantReduction(C_ItemStack* armor) {
	float epf = 0.f;

	if (calcDmgType.GetSelectedEntry().GetValue() == 0) {
		epf += (armor->getEnchantValue(0) != 4) ? armor->getEnchantValue(0) : 5;
		epf += armor->getEnchantValue(3) * 2.f;
	}
	else if (calcDmgType.GetSelectedEntry().GetValue() == 1) {
		epf += (armor->getEnchantValue(0) != 4) ? armor->getEnchantValue(0) : 5;
		epf += armor->getEnchantValue(3) * 2.f;
	}

	return epf;
}
#include <unordered_set>
std::vector<CrystalPlacements> CrystalAuraWTA::generateValidPlacements(C_Entity* target, int yOffset) {
	vec3_t targetPos = target->getHumanPos().floor().add(0, static_cast<float>(yOffset), 0);

	std::vector<CrystalPlacements> validPlacements;
	std::unordered_set<vec3_t, Vec3Hash> checkedPositions;

	for (int x = -maxProximity; x < maxProximity; x++) {
		for (int z = -maxProximity; z < maxProximity; z++) {
			vec3_t search = targetPos.add(x, 0, z);

			if (search.dist(target->getHumanPos()) < maxProximity && targetPos.floor() != search) {
				if (checkedPositions.find(search) == checkedPositions.end()) {
					if (isPlaceValid(search, target)) {
						CrystalPlacements placement;
						placement.toPlace = search;
						placement.enemyDmg = computeExplosionDamage(search, target, target->region);
						placement.selfDmg = computeExplosionDamage(search, g_Data.getLocalPlayer(), g_Data.getLocalPlayer()->region);

						validPlacements.push_back(placement);
					}
					checkedPositions.insert(search);
				}
			}
		}
	}

	return validPlacements;
}

bool cmpPlacements(CrystalPlacements E1, CrystalPlacements E2) {
	bool cmpType = moduleMgr->getModule<CrystalAuraWTA>()->safetyFirst;

	if (!cmpType) {
		return E1.enemyDmg > E2.enemyDmg;
	}

	if (cmpType) {
		return E1.selfDmg < E2.selfDmg;
	}
}

bool cmpDup(CrystalPlacements E1, CrystalPlacements E2) {
	bool cmpType = not(moduleMgr->getModule<CrystalAuraWTA>()->safetyFirst);

	if (!cmpType) {
		return E1.enemyDmg > E2.enemyDmg;
	}

	if (cmpType) {
		return E1.selfDmg < E2.selfDmg;
	}
}
float getVisibility(const vec3_t& position, const AABB& targetAABB) {
	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(
		FindSignature("40 53 55 41 56 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24"));
	vec3_t pos = vec3_t(0.5f, 0.f, 0.5f);
	return getSeenPercent(g_Data.getLocalPlayer()->region, position.add(pos), targetAABB);
}

CrystalPlacements CrystalAuraWTA::bestPlaceOnPlane(C_Entity* targ, int yLevel) {
	std::vector<CrystalPlacements> validPlacements = generateValidPlacements(targ, yLevel);

	if (validPlacements.empty()) {
		CrystalPlacements emptyPlacement;
		emptyPlacement.enemyDmg = -42069;
		return emptyPlacement;
	}

	auto cmpCustom = [targ](const CrystalPlacements& E1, const CrystalPlacements& E2) {
		float distanceE1 = E1.toPlace.distanceTo(*targ->getPos());
		float distanceE2 = E2.toPlace.distanceTo(*targ->getPos());

		float visibility1 = getVisibility(E1.toPlace, targ->aabb);
		float visibility2 = getVisibility(E2.toPlace, targ->aabb);

		if (visibility1 != visibility2)
			return visibility1 > visibility2;

		if (distanceE1 != distanceE2)
			return distanceE1 < distanceE2;

		return E1.enemyDmg > E2.enemyDmg;
	};

	std::sort(validPlacements.begin(), validPlacements.end(), cmpCustom);

	float highestDamage = 0.0f;
	std::vector<CrystalPlacements> duplicates;

	if (safetyFirst) {
		for (const CrystalPlacements& placement : validPlacements) {
			highestDamage = placement.selfDmg;
			if (placement.selfDmg == highestDamage) {
				duplicates.push_back(placement);
			}
			else {
				break;
			}
		}
	}
	else {
		for (const CrystalPlacements& placement : validPlacements) {
			highestDamage = placement.enemyDmg;
			if (placement.enemyDmg == highestDamage) {
				duplicates.push_back(placement);
			}
			else {
				break;
			}
		}
	}

	if (duplicates.size() == 1)
		return duplicates[0];

	std::sort(duplicates.begin(), duplicates.end(), cmpDup);

	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(
		FindSignature("40 53 55 41 56 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24"));

	for (const CrystalPlacements& duplicate : duplicates) {
		vec3_t block = duplicate.toPlace;

		float exposure = getSeenPercent(g_Data.getLocalPlayer()->region, block.add(0.5f, 0.f, 0.5f), targ->aabb);

		if (exposure > 0.5f && !checkCornerHitboxCollision(block, targ)) {
			return duplicate;
		}
	}

	return duplicates[0];
}


#include <algorithm>

bool cmpAgain(CrystalPlacements E1, CrystalPlacements E2) {
	return E1.enemyDmg > E2.enemyDmg;
}

CrystalPlacements CrystalAuraWTA::CrystalAuraJTWD(C_Entity* target) {
	const int maxVOff = (facePlaceType.GetSelectedEntry().GetValue() == 1 ? 1 : 3);
	const int minVOff = -5;

	std::vector<CrystalPlacements> validPlacements;

	auto calculatePlacement = [this](C_Entity* target, int vOffset) -> CrystalPlacements {
		CrystalPlacements placement = bestPlaceOnPlane(target, vOffset);
		if (placement.enemyDmg != -42069) {
			return placement;
		}
		else {
			return {};
		}
	};

#pragma omp parallel for
	for (int vOffset = minVOff; vOffset < maxVOff; ++vOffset) {
		CrystalPlacements placement = calculatePlacement(target, vOffset);

#pragma omp critical
		{
			if (placement.enemyDmg != -42069) {
				validPlacements.push_back(placement);
			}
		}
	}

	if (validPlacements.empty()) {
		return {};
	}

	auto advancedComparator = [target](const CrystalPlacements& place1, const CrystalPlacements& place2) {
		float distance1 = place1.toPlace.distanceTo(*target->getPos());
		float distance2 = place2.toPlace.distanceTo(*target->getPos());

		float visibility1 = getVisibility(place1.toPlace, target->aabb);
		float visibility2 = getVisibility(place2.toPlace, target->aabb);

		if (visibility1 != visibility2) {
			return visibility1 > visibility2;
		}

		if (distance1 != distance2) {
			return distance1 < distance2;
		}

		return place1.enemyDmg > place2.enemyDmg;
	};

	// Sort placements outside of parallel section for better performance
	std::sort(validPlacements.begin(), validPlacements.end(), advancedComparator);

	return validPlacements[0];
}


bool compareTargListVec(C_Entity* E1, C_Entity* E2) {
	int whatToCompare = moduleMgr->getModule<CrystalAuraWTA>()->priority.GetSelectedEntry().GetValue();

	if (whatToCompare == 0) {
		return g_Data.getLocalPlayer()->getHumanPos().dist(E1->getHumanPos()) < g_Data.getLocalPlayer()->getHumanPos().dist(E2->getHumanPos());
	}
	else if (whatToCompare == 1) {
		return E1->getHealth() < E2->getHealth();
	}
}
int switchSlot;
bool grabNow = false;
bool hasRotten = false;
bool packetSent = false;
bool isPlayerAuthInput = false;
vec3_t postorot;
#include "../pch.h"
#include "../../../../Utils/Logger.h"

void placeCrystal(const vec3_t& position) {
	g_Data.getCGameMode()->buildBlock(&vec3_ti(position).sub(0, 1, 0), 0);
	g_Data.getCGameMode()->continueBuildBlock(vec3_ti(position).sub(0, 1, 0), 0);
}

void breakCrystal(const vec3_t& position) {
	
}
bool hasPlaced = false;
void CrystalAuraWTA::onTick(C_GameMode* gm) {
	hasRotten = false;
	rotUpNow = true;
	packetSent = false;
	isPlayerAuthInput = false;

	placeArrEmpty = CJTWDPlaceArr.empty();
	entLocator120.clear();
	tgtList.clear();
	CJTWDPlaceArr.clear();

	g_Data.forEachEntity(locateEntCrPl);

	if (tgtList.empty()) return;

	std::sort(tgtList.begin(), tgtList.end(), compareTargListVec);

	auto supplies = g_Data.getLocalPlayer()->getSupplies();
	auto inv = supplies->inventory;

	if (!attackMulti) {
		if (!tgtList.empty()) {
			findCr();

			CrystalPlacements placeInfo = CrystalAuraJTWD(tgtList[0]);
			if (placeInfo.enemyDmg != -42069) {
				CrystalInfo a2vec;
				a2vec.CPlacements = placeInfo;
				a2vec.ent = tgtList[0];
				CJTWDPlaceArr.push_back(a2vec);
			}
		}
	}
	else if (attackMulti) {
		for (auto& enemy : tgtList) {
			findCr();

			CrystalPlacements placeInfo = CrystalAuraJTWD(enemy);
			if (placeInfo.enemyDmg != -42069) {
				CrystalInfo a2vec;
				a2vec.CPlacements = placeInfo;
				a2vec.ent = enemy;
				CJTWDPlaceArr.push_back(a2vec);
			}
		}
	}

	ctr++;
	C_ItemStack* item = supplies->inventory->getItemStack(0);

	if (ctr >= delay) {
		for (CrystalInfo& Place : CJTWDPlaceArr) {
			vec3_t placeMe = Place.CPlacements.toPlace;

			placeCrystal(placeMe);

			findCr();
			
			
		}
		
		g_Data.forEachEntity([](C_Entity* ent, bool b) {
			if (tgtList.empty()) return;
			int id = ent->getEntityTypeId();
			int range = moduleMgr->getModule<CrystalAuraWTA>()->range;
			if (id == 71 && g_Data.getLocalPlayer()->getPos()->dist(*ent->getPos()) <= range) {
				g_Data.getCGameMode()->attack(ent);
				hasPlaced = false;

				if (!moduleMgr->getModule<NoSwing>()->isEnabled())
					g_Data.getLocalPlayer()->swingArm();
			}
			});
		CJTWDPlaceArr.clear();

		
		supplies->selectedHotbarSlot = slotCA;


		
		ctr = 0;
	}
}

# define M_PI 3.14159265358979323846 /* pi */

vec2_t GetRotations(vec3_t playerEyePos, vec3_t* targetPos) {
	vec3_t delta = *targetPos - playerEyePos;
	float yaw = atan2(delta.z, delta.x) * 180.0f / M_PI;
	float pitch = atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) * 180.0f / M_PI;
	return { -pitch, yaw - 90 };
}
std::vector<CrystalInfo> CJTWDPlaceArr1;
void CrystalAuraWTA::onPlayerTick(C_Player* plr) {
	if (plr == nullptr) return;

	for (auto& i : tgtList) {
		CrystalPlacements placeInfo1 = CrystalAuraJTWD(i);
		if (placeInfo1.enemyDmg != -42069) {
			CrystalInfo a2vec1;
			a2vec1.CPlacements = placeInfo1;
			a2vec1.ent = i;
			CJTWDPlaceArr1.push_back(a2vec1);
			for (CrystalInfo& Place1 : CJTWDPlaceArr1)
			{
				vec3_t placeMe = Place1.CPlacements.toPlace;
				auto rot = GetRotations(plr->currentPos, &placeMe);
				plr->yawUnused1 = rot.y;
				plr->pitch = rot.x;
				plr->bodyYaw = rot.y;

			}
		}

	}

}

void CrystalAuraWTA::onDisable() {

	Utils::patchBytes((unsigned char*)HiveRotations1, (unsigned char*)"\x89\x41\x18", 3);
	Utils::patchBytes((unsigned char*)HiveRotations2, (unsigned char*)"\xC7\x40\x18\x03\x00\x00\x00", 7);
}

float roundoff(float value, unsigned char prec) {
	float pow_10 = pow(10.0f, (float)prec);
	return round(value * pow_10) / pow_10;
}

std::string chopOffDigits(std::string STUFF, int digits) {
	bool afterDecimalPt = false;
	int dp = 0;
	std::string toOutput;

	for (int i = 0; i < STUFF.length(); ++i) {
		if (STUFF.at(i) == '.') {
			afterDecimalPt = true;
		}

		toOutput.append(std::string(1, STUFF.at(i)));

		if (afterDecimalPt) {
			dp++;

			if (dp > digits)
				break;
		}
	}

	return toOutput;
}
vec3_t prevRenderPos;

void CrystalAuraWTA::onPreRender(C_MinecraftUIRenderContext* renderCtx) {
	if (g_Data.getLocalPlayer() == nullptr) return;
	if (!g_Data.getLocalPlayer()->isAlive()) {
		moduleMgr->getModule<CrystalBreak>()->setEnabled(false);
		return;
	}

	if (!g_Data.canUseMoveKeys() || tgtList.empty() || !g_Data.isInGame() || tgtList[0] == nullptr || tgtList.empty() || tgtList[0] == nullptr || !g_Data.getLocalPlayer()->isAlive()) {
		tgtList.clear();
		return;
	}
	if (CJTWDPlaceArr.empty()) return;

	if (renderPlacing) {
		for (CrystalInfo& indivCrystal : CJTWDPlaceArr) {
			if (CJTWDPlaceArr.empty() || tgtList.empty()) return;

			vec3_t placeLoc = indivCrystal.CPlacements.toPlace;
			float enemyDmg = indivCrystal.CPlacements.enemyDmg;
			float selfDmg = indivCrystal.CPlacements.selfDmg;

			placeLoc = prevRenderPos + (placeLoc - prevRenderPos) * 0.3f;

			DrawUtils::setColor(.1f, .75f, 1.f, 1.f);
			DrawUtils::drawBox(placeLoc.sub(0, 1, 0), placeLoc.add(1, 0, 1), .4f, false);

			std::string self_dmg = std::to_string(roundoff(selfDmg, 2));
			self_dmg = chopOffDigits(self_dmg, 2);

			DrawUtils::drawTextInWorld(&self_dmg, placeLoc.sub(0, 1.4f, 0), 0.6f, vec3_ti(66, 238, 238), vec3_ti(12, 12, 12), .1f);

			vec3_t posplace = indivCrystal.ent->getHumanPos().floor();

			std::string enem_dmg = std::to_string(roundoff(enemyDmg, 2));
			enem_dmg = chopOffDigits(enem_dmg, 2);

			DrawUtils::drawTextInWorld(&enem_dmg, placeLoc.sub(0, 1.2f, 0), 0.6f, vec3_ti(255, 0, 0), vec3_ti(12, 12, 12), .1f);

			prevRenderPos = placeLoc;
		}
	}

	return;
}
std::vector<CrystalInfo> CJTWDPlaceArr11;
void CrystalAuraWTA::onSendPacket(Packet* packet) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	if (!tgtList.empty()) {

		if (packet->isInstanceOf<C_MovePlayerPacket>() && !tgtList.empty()) {

			for (auto& i : tgtList) {
				CrystalPlacements placeInfo11 = CrystalAuraJTWD(i);
				if (placeInfo11.enemyDmg != -42069) {
					CrystalInfo a2vec11;
					auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
					a2vec11.CPlacements = placeInfo11;
					a2vec11.ent = i;
					CJTWDPlaceArr11.push_back(a2vec11);
					for (CrystalInfo& Place1 : CJTWDPlaceArr11)
					{
						vec3_t placeMe = Place1.CPlacements.toPlace;
						auto rot = GetRotations(player->currentPos, &placeMe);
						movePacket->pitch = rot.x;
						movePacket->headYaw = rot.y;
						movePacket->yaw = rot.y;

					}
				}

			}

		}

	}
}