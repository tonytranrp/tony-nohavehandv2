#include "PlayerList2.h"
#include "../../../../Utils/DrawUtils.h"
#include "../../ModuleManager.h"

PlayerListx::PlayerListx() : IModule(0x0, Category::VISUAL, "draws a list of nearby players next to the array list") {
	registerBoolSetting("Show Armour", &this->armour, this->armour);
    
}

PlayerListx::~PlayerListx() {
}

const char* PlayerListx::getModuleName() {
	return ("PlayerList");
}

struct CompareTargetEnArray {
	bool operator()(C_Entity* lhs, C_Entity* rhs) {
		C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
		return (*lhs->getPos()).dist(*localPlayer->getPos()) < (*rhs->getPos()).dist(*localPlayer->getPos());
	}
};


static std::vector<C_Entity*> playerListx;

void findPlayerscc(C_Entity* currentEntity, bool isRegularEntity) {
	if (currentEntity == nullptr) return;
	if (!g_Data.getLocalPlayer()->canAttack(currentEntity, false)) return;
	if (!g_Data.getLocalPlayer()->isAlive()) return;
	if (!currentEntity->isAlive()) return;
	if (currentEntity->getEntityTypeId() != 319) return;
	if (currentEntity->getNameTag()->getTextLength() <= 1) return;
	if (currentEntity->isLocalPlayer()) return;

	playerListx.push_back(currentEntity);
	std::sort(playerListx.begin(), playerListx.end(), CompareTargetEnArray());
}

void PlayerListx::onEnable() {
	playerListx.clear();
}

void PlayerListx::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	
	static auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;


	if ((g_Data.getLocalPlayer() != nullptr) && g_Data.canUseMoveKeys()) {
		int runs2 = 0;
		playerListx.clear();
		g_Data.forEachEntity(findPlayerscc);


        if (!playerListx.empty()) {
            for (auto& ye : playerListx) {
                vec3_t* currPos = ye->getPos();
                char eeeee = *ye->getNameTag()->getText();
                std::string why = ye->getNameTag()->getText() + std::string(" (") + std::to_string((int)floorf(currPos->x)) + "," + std::to_string((int)floorf(currPos->y - 1)) + "," + std::to_string((int)floorf(currPos->z)) + std::string(")");

                why = Utils::sanitize(why); why = why.substr(0, why.find('\n'));
                std::string* isThisSoBroken = &why;

                vec2_t testdx = vec2_t(windowSize.x - 162.5f, 10.f + (10.f * runs2)); // Adjusted y position

                DrawUtils::drawText(vec2_t(windowSize.x - 160.f, 10.f + (10.f * runs2)), isThisSoBroken, MC_Color(122, 224, 202), 1.f, 1.f); // Adjusted y position

                if (ye->damageTime > 1) {
                    DrawUtils::drawRectangle(vec4_t(testdx.x - 7.5, testdx.y, testdx.x, testdx.y + 7.5), MC_Color(255, 0, 0), 1.f, 1.f);
                  
                }
                else {
                    DrawUtils::drawRectangle(vec4_t(testdx.x - 7.5, testdx.y, testdx.x, testdx.y + 7.5), MC_Color(0, 255, 0), 1.f, 1.f);
                    
                }

                if (armour) {
                    static float constexpr scale2 = 0.6f;
                    static float constexpr opacity2 = 0.25f;
                    static float constexpr spacing2 = scale2 + 10.f;
                    float x2 = windowSize.x - 210.f;
                    float y2 = 10.f;
                    y2 = 10.f + (10.f * runs2); // Adjusted y position

                    for (int i2 = 0; i2 < 4; i2++) {
                        C_ItemStack* stack2 = ye->getArmor(i2);
                        if (stack2->item != nullptr) {
                            DrawUtils::drawItem(stack2, vec2_t(x2, y2), opacity2, scale2, false);
                            x2 += scale * spacing2;
                        }
                    }
                }
                runs2++;
            }
        }


	}
}
void PlayerListx::onDisable() {
	playerListx.clear();
}
