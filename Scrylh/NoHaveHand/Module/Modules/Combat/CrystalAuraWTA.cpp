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
	auto blockType = region->getBlock(location)->toLegacy()->blockId;
	if (blockType != 0)
		return false;

	if (!noCheckUpper) {
		auto upperBlockType = region->getBlock(location.add(0, 1, 0))->toLegacy()->blockId;
		if (upperBlockType != 0)
			return false;

		if (checkCornerHitboxCollision(location.add(0, 1, 0), atkObj))
			return false;
	}

	if (checkCornerHitboxCollision(location, atkObj))
		return false;

	auto lowerBlockType = region->getBlock(location.sub(0, 1, 0))->toLegacy()->blockId;
	if (lowerBlockType != 49 && lowerBlockType != 7)
		return false;

	float distToLoc = location.dist(localPlayer->getHumanPos());
	if (distToLoc > placeRange)
		return false;

	if (localPlayer->getHealth() < minHealth)
		return false;

	float possibleDmg = computeExplosionDamage(location, localPlayer, region);
	if (possibleDmg > maxSelfDmg && maxSelfDmg != 20.f)
		
		return false;

	if (atkObj->getHumanPos().floor().y >= location.y) {
		float targetSeenPercent = getSeenPercent(atkObj->region, location.add(0.5f, 0.f, 0.5f), atkObj->aabb);
		float enemyDmg = computeExplosionDamage(location, atkObj, atkObj->region);

		if (enemyDmg < minEnemDmg && minEnemDmg != 0.f && targetSeenPercent == 0.f)
			return false;
	}

	if ((atkObj->getHumanPos().floor().y + 1) <= location.y && facePlaceType.GetSelectedEntry().GetValue() == 0) {
		if ((fpThresh < atkObj->getHealth()) && fpThresh != 20.f)
			return false;

		float enemyDmg = computeExplosionDamage(location, atkObj, atkObj->region);
		if (enemyDmg < dmgAtThresh && enemyDmg != 0.f)
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

	if (g_Data.getLocalPlayer()->getHumanPos().floor() == location)
		return false;

	return true;
}
float CrystalAuraWTA::computeExplosionDamage(vec3_t crystalPos, C_Entity* target, C_BlockSource* reg) {
	constexpr float explosionRadius = 12.f;

	vec3_t pos = target->getHumanPos();
	float dist = pos.dist(crystalPos) / explosionRadius;

	if (dist > 1.f) {
		return 0.f;
	}

	int armorPoints = 0, epf = 0;
	float damage = 0.f;

	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(
		FindSignature("40 53 55 41 56 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24"));

	float distedsize = target->getHumanPos().dist(crystalPos) / explosionRadius;
	double blockDensity = getSeenPercent(reg, crystalPos.add(0.5f, 0.f, 0.5f), target->aabb);
	double v = (1.0 - distedsize) * blockDensity;

	if (calcDmgType.GetSelectedEntry().GetValue() == 0) {
		float armorProtection = 0.f;

		for (int i = 0; i < 4; i++) {
			C_ItemStack* armor = target->getArmor(i);
			if (armor->item == nullptr) continue;

			int armorId = armor->getItem()->itemId;

			switch (armorId) {
			case 573: case 335: case 338: case 351: case 353: case 354:
				armorProtection += 1.0 - (1.0 / (2.0 * armorPoints + 2.0)); break;
			case 336: case 337: case 340: case 341: case 344: case 345: case 346: case 609: case 610: case 611: case 612:
				armorProtection += 1.0 - (1.0 / (4.0 * armorPoints + 4.0)); break;
			case 347: case 350:
				armorProtection += 3.0; break;
			case 348:
				armorProtection += 8.0; break;
			case 349:
				armorProtection += 6.0; break;
			}

			epf += (int)getBlastDamageEnchantReduction(armor);
		}

		float F_epf = fminf((float)epf, 15.f) / 25.f;
		damage = (float)(int)(v * v * 7.0 * explosionRadius * (1.0 - armorProtection) + 1.0);
		damage *= (1 - F_epf);
	}
	else if (calcDmgType.GetSelectedEntry().GetValue() == 1) {

		float exposure = getSeenPercent(reg, crystalPos.add(0.5f, 0.f, 0.5f), target->aabb);
		float impact = (1 - dist) * exposure;
		damage = (float)(int)((impact * impact * 3.5f + impact * 0.5f * 7.f) * explosionRadius + 1.f);

		for (int i = 0; i < 4; i++) {
			C_ItemStack* armor = target->getArmor(i);
			if (armor->item == nullptr) continue;

			armorPoints += (*armor->item)->getArmorValue();
			epf += (int)getBlastDamageEnchantReduction(armor);
		}

		damage -= damage * armorPoints * 0.04f;
		damage -= damage * std::min(ceilf(std::min(epf, 25) * 0.75f), 20.f) * 0.04f;
	}

	return damage;
}

float CrystalAuraWTA::getBlastDamageEnchantReduction(C_ItemStack* armor) {
	float epf = 0.f;

	if (calcDmgType.GetSelectedEntry().GetValue() == 0) {
		epf += (armor->getEnchantValue(0) != 4) ? armor->getEnchantValue(0) : 5;
		epf += armor->getEnchantValue(3) * 2.f;
	}
	else if (calcDmgType.GetSelectedEntry().GetValue() == 1) {
		epf += armor->getEnchantValue(0);
		epf += armor->getEnchantValue(3) * 2.f;
	}

	return epf;
}

std::vector<CrystalPlacements> CrystalAuraWTA::generateValidPlacements(C_Entity* target, int yOffset) {
	vec3_t targetPos = target->getHumanPos().floor();
	targetPos = vec3_t(targetPos.x, targetPos.y + (float)yOffset, targetPos.z);

	std::vector<CrystalPlacements> bunchashit;

	for (int i = 0; i < maxProximity * maxProximity; i++) {
		int x = i % (2 * maxProximity) - maxProximity;
		int z = i / (2 * maxProximity) - maxProximity;

		vec3_t search = targetPos.add(x, 0, z);
		if (search.dist(target->getHumanPos()) < maxProximity &&
			target->getHumanPos().floor() != search &&
			isPlaceValid(search, target)) {

			CrystalPlacements me;
			me.toPlace = search;
			me.enemyDmg = computeExplosionDamage(search, target, target->region);
			me.selfDmg = computeExplosionDamage(search, g_Data.getLocalPlayer(), g_Data.getLocalPlayer()->region);

			bunchashit.push_back(me);
		}
	}

	return bunchashit;
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
	std::vector<CrystalPlacements> validShit = generateValidPlacements(targ, yLevel);

	if (validShit.empty()) {
		CrystalPlacements anEmptyValue;
		anEmptyValue.enemyDmg = -42069;
		return anEmptyValue;
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

	std::sort(validShit.begin(), validShit.end(), cmpCustom);

	float hl = 0.f;
	std::vector<CrystalPlacements> dups;

	if (safetyFirst) {
		for (const CrystalPlacements& i : validShit) {
			hl = i.selfDmg;
			if (i.selfDmg == hl) {
				dups.push_back(i);
			}
			else {
				break;
			}
		}
	}
	else {
		for (const CrystalPlacements& i : validShit) {
			hl = i.enemyDmg;
			if (i.enemyDmg == hl) {
				dups.push_back(i);
			}
			else {
				break;
			}
		}
	}

	if (dups.size() == 1)
		return dups[0];

	std::sort(dups.begin(), dups.end(), cmpDup);
	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(FindSignature("40 53 55 41 56 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 84 24"));

	for (const CrystalPlacements& dup : dups) {
		vec3_t block = dup.toPlace;

		float exposure = getSeenPercent(g_Data.getLocalPlayer()->region, block.add(0.5f, 0.f, 0.5f), targ->aabb);

		if (exposure > 0.5f && !checkCornerHitboxCollision(block, targ)) {
			return dup;
		}
	}

	return dups[0];
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

			return { };
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

		return { };
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

#pragma omp parallel sections
	{
#pragma omp section
		{

			std::sort(validPlacements.begin(), validPlacements.end(), advancedComparator);
		}
	}

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
// Define a delay between crystal placements to prevent excessive actions
constexpr int CrystalPlacementDelay = 2;

void placeCrystal(const vec3_t& position) {
	g_Data.getCGameMode()->buildBlock(&vec3_ti(position).sub(0, 1, 0), 0);
	g_Data.getCGameMode()->continueBuildBlock(vec3_ti(position).sub(0, 1, 0), 0);
}

void breakCrystal(const vec3_t& position) {
	g_Data.forEachEntity([position](C_Entity* ent, bool b) {
		int id = ent->getEntityTypeId();
		int range = moduleMgr->getModule<CrystalAuraWTA>()->placeRange;

		if (id == 71 && g_Data.getLocalPlayer()->getPos()->dist(*ent->getPos()) <= range) {
			g_Data.getCGameMode()->attack(ent);
			g_Data.getLocalPlayer()->pointingStruct->rayHitType = 0;
			Utils::nopBytes((unsigned char*)HiveRotations1, 3);
			Utils::patchBytes((unsigned char*)HiveRotations2, (unsigned char*)"\xC7\x40\x18\x00\x00\x00\x00", 7);

			if (!moduleMgr->getModule<NoSwing>()->isEnabled())
				g_Data.getLocalPlayer()->swingArm();
		}
		});
}

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

			// Place multiple crystals in a single game tick
			for (int i = 0; i < CrystalPlacementDelay; ++i) {
				placeCrystal(placeMe);
			}

			findCr();

			// Break crystals faster
			breakCrystal(placeMe);
		}

		slotCA = supplies->selectedHotbarSlot;
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