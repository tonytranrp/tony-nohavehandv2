#include "AutoEz.h"

#include <stdlib.h> /* srand, rand */

#include "../../../../Utils/Utils.h"

AutoEZ::AutoEZ() : IModule(0, Category::OTHER, "troll enemies and anger them2") {
	registerBoolSetting("Lose Message", &this->sayUserDied, this->sayUserDied);
	registerBoolSetting("Win Message", &this->sayEnemDied, this->sayEnemDied);
}

AutoEZ::~AutoEZ() {}

const char* AutoEZ::getModuleName() {
	return ("AutoEZ");
}

static std::vector<C_Entity*> targetList;

void findEnemyPlayer(C_Entity* currentEntity, bool isRegularEntity) {
	if (currentEntity == nullptr)
		return;  // something i just learned: return = terminate func

	if (currentEntity == g_Data.getLocalPlayer())  // Skip Local player
		return;

	if (currentEntity->getEntityTypeId() != 319)  // player
		return;

	float dist = (*currentEntity->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

	if (dist < 12) {
		targetList.push_back(currentEntity);
	}
}

void AutoEZ::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	targetList.clear();

	g_Data.forEachEntity(findEnemyPlayer);

	if (!targetList.empty() && g_Data.isInGame() && sayEnemDied) {  // Send kill messages
		if (GameData::canUseMoveKeys()) {
			// kill msg == switch back at 4  ...

			for (C_Entity* tgt : targetList) {
				vec3_t Pos = targetList[0]->eyePos0;
				Pos.x -= targetList[0]->height + 0;
				Pos.z -= targetList[0]->height + 0;
				Pos.y -= targetList[0]->height + 0;
				std::string enemyname = targetList[0]->getNameTag()->getText();
				clientMessageF("%s Log/Died in %s, %s, %s", enemyname, Pos.x, Pos.y, Pos.z);
				if (tgt->isAlive() && tgt->height > 1.5f) {
					foundPlayer = true;
				}
				if (foundPlayer && !tgt->isAlive() &&  tgt->getNameTag()->getTextLength() > 1) {
					isDead = true;
				}

				if (isDead) {
					C_TextPacket textPacket;
					textPacket.message.setText(killMsg);
					textPacket.sourceName.setText(g_Data.getLocalPlayer()->getNameTag()->getText());
					textPacket.xboxUserId = std::to_string(g_Data.getLocalPlayer()->getUserId());
					g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&textPacket);
					foundPlayer = false;
					isDead = false;
				}
			}
		}
	}
	if (targetList.empty()) {
		foundPlayer = false;
		isDead = false;
	}

	if (g_Data.isInGame() && sayUserDied && antiSpamCheck) {  // Send death messages
		if ((g_Data.getLocalPlayer()->isAlive()) == false) {
			C_TextPacket textPacket;
			textPacket.message.setText(deathMsg);
			textPacket.sourceName.setText(g_Data.getLocalPlayer()->getNameTag()->getText());
			textPacket.xboxUserId = std::to_string(g_Data.getLocalPlayer()->getUserId());
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&textPacket);
			antiSpamCheck = false;
		}
	}

	if (antiSpamCheck == false) {
		if ((g_Data.getLocalPlayer()->isAlive()) == true) {
			antiSpamCheck = true;
		}
	}
}