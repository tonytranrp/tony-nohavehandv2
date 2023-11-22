#include "TargetVisualize.h"

using namespace std;

TargetVisualize::TargetVisualize() : IModule(0, Category::PLAYER, "Displays Target") {
}

const char* TargetVisualize::getModuleName() {
    return "TargetVisualize";
}

static vector<C_Entity*> targetList;

struct CompareTargetEnArray {
    bool operator()(C_Entity* lhs, C_Entity* rhs) {
        C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        return (*lhs->getPos()).dist(*localPlayer->getPos()) < (*rhs->getPos()).dist(*localPlayer->getPos());
    }
};

void findEntitysss(C_Entity* currentEntity, bool isRegularEntity) {
    static auto TargetVisualizes = moduleMgr->getModule<TargetVisualize>();

    if (currentEntity == nullptr) return;
    if (currentEntity == g_Data.getLocalPlayer()) return;
    if (!g_Data.getLocalPlayer()->canAttack(currentEntity, false)) return;
    if (!g_Data.getLocalPlayer()->isAlive()) return;
    if (!currentEntity->isAlive()) return;
    if (currentEntity->getEntityTypeId() == 66) // falling block
        return;
    if (currentEntity->getEntityTypeId() == 69)  // XP
        return;
    if (currentEntity->getEntityTypeId() == 64) // item
        return;
    if (currentEntity->getEntityTypeId() == 80 || currentEntity->getEntityTypeId() == 69) return;
    if (!TargetUtil::isValidTarget(currentEntity)) return;
    if (currentEntity->getEntityTypeId() == 51 || currentEntity->getEntityTypeId() == 1677999) return;

    float dist = (*currentEntity->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

    if (dist < TargetVisualizes->range) {
        targetList.push_back(currentEntity);
        sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
    }
}

void TargetVisualize::onLevelRender() {
    auto player = g_Data.getLocalPlayer();
    if (player == nullptr) return;

    static float n = 0;
    static float anim = 0;
    targetList.clear();

    g_Data.forEachEntity(findEntitysss);

    if (!targetList.empty()) {
        anim += 2; // Increase animation speed
        DrawUtils::setColor(0, 0, 1, 0.9f); // Blue color for the bottom part of the circle

        int numCircles = 5; // Number of circles to draw
        float circleSpacing = 0.05f; // Spacing between circles
        float baseRadius = 0.4f; // Base radius of the circle

        vector<vec3_t> posList; // Declare posList outside the loop
        posList.reserve(56 * numCircles);

        for (int j = 0; j < numCircles; j++) {
            vec3_t permutations[56];
            for (int i = 0; i < 56; i++) {
                float angle = (i * 10.f + anim) * (PI / 180);
                float radius = baseRadius + j * circleSpacing;
                permutations[i] = { sinf(angle) * radius, -0.4f * sinf((anim / 15.f) + (i * 0.15f)), cosf(angle) * radius };
            }

            vec3_t* start = targetList[0]->getPosOld();
            vec3_t* end = targetList[0]->getPos();
            auto te = DrawUtils::getLerpTime();
            vec3_t pos = start->lerp(end, te);

            for (auto& perm : permutations) {
                vec3_t curPos(pos.x, pos.y + perm.y, pos.z);
                posList.push_back(curPos.add(perm));
            }

            // Draw the circles with increasing radii
            DrawUtils::setColor(0, 0, 1, 0.9f);
            DrawUtils::drawLineStrip3D(posList, 5);
        }

        // Draw the top part of the cylinder in fading red
        DrawUtils::setColor(1, 0, 0, 0.2f);
        DrawUtils::drawLineStrip3D(posList, 5);
    }
}

