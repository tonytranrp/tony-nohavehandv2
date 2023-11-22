#include "CombatInfo.h"

CombatInfo::CombatInfo() : IModule(0, Category::VISUAL, "ConbatHack Display") {
	registerFloatSetting("X", &this->CombatInfoX, this->CombatInfoX, 0.f, g_Data.getClientInstance()->getGuiData()->windowSize.x);
	registerFloatSetting("Y", &this->CombatInfoY, this->CombatInfoY, 0.f, g_Data.getClientInstance()->getGuiData()->windowSize.y);
	registerFloatSetting("Scale", &this->scale, this->scale, 0.5f, 1.5f);
}

CombatInfo::~CombatInfo() {
}

const char* CombatInfo::getModuleName() {
	return ("ModuleInfo");
}

void CombatInfo::onMove(C_MoveInputHandler* input) {
}

void CombatInfo::onPreRender(C_MinecraftUIRenderContext* renderCtx) {
		float yVal = CombatInfoY;
		float xVal = CombatInfoX;
		auto kam = moduleMgr->getModule<Killaura>();
		auto srm = moduleMgr->getModule<Surround>();
		auto acm = moduleMgr->getModule<AntiCrystal>();
		auto cbm = moduleMgr->getModule<Crystalbreak>();
		
		auto xtxm = moduleMgr->getModule<CrystalAuraWTA>();
		auto aufm = moduleMgr->getModule<AutoFallBlock>();
		auto hkm = moduleMgr->getModule<HoleKick>();
		auto alm = moduleMgr->getModule<AutoLog>();
		auto Flym = moduleMgr->getModule<FlyBypass>();
		auto hfm = moduleMgr->getModule<HoleFill>();
		auto abm = moduleMgr->getModule<AutoButton>();
		std::string ka = "KA: ";
		std::string sr = "SR: ";
		std::string ac = "AC: ";
		std::string cb = "CB: ";
		std::string wta = "WTA: ";
		std::string xtx = "XTX: ";
		std::string auf = "AUF: ";
		std::string hk = "HK: ";
		std::string Fly = "2EF+: ";
		std::string al = "AL: ";
		std::string hf = "HF: ";
		std::string ab = "AB: ";
		std::string disable = "OFF";
		std::string enable = "ON";
		float kaw = DrawUtils::getTextWidth(&ka, scale);
		float srw = DrawUtils::getTextWidth(&sr, scale);
		float acw = DrawUtils::getTextWidth(&ac, scale);
		float cbw = DrawUtils::getTextWidth(&cb, scale);
		float wtaw = DrawUtils::getTextWidth(&wta, scale);
		float xtxw = DrawUtils::getTextWidth(&xtx, scale);
		float hkw = DrawUtils::getTextWidth(&hk, scale);
		float aufw = DrawUtils::getTextWidth(&auf, scale);
		float Flyw = DrawUtils::getTextWidth(&Fly, scale);
		float atw = DrawUtils::getTextWidth(&al, scale);
		float hfw = DrawUtils::getTextWidth(&hf, scale);
		float abw = DrawUtils::getTextWidth(&ab, scale);

		auto interfaceColor = ColorUtil::interfaceColor(1);
		DrawUtils::drawText(vec2_t(CombatInfoX, CombatInfoY), &ka, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + kaw, CombatInfoY), kam->isEnabled() ? &enable : &disable, kam->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX, CombatInfoY + 10.f), &sr, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + srw, CombatInfoY + 10.f), srm->isEnabled() ? &enable : &disable, srm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX, CombatInfoY + 20.f), &ac, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + acw, CombatInfoY + 20.f), acm->isEnabled() ? &enable : &disable, acm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX, CombatInfoY + 30.f), &cb, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + cbw, CombatInfoY + 30.f), cbm->isEnabled() ? &enable : &disable, cbm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);

		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f, CombatInfoY), &wta, MC_Color(243,117,180), scale);
		
		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f, CombatInfoY + 10.f), &xtx, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f + xtxw, CombatInfoY + 10.f), xtxm->isEnabled() ? &enable : &disable, xtxm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f, CombatInfoY + 20.f), &hk, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f + hkw , CombatInfoY + 20.f), hkm->isEnabled() ? &enable : &disable, hkm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f, CombatInfoY + 30.f), &auf, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 50.f + aufw , CombatInfoY + 30.f), aufm->isEnabled() ? &enable : &disable, aufm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);

		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f, CombatInfoY), &Fly, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f + Flyw, CombatInfoY), Flym->isEnabled() ? &enable : &disable, Flym->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f, CombatInfoY + 10.f), &al, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f + atw, CombatInfoY + 10.f), alm->isEnabled() ? &enable : &disable, alm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f, CombatInfoY + 20.f), &hk, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f + hkw, CombatInfoY + 20.f), hkm->isEnabled() ? &enable : &disable, hkm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f, CombatInfoY + 30.f), &ab, MC_Color(243,117,180), scale);
		DrawUtils::drawText(vec2_t(CombatInfoX + 100.f + abw, CombatInfoY + 30.f), abm->isEnabled() ? &enable : &disable, abm->isEnabled() ? MC_Color(0, 255, 0) : MC_Color(255, 0, 0), scale);


}
void CombatInfo::onDisable() {
	auto player = g_Data.getLocalPlayer();
}
