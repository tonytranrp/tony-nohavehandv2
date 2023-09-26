#pragma once
//#include "../../../../Utils/TargetUtil.h"
#include "../../ModuleManager.h"
#include "../Module.h"

class Killaura : public IModule {
private:
    int slotKA = 0;
    void findWeapon();
    int delayka = 0;
    int Odelay = 0;
public:
    bool hurttimeka = false;
    bool targetListEmpty = true;
    bool visualize = true;
    bool swingKA = true;
    bool hold = false;
    bool mobs = false;

    float range = 8.f;

    vec2_t tro;

    std::string name = "Killaura";
    SettingEnum rotations = this;
    SettingEnum mode = this;
    SettingEnum AutoWeapon = this;

    Killaura();

    // Inherited via IModule
    virtual void onSendPacket(Packet* packet);
    virtual void onPlayerTick(C_Player* plr);
    virtual const char* getRawModuleName();
    virtual const char* getModuleName();
    virtual void onTick(C_GameMode* gm);
    virtual void onLevelRender();
    virtual void onDisable();
    virtual void onEnable();
};