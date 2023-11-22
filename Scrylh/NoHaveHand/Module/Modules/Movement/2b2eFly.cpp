#include "2b2eFly.h"
#include <chrono>

FlyBypass::FlyBypass() : IModule(70, Category::MOVEMENT, "2b2e-fly, dev by tony") {
    registerFloatSetting("speed", &speed, speed, 0.f, 0.50f);
    
    enabledTicks = 0;
}

FlyBypass::~FlyBypass() {
}

const char* FlyBypass::getModuleName() {
    return "2B2E-Fly+";
}

void FlyBypass::onEnable() {
    auto player = g_Data.getLocalPlayer();
    player->animateHurt();
    enabledTicks = 0;
}

void FlyBypass::onTick(C_GameMode* gm) {
    glideModEffective = glideMod;
    C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();

    if (g_Data.canUseMoveKeys()) {
        if (GameData::isKeyDown(*input->spaceBarKey))
            glideModEffective += 0.0f;
        if (GameData::isKeyDown(*input->sneakKey))
            glideModEffective -= 0.0f;
    }
    gm->player->velocity.y = glideModEffective;
    if (!gm->player->isSprinting() && gm->player->velocity.magnitudexz() > 0.01f) {
        C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
        gm->player->setSprinting(true);
    }

    // Increase the enabled ticks
    enabledTicks += 1;

    // Check if the enabled ticks have exceeded the desired duration (10 ticks = 0.5 seconds)
    if (enabledTicks > 45) {
        this->setEnabled(false);
    }
}

void FlyBypass::onMove(C_MoveInputHandler* input) {
    *g_Data.getClientInstance()->minecraft->timer = 22.5f;
    auto player = g_Data.getLocalPlayer();
    if (player == nullptr) return;

    if (player->isInLava() == 1 || player->isInWater() == 1)
        return;

    if (player->isSneaking())
        return;

    float yaw = player->yaw;

    bool pressed = input->forward || input->backward || input->right || input->left;

    if (input->forward && input->backward)
        return;

    if (input->right) {
        yaw += 90.f;
        if (input->forward)
            yaw -= 45.f;
        else if (input->backward)
            yaw += 45.f;
    }
    if (input->left) {
        yaw -= 90.f;
        if (input->forward)
            yaw += 45.f;
        else if (input->backward)
            yaw -= 45.f;
    }

    if (input->backward && !input->left && !input->right)
        yaw += 180.f;

    float calcYaw = (yaw + 90) * (PI / 180);
    vec3_t moveVec;
    moveVec.x = cos(calcYaw) * speed;
    moveVec.y = player->velocity.y;
    moveVec.z = sin(calcYaw) * speed;
    if (pressed) player->lerpMotion(moveVec);
}

void FlyBypass::onDisable() {
    auto player = g_Data.getLocalPlayer();
    g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f); // Adjust the timer value as needed
}
