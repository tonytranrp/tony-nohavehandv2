#include "../pch.h"
#include "BindList.h"
#include "../../../../Utils/DrawUtils.h"
#include "../../ModuleManager.h"

BindList::BindList() : IModule(0x0, Category::VISUAL, "Bind List") {
}

BindList::~BindList() {
}

const char* BindList::getModuleName() {
	return ("BindList");
}

void BindList::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
	auto player = g_Data.getLocalPlayer();
	auto color_A873HFA = ColorUtil::interfaceColor(1);
	auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	auto Mods = std::shared_ptr<IModule>();
	if (player == nullptr) return;
	float positionX = windowSize.x;
	float positionY = 0.f;
	positionX = windowSize.x;
	positionY = 0;
	std::string moduleName;
	const char* name = "Module";
	int keybind;
	this->keybind = Mods->getKeybind();
	name = Mods->getModuleName();
	moduleName = name;
	if ((g_Data.getLocalPlayer() != nullptr) && g_Data.canUseMoveKeys() && !clickGUI->hasOpenedGUI) {
		if (keybind != 0x0) {
			char text[50];
			std::string txts = name + std::string(GREEN) + std::string(Utils::getKeybindName(keybind));
			float keywit = DrawUtils::getTextWidth(&txts, 1.f);
			float techai = DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight() * 1.f;
			DrawUtils::drawText(vec2_t(1, 480), &txts, MC_Color(color_A873HFA), 1.f, 1.f, true);
		}
	}
}
