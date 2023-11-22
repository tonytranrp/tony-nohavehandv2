#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

struct CrystalPlacements {
	vec3_t toPlace;
	float enemyDmg = 0.f;
	float selfDmg = 0.f;
};

struct CrystalInfo {
	CrystalPlacements CPlacements;
	C_Entity* ent;
};

struct lineResults {
	float blockCount = 0;
	vec3_t lastSolidBlock;
};
inline bool checkCornerHitboxCollision(const vec3_t& block, C_Entity* ent) {
	if (ent->getEntityTypeId() != 319) {
		for (int i = 0; i < 4; ++i) {
			vec3_t corner(
				i % 2 == 0 ? ent->aabb.lower.x : ent->aabb.upper.x,
				ent->aabb.lower.y,
				(i < 2) ? ent->aabb.lower.z : ent->aabb.upper.z
			);

			// Use epsilon to account for rounding errors
			if ((corner.sub(block)).length() < 0.001f) {
				return true;
			}
		}
	}
	else {
		vec3_t pp = ent->getHumanPos();
		vec3_t entCorners[8] = {
			pp.add(.3f, 0, .3f), pp.add(-.3f, 0, .3f),
			pp.add(.3f, 0, -.3f), pp.add(-.3f, 0, -.3f),
			pp.add(.33541f, 0, 0), pp.add(-.33541f, 0, 0),
			pp.add(0, 0, .33541f), pp.add(0, 0, -.33541f),
		};

		for (const vec3_t& i : entCorners) {
			// Use epsilon to account for rounding errors
			if ((i.sub(block)).length() < 0.001f) {
				return true;
			}
		}
	}

	return false;
}


inline lineResults countBlksAlongLine(vec3_t start, vec3_t end) {
	vec3_t endf = end.floor();
	vec3_t startf = start.floor();

	lineResults rtn;

	if (endf == startf) {
		rtn.blockCount = 0.f;
		rtn.lastSolidBlock = startf;
		return rtn;
	}

	float dist = startf.dist(endf);

	float dX = endf.x - startf.x;
	float dY = endf.y - startf.y;
	float dZ = endf.z - startf.z;

	int steps = 200;
	const float segment = dist / steps;
	const vec3_t seggs3d = vec3_t((dX / steps), (dY / steps), (dZ / steps));

	vec3_t imHere = startf;
	vec3_ti inCoord = startf;

	while (steps-- >= 0) {

		inCoord = imHere.floor();

		if (!g_Data.getLocalPlayer()->region->getBlock(inCoord)->blockLegacy->material->isReplaceable) {
			rtn.blockCount = rtn.blockCount + segment;
			rtn.lastSolidBlock = inCoord.toVector3();
		}

		imHere = imHere.add(seggs3d);
	}
	return rtn;
}

class CrystalAuraWTA : public IModule {
private:
	int origSlot;
	int ctr = 0;

	vec3_t toPlace;

	bool currentlySwitchingHotbar = false;
	float dmg_nukkit = false;

public:
	std::vector<CrystalInfo> CJTWDPlaceArr;
	vec2_t rotAngle;
	bool placeArrEmpty = false;
	bool rotUpNow = false;

	int delay = 0;

	int maxProximity = 4;
	float range = 8;
	float placeRange = 6.f;
	SettingEnum priority;

	float thruWallsR = 6;
	float postWallsR = 10;

	/* SOON
	int lookAhead = 5;
	*/

	SettingEnum calcDmgType;

	bool safetyFirst = false;
	float minHealth = 5.f;
	float maxSelfDmg = 5.f;
	float minEnemDmg = 10.f;

	SettingEnum facePlaceType;

	float fpThresh = 20.f;
	float dmgAtThresh = 3.f;

	bool renderPlacing = true;
	bool noCheckUpper = false;
	bool attackMulti = false;
	SettingEnum switchType;

	SettingEnum rotate;

	/* TODO
	*  when APVPSpoof on, stop crystalling on mine
	*  fix crashing when there is no crystal and apvpspoof is on
	*  steal silent rots from ng
	*/

	/* TODO (in CrystalBreak):
	* anti-weakness
	* anti blocking crystals
	*/

	CrystalAuraWTA();
	~CrystalAuraWTA();

	bool isPlaceValid(vec3_t location, C_Entity* atkObj);
	bool tryRaytrace(vec3_t pos, vec3_t end, C_BlockSource* region);

	float computeExplosionDamage(vec3_t crystalPos, C_Entity* target, C_BlockSource* reg);
	float getBlastDamageEnchantReduction(C_ItemStack* armor);

	std::vector<CrystalPlacements> generateValidPlacements(C_Entity* target, int yOffset);

	CrystalPlacements bestPlaceOnPlane(C_Entity* target, int yOffset);

	CrystalPlacements CrystalAuraJTWD(C_Entity* target);

	bool getCrystalSpoof();

	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onPlayerTick(C_Player* plr) override;

	virtual void onEnable() override;
	virtual void onDisable() override;
	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onSendPacket(Packet* packet);
};

class CrystalBreak : public IModule {
private:
	int oldSlot;

	int Mdel = 0;
	int Rdel = 0;

public:
	vec2_t rotAngleB;
	bool breakArrEmpty = false;

	int aSlot;

	int breakdelay = 0;

	bool retardBreak = false;

	float breakRange = 6.f;

	bool breakAll = true;

	float breakWalls = 10;
	float postBWalls = 10;

	float breakHealth = 5.f;
	float BmaxSelfDmg = 5.f;

	bool antiWeakness = false;

	bool renderBreaking = true;

	SettingEnum rotateBreak;

	bool configHelper = true;

	void sortCrystals();
	void findAFuckingWeapon();

	CrystalBreak();
	~CrystalBreak();

	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onEnable() override;
	virtual void onDisable() override;
	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onLevelRender();
	virtual void onSendPacket(Packet* packet);

};