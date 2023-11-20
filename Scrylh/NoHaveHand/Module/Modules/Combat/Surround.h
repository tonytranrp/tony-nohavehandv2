
#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class Surround : public IModule {
private:
	int delay = 0;
	bool anticity = true;
	bool citySwitch = true;

public:
	inline bool checkCornerHitboxCollision(vec3_t* block, C_Entity* ent) {
		std::vector<vec3_t> corners;
		corners.clear();

		corners.push_back(vec3_t(ent->aabb.lower.x, ent->aabb.lower.y, ent->aabb.lower.z));
		corners.push_back(vec3_t(ent->aabb.lower.x, ent->aabb.lower.y, ent->aabb.upper.z));
		corners.push_back(vec3_t(ent->aabb.upper.x, ent->aabb.lower.y, ent->aabb.upper.z));
		corners.push_back(vec3_t(ent->aabb.upper.x, ent->aabb.lower.y, ent->aabb.lower.z));

		if (ent->getEntityTypeId() != 319) {
			if (!corners.empty()) {
				for (auto corner : corners) {
					if ((floor(corner.x) == floor(block->x)) && (floor(corner.y) == floor(block->y)) && (floor(corner.z) == floor(block->z))) {
						return true;
					}
				}
			}
		}
		else {
			vec3_t pp = ent->getHumanPos();
			vec3_t entCorners[8] = {
				pp.add(.3f, 0, .3f),
				pp.add(-.3f, 0, .3f),
				pp.add(.3f, 0, -.3f),
				pp.add(-.3f, 0, -.3f),
				pp.add(.33541f, 0, 0),
				pp.add(-.33541f, 0, 0),
				pp.add(0, 0, .33541f),
				pp.add(0, 0, -.33541f),
			};

			for (vec3_t i : entCorners) {
				// Check if crystal placement position is inside the target's hitbox
				if (block->x >= ent->aabb.lower.x && block->x <= ent->aabb.upper.x &&
					block->y >= ent->aabb.lower.y && block->y <= ent->aabb.upper.y &&
					block->z >= ent->aabb.lower.z && block->z <= ent->aabb.upper.z) {
					// Adjust the crystal placement position to avoid intersection
					block->y = ent->aabb.upper.y + 1.0f;
				}

				if (i.floor() == *block) {
					return true;
				}
			}
		}

		return false;
	}
	int origSlot;
	SettingEnum enum1;

	SettingEnum type;
	bool renderSurround = true;
	bool buttonplace = true;
	SettingEnum buttonidchoser;
	bool doAirplace = false;
	bool selftrap = false;
	//SettingEnum rotate;  // Rotations (none->obvious
						 //			  normal->clientside rotate
						 //			  silent->sends packet to go look at the crystal
						 //			  pitchUp->so that shit works on APVP

	Surround();
	~Surround();
	bool blockSwap = true;
	int prevSlot3 = -1;
	vec2_t rotAngle;
	bool pn = false;

	bool jumpDisable = false;
	bool disableOnComplete2 = true;
	bool center = true;
	bool spoof = true;

	std::vector<vec3_t> regularBlocks = {

		vec3_t(0, -1, 0),

		vec3_t(1, -1, 0),
		vec3_t(1, 0, 0),

		vec3_t(0, -1, 1),
		vec3_t(0, 0, 1),

		vec3_t(-1, -1, 0),
		vec3_t(-1, 0, 0),

		vec3_t(0, -1, -1),
		vec3_t(0, 0, -1),

	};

	std::vector<vec3_t> sidesBlocksOnly = {
		vec3_t(1, 0, 0),
		vec3_t(0, 0, 1),
		vec3_t(-1, 0, 0),
		vec3_t(0, 0, -1),
		vec3_t(0, -1, 0),
	};


	//void getObby();
	bool isSideEmpty();
	void onMove(C_MoveInputHandler* hand);
	//vec3_t playerPosWithFC();
	// Inherited via IModule
	void findAndPlaceBlocks(const vec3_t& playerPos, const vec3_t& targetPos, C_Inventory* inv, int itemId);
	virtual const char* getModuleName() override;
	//virtual bool isHoldMode() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
	//void onSendPacket(Packet* packet);
	//virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onEnable();
	//virtual void onMove(C_MoveInputHandler* hand) override;
};