#pragma once
#include "../../../../Utils/TargetUtil.h"
#include "../../ModuleManager.h"
#include "../Module.h"
class Crystalbreak : public IModule {
public:
    int eRange = 6;
    bool notarget = true;
    Crystalbreak();
    ~Crystalbreak();
    virtual const char* getModuleName() override;
    virtual void onTick(C_GameMode* gm) override;
};