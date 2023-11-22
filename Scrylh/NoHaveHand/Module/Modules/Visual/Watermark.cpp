#include "Watermark.h"

using namespace std;
Watermark::Watermark() : IModule(0, Category::VISUAL, "Displays the watermark") {
	//registerIntSetting("Opacity", &opacity, opacity, 0, 255);
	//registerBoolSetting("Outline", &outlinec, outlinec);
	shouldHide = true;
}

const char* Watermark::getModuleName() {
	return ("Watermark");
}

void Watermark::onEnable() {
}

void Watermark::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	message = getCustomWatermarkMessage();
	std::string playerName = std::string(g_Data.getLocalPlayer()->getNameTag()->getText());
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr || moduleMgr->getModule<DebugMenu>()->isEnabled()) return;
	auto interfaceMod = moduleMgr->getModule<Interface>();
	auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	if (g_Data.canUseMoveKeys() && !clickGUI->hasOpenedGUI) {
		float cc = 0;
		std::string Release;
		Release = "Paid";
		auto Color = ColorUtil::astolfoRainbow(0, 0);
		auto interfaceColor = ColorUtil::interfaceColor(2);
		DrawUtils::drawText(vec2_t(1, 490), &std::string(message), MC_Color(interfaceColor), 1.f, 1.f, true);
		DrawUtils::drawText(vec2_t(70, 490), &std::string("-"), MC_Color(255, 0, 0, 0), 1.f, 1.f, true);
		DrawUtils::drawText(vec2_t(80, 490), &std::string("V1.3 " + Release), MC_Color(Color), 1.f, 1.f, true);
	}
}
