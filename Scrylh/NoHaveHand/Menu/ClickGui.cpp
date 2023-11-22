#include "ClickGui.h"

#include <Windows.h>
#include "../../Utils/Json.hpp"
#include "../../Utils/Logger.h"
using namespace std;
bool isLeftClickDown = false;
bool isRightClickDown = false;
bool shouldToggleLeftClick = false;  // If true, toggle the focused module
bool shouldToggleRightClick = false;
bool resetStartPos = true;
bool initialised = false;
int scrollingDirection = 0;

struct SavedWindowSettings {
	vec2_t pos = { -1, -1 };
	bool isExtended = true;
	const char* name = "";
};

map<unsigned int, shared_ptr<ClickWindow>> windowMap;
map<unsigned int, SavedWindowSettings> savedWindowSettings;

bool isDragging = false;
unsigned int draggedWindow = -1;
vec2_t dragStart = vec2_t();

unsigned int focusedElement = -1;
bool isFocused = false;

//static constexpr float textPadding = 2.0f;
static constexpr float textPadding = 1.0f;
static constexpr float textSize = 1.0f;
static constexpr float textHeight = textSize * 9.5f;
static constexpr float categoryMargin = 0.5f;
static const MC_Color moduleColor = MC_Color(0, 0, 0);
static constexpr float paddingRight = 13.5f;
static constexpr float crossSize = textHeight / 2.f;
static constexpr float crossWidth = 0.2f;
static const MC_Color whiteColor = MC_Color(245, 240, 217);
static const MC_Color brightModuleBlendColor = MC_Color(24, 175, 216);

//static float rcolors[4];  // Rainbow color array RGBA

float currentYOffset = 0;
float currentXOffset = 0;

int timesRendered = 0;

void ClickGui::getModuleListByCategory(Category category, vector<shared_ptr<IModule>>* modList) {
	auto lock = moduleMgr->lockModuleList();
	vector<shared_ptr<IModule>>* moduleList = moduleMgr->getModuleList();

	for (auto& it : *moduleList) {
		if (it->getCategory() == category) modList->push_back(it);
	}
}

shared_ptr<ClickWindow> ClickGui::getWindow(const char* name) {
	unsigned int id = Utils::getCrcHash(name);

	auto search = windowMap.find(id);
	if (search != windowMap.end()) {  // Window exists already
		return search->second;
	}
	else {  // Create window
		// TODO: restore settings for position etc
		shared_ptr<ClickWindow> newWindow = make_shared<ClickWindow>();
		newWindow->name = name;

		auto savedSearch = savedWindowSettings.find(id);
		if (savedSearch != savedWindowSettings.end()) {  // Use values from config
			newWindow->isExtended = savedSearch->second.isExtended;
			if (savedSearch->second.pos.x > 0) newWindow->pos = savedSearch->second.pos;
		}

		windowMap.insert(make_pair(id, newWindow));
		return newWindow;
	}
}

shared_ptr<ClickModule> ClickGui::getClickModule(shared_ptr<ClickWindow> window, const char* name) {
	unsigned int id = Utils::getCrcHash(name);

	auto search = window->moduleMap.find(id);
	if (search != window->moduleMap.end()) {  // Window exists already
		return search->second;
	}
	else {  // Create window
		// TODO: restore settings for position etc
		shared_ptr<ClickModule> newModule = make_shared<ClickModule>();

		window->moduleMap.insert(make_pair(id, newModule));
		return newModule;
	}
}

void ClickGui::renderLabel(const char* text) { }

void ClickGui::renderTooltip(std::string* text, vec2_t mousePos) {
	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
	vec2_t currentTooltipPos = vec2_t(mousePos.x + 5.f, mousePos.y - 15.f);

	// Convert the const reference to a pointer before passing it to getTextWidth
	std::string tooltipText = *text;
	float textWidth = DrawUtils::getTextWidth(&tooltipText, textSize);  // Pass the pointer here
	vec2_t textPos = vec2_t(currentTooltipPos.x + textPadding, currentTooltipPos.y);
	vec4_t rectPos = vec4_t(
		currentTooltipPos.x - 2.f,
		currentTooltipPos.y - 2.f,
		currentTooltipPos.x + (textPadding * 2) + textWidth + 2.f,
		currentTooltipPos.y + textHeight + 2.f);

	// tooltips
	DrawUtils::fillRectangle(rectPos, moduleColor, 1.0f);                  // tooltips
	DrawUtils::drawRectangle(rectPos, brightModuleBlendColor, 1.f, 0.5f);  // tooltips
	DrawUtils::drawText(textPos, text, whiteColor, textSize);
}


// Assuming you have a function to get the mouse position in your Minecraft Bedrock Edition modding framework
vec2_t getMousePosition() {
	// Implement this function to get the current mouse position in the game
	// Return the mouse position as a vec2_t
	return vec2_t(0.f, 0.f);  // Replace with the actual mouse position
}

#pragma region Packet
void ClickGui::renderPacketCategory(Category category, MC_Color categoryColor) {
	static constexpr float textHeight = textSize * 10.f;
	const char* categoryName = ClickGui::catToName(category);
	static auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	const std::shared_ptr<ClickWindow> ourWindow = getWindow(categoryName);

	// Reset Windows to pre-set positions to avoid confusion
	if (resetStartPos && ourWindow->pos.x <= 0) {
		float yot = g_Data.getGuiData()->windowSize.x;
		ourWindow->pos.y = 4;
		switch (category) {
		case Category::COMBAT:
			ourWindow->pos.x = yot / 7.5f;
			break;
		case Category::VISUAL:
			ourWindow->pos.x = yot / 4.1f;
			break;
		case Category::MOVEMENT:
			ourWindow->pos.x = yot / 5.6f * 2.f;
			break;
		case Category::PLAYER:
			ourWindow->pos.x = yot / 4.25f * 2.f;
			break;
		case Category::EXPLOIT:
			ourWindow->pos.x = yot / 3.4f * 2;
			break;
		case Category::OTHER:
			ourWindow->pos.x = yot / 2.9f * 2.05f;
			break;
		case Category::UNUSED:
			ourWindow->pos.x = yot / 1.6f * 2.2f;
			break;
		case Category::CUSTOM:
			ourWindow->pos.x = yot / 5.6f * 2.f;
			break;
		}
	}

	// Set brightness based on a variable (e.g., brightnessValue)
	// You can change the value of brightnessValue (0 to 1) to make the window brighter or darker
	float brightnessValue = 0.7f; // You can adjust this value as needed
	MC_Color brightenedColor = MC_Color(
		static_cast<int>(categoryColor.r * brightnessValue),
		static_cast<int>(categoryColor.g * brightnessValue),
		static_cast<int>(categoryColor.b * brightnessValue),
		categoryColor.a
	);
	if (clickGUI->openAnim < 27) ourWindow->pos.y = clickGUI->openAnim;

	const float xOffset = ourWindow->pos.x;
	const float yOffset = ourWindow->pos.y;
	vec2_t* windowSize = &ourWindow->size;
	currentXOffset = xOffset;
	currentYOffset = yOffset;

	// Get All Modules in our category
	std::vector<std::shared_ptr<IModule>> ModuleList;
	getModuleListByCategory(category, &ModuleList);

	// Get max width of all text
	{
		for (auto& it : ModuleList) {
			auto intf = moduleMgr->getModule<Interface>();
			//std::string label = "-------------";
			//std::string label = "--------------";
			std::string label = "--------------";
			windowSize->x = fmax(windowSize->x, DrawUtils::getTextWidth(&string(label), textSize, Fonts::SMOOTH));
		}
	}

	vector<shared_ptr<IModule>> moduleList;
	/*{
		vector<int> toIgniore;
		int moduleCount = (int)ModuleList.size();
		for (int i = 0; i < moduleCount; i++) {
			float bestWidth = 1.f;
			int bestIndex = 1;

			for (int j = 0; j < ModuleList.size(); j++) {
				bool stop = false;
				for (int bruhwth = 0; bruhwth < toIgniore.size(); bruhwth++)
					if (j == toIgniore[bruhwth]) {
						stop = true;
						break;
					}
				if (stop)
					continue;

				string t = ModuleList[j]->getRawModuleName();
				float textWidthRn = DrawUtils::getTextWidth(&t, textSize, Fonts::SMOOTH);
				if (textWidthRn > bestWidth) {
					bestWidth = textWidthRn;
					bestIndex = j;
				}
			}
			moduleList.push_back(ModuleList[bestIndex]);
			toIgniore.push_back(bestIndex);
		}
	}
	std::sort(moduleList.begin(), moduleList.end(), [](auto lhs, auto rhs) {
		auto current = lhs;
		auto other = rhs;
		return std::string{ *current->getRawModuleName() } < std::string{ *other->getRawModuleName() };
	});
	*/
	getModuleListByCategory(category, &moduleList);
	const float xEnd = currentXOffset + windowSize->x + paddingRight;

	vec2_t mousePos = *g_Data.getClientInstance()->getMousePos();
	{
		vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
		vec2_t windowSizeReal = g_Data.getClientInstance()->getGuiData()->windowSizeReal;
		mousePos = mousePos.div(windowSizeReal);
		mousePos = mousePos.mul(windowSize);
	}

	float categoryHeaderYOffset = currentYOffset;

	if (ourWindow->isInAnimation) {
		if (ourWindow->isExtended) {
			clickGUI->animation *= 0.85f;
			if (clickGUI->animation < 0.001f) {
				ourWindow->yOffset = 0;
				ourWindow->isInAnimation = false;
			}

		}
		else {
			clickGUI->animation = 1 - ((1 - clickGUI->animation) * 0.85f);
			if (1 - clickGUI->animation < 0.001f) ourWindow->isInAnimation = false;
		}
	}

	currentYOffset += textHeight + (textPadding * 2) + 2.f;
	if (ourWindow->isExtended) {
		if (ourWindow->isInAnimation) {
			currentYOffset -= clickGUI->animation * moduleList.size() * (textHeight + (textPadding * 2));
		}

		bool overflowing = false;
		const float cutoffHeight = roundf(g_Data.getGuiData()->heightGame * 0.75f) + 0.5f /*fix flickering related to rounding errors*/;
		int moduleIndex = 0;
		for (auto& mod : moduleList) {
			moduleIndex++;
			if (moduleIndex < ourWindow->yOffset) continue;
			float probableYOffset = (moduleIndex - ourWindow->yOffset) * (textHeight + (textPadding * 2));

			if (ourWindow->isInAnimation) { // Estimate, we don't know about module settings yet
				if (probableYOffset > cutoffHeight) {
					overflowing = true;
					break;
				}
			}
			else if ((currentYOffset - ourWindow->pos.y) > cutoffHeight || currentYOffset > g_Data.getGuiData()->heightGame - 5) {
				overflowing = true;
				break;
			}

			std::string ncdb = "LOL";
			std::string textStr = mod->getRawModuleName();
			if (mod->getKeybind() != 0x0) {
				char textgf[50];
				//printf_s(textgf, 50, "%s", std::string(std::string(GRAY) + " [" + std::string(WHITE) + std::string(Utils::getKeybindName(mod->getKeybind())) + std::string(GRAY) + "]").c_str());
				sprintf_s(textgf, 50, "%s", ("[" + std::string(Utils::getKeybindName(mod->getKeybind()))  + "]").c_str());
				//moduleName = text;
				ncdb = textgf;
			}

			vec2_t textPos = vec2_t(currentXOffset + textPadding + 45, currentYOffset + textPadding + 5);
			vec4_t rectPos = vec4_t(
				currentXOffset, currentYOffset - 2.f, xEnd,
				currentYOffset + textHeight + (textPadding * 2));
			/*
			vec4_t rectPos2 = vec4_t(
				currentXOffset - 2, currentYOffset - 1, currentXOffset,
				currentYOffset + textHeight + (textPadding * 2) + 1);
			vec4_t rectPos3 = vec4_t(
				xEnd, currentYOffset - 1, xEnd + 2,
				currentYOffset + textHeight + (textPadding * 2) + 1);
				*/
			bool allowRender = currentYOffset >= categoryHeaderYOffset;

			// Background
			if (allowRender) {
				if (!ourWindow->isInAnimation && !isDragging && rectPos.contains(&mousePos)) {
					static auto clickGuiMod = moduleMgr->getModule<ClickGUIMod>();
					DrawUtils::fillRectangle(rectPos, MC_Color(206, 194, 194), 0.2f);  // hovering

					std::string tooltip = mod->getTooltip();
					if (clickGuiMod->showTooltips && !tooltip.empty()) {
						renderTooltip(&tooltip, mousePos);  // Pass the tooltip by reference
					}

					if (shouldToggleLeftClick && !ourWindow->isInAnimation) {  // Are we being clicked?
						mod->toggle();
						shouldToggleLeftClick = false;
					}
				}
				else {
					
				}
			}

			// Text
			//if (allowRender) DrawUtils::drawCenteredString(textPos, &textStr, textSize, mod->isEnabled() ? MC_Color(255, 255, 255) : MC_Color(255, 255, 255), true);
			//DrawUtils::getTextWidth(&textStr)

			//vec2_t textPosFix = vec2_t(currentXOffset + textPadding + 0.5f, currentYOffset + textPadding + 0.5f);

			float ncdb4 = DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight() * 1.f;
			vec2_t textPosFix = vec2_t(currentXOffset + textPadding + 2.f, (rectPos.y + rectPos.w) / 2 - ncdb4 / 2 + 1.5f);
			if (allowRender) {
				if (clickGUI->ModuleOutline) {
					DrawUtils::drawRectangle((vec4_t(rectPos.x + 1, rectPos.y + 1, rectPos.z - 1, rectPos.w - 1)), MC_Color(0.f, 0.f, 0.f), 0.5f);
				}

				//DrawUtils::drawRectangle(vec4_t(rectPos.x + 1, rectPos.y + 1, rectPos.z - 1, rectPos.w - 1), MC_Color(25, 25, 25, 180), 0.5f);
				//DrawUtils::drawText(textPosFix, &textStr, MC_Color(255, 255, 255), 1.f, 1.f, true);
				DrawUtils::drawText(textPosFix, &textStr, mod->isEnabled() ? MC_Color(255, 255, 255) : MC_Color(175, 175, 175), 1.f, 1.f, true);
				/*
				if (mod->getKeybind() != 0x0) {
					float ncdb2 = DrawUtils::getTextWidth(&ncdb, 0.75f);
					float ncdb3 = DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight() * 0.75f;
					vec2_t ncdbpos = vec2_t(rectPos.z - ncdb2 - 9.5f, (rectPos.y + rectPos.w) / 2 - ncdb3 / 2 + 1.5f);
					DrawUtils::drawText(ncdbpos, &ncdb, MC_Color(255, 255, 255), 0.75f, 1.f, true);
				}
				*/
			}

			// Settings
			{
				std::vector<SettingEntry*>* settings = mod->getSettings();
				if (settings->size() > 2 && allowRender) {
					std::shared_ptr<ClickModule> clickMod = getClickModule(ourWindow, mod->getRawModuleName());
					if (rectPos.contains(&mousePos) && shouldToggleRightClick && !ourWindow->isInAnimation) {
						if (clickGUI->sounds) {
							auto player = g_Data.getLocalPlayer();
							PointingStruct* level = g_Data.getLocalPlayer()->pointingStruct;
							level->playSound("random.click", *player->getPos(), 1, 1);
						}
						shouldToggleRightClick = false;
						clickMod->isExtended = !clickMod->isExtended;
					}
					if (clickGUI->Crossline) {
						GuiUtils::drawCrossLine(vec2_t(
							currentXOffset + windowSize->x + paddingRight - (crossSize / 2) - 3.f,
							currentYOffset + textPadding + (textHeight / 2) - 1),
							mod->isEnabled() ? MC_Color(255, 255, 255) : MC_Color(175, 175, 175), 0.5f, crossSize, !clickMod->isExtended);
					}
					float dc1 = DrawUtils::getTextWidth(&std::string(">"), 0.75f);
					float dc2 = DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight() * 0.75f;
					vec2_t ncdbpos = vec2_t(rectPos.z - dc1 - 5, (rectPos.y + rectPos.w) / 2 - dc2 / 2 + 1.5f);
					currentYOffset += textHeight + (textPadding * 2) + 2.f;

					if (clickMod->isExtended) {
						float startYOffset = currentYOffset;
						currentYOffset = currentYOffset - 0.5f;
						for (auto setting : *settings) {
							if (strcmp(setting->name, "enabled") == 0 || strcmp(setting->name, "keybind") == 0) continue;

							vec2_t textPos = vec2_t(currentXOffset + textPadding + 6, currentYOffset + textPadding);
							vec4_t rectPos = vec4_t(currentXOffset + 1, currentYOffset, xEnd - 1, 0);

							if ((currentYOffset - ourWindow->pos.y) > cutoffHeight) {
								overflowing = true;
								break;
							}

							string len = "saturation  ";
							switch (setting->valueType) {
							case ValueType::BOOL_T: {
								auto interfaceColor = ColorUtil::interfaceColor(1);
								rectPos.w = currentYOffset + textHeight + (textPadding * 2);
								DrawUtils::fillRectangleA(rectPos, MC_Color(0, 0, 0, 90));

								vec4_t selectableSurface = vec4_t(
									rectPos.x + textPadding,
									rectPos.y + textPadding,
									rectPos.z + textHeight - textPadding - 15,
									rectPos.y + textHeight - textPadding);

								bool isFocused = selectableSurface.contains(&mousePos);
								// Logic
								{
									if (isFocused && shouldToggleLeftClick && !ourWindow->isInAnimation) {
										shouldToggleLeftClick = false;
										setting->value->_bool = !setting->value->_bool;
										if (clickGUI->sounds) {
											auto player = g_Data.getLocalPlayer();
											PointingStruct* level = g_Data.getLocalPlayer()->pointingStruct;
											level->playSound("random.click", *player->getPos(), 1, 1);
										}
									}
								}
								// Checkbox
								{
									vec4_t boxPos = vec4_t(
										rectPos.z + textPadding - 15,
										rectPos.y + textPadding + 2,
										rectPos.z + textHeight - textPadding - 15,
										rectPos.y + textHeight - textPadding + 2);

									DrawUtils::drawRectangle(boxPos, MC_Color(255, 255, 255), isFocused ? 1 : 0.8f, 0.5f);

									if (setting->value->_bool) {
										DrawUtils::setColor(28, 107, 201, 1);
										boxPos.x += textPadding;
										boxPos.y += textPadding;
										boxPos.z -= textPadding;
										boxPos.w -= textPadding;
										DrawUtils::drawLine(vec2_t(boxPos.x, boxPos.y), vec2_t(boxPos.z, boxPos.w), 0.5f);
										DrawUtils::drawLine(vec2_t(boxPos.z, boxPos.y), vec2_t(boxPos.x, boxPos.w), 0.5f);
									}
								}

								// Text
								{
									// Convert first letter to uppercase for more friendlieness
									char name[0x21]; sprintf_s(name, 0x21, "%s", setting->name); if (name[0] != 0) name[0] = toupper(name[0]);

									string elTexto = name;

									windowSize->x = fmax(windowSize->x, DrawUtils::getTextWidth(&len, textSize) + 16);
									DrawUtils::drawText(textPos, &elTexto, isFocused || setting->value->_bool ? MC_Color(255, 255, 255) : MC_Color(140, 140, 140), true, true, true);
									currentYOffset += textHeight + (textPadding * 2);
								}
								break;
							}
							case ValueType::ENUM_T: {  // Click setting
								// Text and background
								float settingStart = currentYOffset;
								{
									char name[0x22]; sprintf_s(name, +"%s:", setting->name); if (name[0] != 0) name[0] = toupper(name[0]);

									EnumEntry& i = ((SettingEnum*)setting->extraData)->GetSelectedEntry();

									char name2[0x21]; sprintf_s(name2, 0x21, " %s", i.GetName().c_str()); if (name2[0] != 0) name2[0] = toupper(name2[0]);
									string elTexto2 = name2;

									auto interfaceColor = ColorUtil::interfaceColor(1);
									string elTexto = string(GRAY) + name + string(RESET) + elTexto2;
									rectPos.w = currentYOffset + textHeight - 0.5 + (textPadding * 2);
									windowSize->x = fmax(windowSize->x, DrawUtils::getTextWidth(&len, textSize) + 16);
									/*
									DrawUtils::fillRectangleA(rectPos, MC_Color(35, 35, 35, 255));
									DrawUtils::fillRectangleA(rectPos2l, MC_Color(interfaceColor));
									DrawUtils::fillRectangleA(rectPos3l, MC_Color(interfaceColor));
									*/
									DrawUtils::fillRectangleA(rectPos, MC_Color(0, 0, 0, 90));
									DrawUtils::drawText(textPos, &elTexto, MC_Color(255, 255, 255), textSize, 1.f, true); //enum aids
									currentYOffset += textHeight + (textPadding * 2 - 12.5);
								}
								int e = 0;

								if ((currentYOffset - ourWindow->pos.y) > cutoffHeight) {
									overflowing = true;
									break;
								}

								bool isEven = e % 2 == 0;

								rectPos.w = currentYOffset + textHeight + (textPadding * 2);
								EnumEntry& i = ((SettingEnum*)setting->extraData)->GetSelectedEntry();
								textPos.y = currentYOffset * textPadding;
								vec4_t selectableSurface = vec4_t(textPos.x, textPos.y, xEnd, rectPos.w);
								// logic
								selectableSurface.y = settingStart;
								if (!ourWindow->isInAnimation && selectableSurface.contains(&mousePos)) {
									if (shouldToggleLeftClick) {
										shouldToggleLeftClick = false;
										((SettingEnum*)setting->extraData)->SelectNextValue(false);
										if (clickGUI->sounds) {
											auto player = g_Data.getLocalPlayer();
											PointingStruct* level = g_Data.getLocalPlayer()->pointingStruct;
											level->playSound("random.click", *player->getPos(), 1, 1);
										}
									}
									else if (shouldToggleRightClick) {
										shouldToggleRightClick = false;
										((SettingEnum*)setting->extraData)->SelectNextValue(true);
										if (clickGUI->sounds) {
											auto player = g_Data.getLocalPlayer();
											PointingStruct* level = g_Data.getLocalPlayer()->pointingStruct;
											level->playSound("random.click", *player->getPos(), 1, 1);
										}
									}
								}
								currentYOffset += textHeight + (textPadding * 2);

								break;
							}

							case ValueType::FLOAT_T: {
								// Text and background
								{
									vec2_t textPos = vec2_t(rectPos.x + ((rectPos.z - rectPos.x) / 2), rectPos.y);
									char str[10]; sprintf_s(str, 10, "%.2f", setting->value->_float);
									string text = str;
									char name[0x22]; sprintf_s(name, "%s: ", setting->name); if (name[0] != 0) name[0] = toupper(name[0]);
									string elTexto = name + text;
									windowSize->x = fmax(windowSize->x, DrawUtils::getTextWidth(&len, textSize) + 16);
									textPos.x = rectPos.x + 5;
									DrawUtils::drawText(textPos, &elTexto, MC_Color(255, 255, 255), textSize, 1.f, true);
									currentYOffset += textPadding + textHeight - 9;
									rectPos.w = currentYOffset;
									auto interfaceColor = ColorUtil::interfaceColor(1);
									/*
									DrawUtils::fillRectangleA(rectPos, MC_Color(35, 35, 35, 255));
									DrawUtils::fillRectangleA(rectPos2l, MC_Color(interfaceColor));
									DrawUtils::fillRectangleA(rectPos3l, MC_Color(interfaceColor));
									*/
									DrawUtils::fillRectangleA(rectPos, MC_Color(0, 0, 0, 90));
								}

								if ((currentYOffset - ourWindow->pos.y) > cutoffHeight) {
									overflowing = true;
									break;
								}
								// Slider
								{
									vec4_t rect = vec4_t(
										currentXOffset + textPadding + 5, currentYOffset + textPadding + 8.5,
										xEnd - textPadding + 5, currentYOffset - textPadding + textHeight + 3.f);
									// Visuals & Logic
									{
										rectPos.y = currentYOffset;
										rectPos.w += textHeight + (textPadding * 2) + 3.f;
										rect.z = rect.z - 10;
										DrawUtils::fillRectangle(rect, MC_Color(150, 150, 150), 1);
										//DrawUtils::fillRectangleA(rectPos, MC_Color(35, 35, 35, 255));
										auto interfaceColor = ColorUtil::interfaceColor(1);
										//DrawUtils::fillRectangleA(rectPos2l, MC_Color(interfaceColor));
										//DrawUtils::fillRectangleA(rectPos3l, MC_Color(interfaceColor));
										DrawUtils::fillRectangleA(rectPos, MC_Color(0, 0, 0, 90));
										DrawUtils::fillRectangle(rect, MC_Color(44, 44, 44), 1);// Background
										const bool areWeFocused = rectPos.contains(&mousePos);
										const float minValue = setting->minValue->_float;
										const float maxValue = setting->maxValue->_float - minValue;
										float value = (float)fmax(0, setting->value->_float - minValue);
										if (value > maxValue) value = maxValue; value /= maxValue;
										const float endXlol = (xEnd - textPadding) - (currentXOffset + textPadding + 10);
										value *= endXlol;

										{
											rect.z = rect.x + value;
											auto interfaceColor = ColorUtil::interfaceColor(1);
											DrawUtils::fillRectangle(rect, MC_Color(interfaceColor), (areWeFocused || setting->isDragging) ? 1.f : 1.f);
										}

										// Drag Logic
										{
											if (setting->isDragging) {
												if (isLeftClickDown && !isRightClickDown)
													value = mousePos.x - rect.x;
												else
													setting->isDragging = false;
											}
											else if (areWeFocused && shouldToggleLeftClick && !ourWindow->isInAnimation) {
												shouldToggleLeftClick = false;
												setting->isDragging = true;
												if (clickGUI->sounds) {
													auto player = g_Data.getLocalPlayer();
													PointingStruct* level = g_Data.getLocalPlayer()->pointingStruct;
													level->playSound("random.click", *player->getPos(), 1, 1);
												}
											}
										}

										// Save Value
										{
											value /= endXlol;  // Now in range 0 - 1
											value *= maxValue;
											value += minValue;

											setting->value->_float = value;
											setting->makeSureTheValueIsAGoodBoiAndTheUserHasntScrewedWithIt();
										}
									}
									currentYOffset += textHeight + (textPadding * 2) + 3.f;
								}
							} break;
							case ValueType::INT_T: {
								// Text and background
								{
									// Convert first letter to uppercase for more friendlieness
									vec2_t textPos2 = vec2_t(rectPos.x + ((rectPos.z - rectPos.x) / 2), rectPos.y);
									char name[0x22]; sprintf_s(name, "%s: ", setting->name); if (name[0] != 0) name[0] = toupper(name[0]);
									vec2_t textPos = vec2_t(rectPos.x + 10, rectPos.y + 10);
									char str[10]; sprintf_s(str, 10, "%i", setting->value->_int);
									string text = str;
									textPos2.x -= DrawUtils::getTextWidth(&text, textSize) / 2;
									textPos2.y += 2.5f;
									string elTexto = name + text;
									windowSize->x = fmax(windowSize->x, DrawUtils::getTextWidth(&len, textSize) + 15);
									textPos2.x = rectPos.x + 5;
									DrawUtils::drawText(textPos2, &elTexto, MC_Color(255, 255, 255), textSize, 1.f, true);
									currentYOffset += textPadding + textHeight - 7;
									rectPos.w = currentYOffset;
									//DrawUtils::fillRectangleA(rectPos, MC_Color(35, 35, 35, 255));
									auto interfaceColor = ColorUtil::interfaceColor(1);
									//DrawUtils::fillRectangleA(rectPos2l, MC_Color(interfaceColor));
									//DrawUtils::fillRectangleA(rectPos3l, MC_Color(interfaceColor));
									DrawUtils::fillRectangleA(rectPos, MC_Color(0, 0, 0, 90));

								}
								if ((currentYOffset - ourWindow->pos.y) > (g_Data.getGuiData()->heightGame * 0.75)) {
									overflowing = true;
									break;
								}
								// Slider
								{
									vec4_t rect = vec4_t(
										currentXOffset + textPadding + 5,
										currentYOffset + textPadding + 8.5,
										xEnd - textPadding + 5,
										currentYOffset - textPadding + textHeight + 3.f);


									// Visuals & Logic
									{
										rect.z = rect.z - 10;
										rectPos.y = currentYOffset;
										rectPos.w += textHeight + (textPadding * 2) + 3.f;
										// Background
										const bool areWeFocused = rectPos.contains(&mousePos);
										DrawUtils::fillRectangle(rect, MC_Color(150, 150, 150), 1);
										//DrawUtils::fillRectangleA(rectPos, MC_Color(35, 35, 35, 255));
										auto interfaceColor = ColorUtil::interfaceColor(1);
										//DrawUtils::fillRectangleA(rectPos2l, MC_Color(interfaceColor));
										//DrawUtils::fillRectangleA(rectPos3l, MC_Color(interfaceColor));
										DrawUtils::fillRectangleA(rectPos, MC_Color(0, 0, 0, 90));
										DrawUtils::fillRectangle(rect, MC_Color(44, 44, 44), 1);// Background

										const float minValue = (float)setting->minValue->_int;
										const float maxValue = (float)setting->maxValue->_int - minValue;
										float value = (float)fmax(0, setting->value->_int - minValue);  // Value is now always > 0 && < maxValue
										if (value > maxValue) value = maxValue;
										value /= maxValue;  // Value is now in range 0 - 1
										const float endXlol = (xEnd - textPadding) - (currentXOffset + textPadding + 5);
										value *= endXlol;  // Value is now pixel diff between start of bar and end of progress
										{
											rect.z = rect.x + value - 5;
											auto interfaceColor = ColorUtil::interfaceColor(1);
											DrawUtils::fillRectangle(rect, MC_Color(interfaceColor), (areWeFocused || setting->isDragging) ? 1.f : 1.f);

										}

										// Drag Logic
										{
											if (setting->isDragging) {
												if (isLeftClickDown && !isRightClickDown)
													value = mousePos.x - rect.x;
												else
													setting->isDragging = false;
											}
											else if (areWeFocused && shouldToggleLeftClick && !ourWindow->isInAnimation) {
												shouldToggleLeftClick = false;
												setting->isDragging = true;
												if (clickGUI->sounds) {
													auto player = g_Data.getLocalPlayer();
													PointingStruct* level = g_Data.getLocalPlayer()->pointingStruct;
													level->playSound("random.click", *player->getPos(), 1, 1);
												}
											}
										}

										// Save Value
										{
											value /= endXlol;  // Now in range 0 - 1
											value *= maxValue;
											value += minValue;

											setting->value->_int = (int)roundf(value);
											setting->makeSureTheValueIsAGoodBoiAndTheUserHasntScrewedWithIt();
										}
									}
									currentYOffset += textHeight + (textPadding * 2) + 3.f;
								}
							} break;
							}
						}
						currentYOffset += 4.5f;
						float endYOffset = currentYOffset;
						if (endYOffset - startYOffset > textHeight + 1 || overflowing) {
							startYOffset += textPadding;
							endYOffset -= textPadding;
							//DrawUtils::setColor(1, 1, 1, 1);
							DrawUtils::drawRectangle(vec4_t(currentXOffset + 2, startYOffset - 2.5f, xEnd - 2, endYOffset - 3.f), MC_Color(255, 255, 255), 0.5f);
							DrawUtils::drawRectangle(vec4_t(currentXOffset + 1, startYOffset - 3.5f, xEnd - 1, endYOffset - 2.f), mod->isEnabled() ? MC_Color(ColorUtil::interfaceColor(1)) : MC_Color(10, 10, 10), 1.f);
							//DrawUtils::drawLine(vec2_t(currentXOffset + 2, startYOffset - 1.75f), vec2_t(currentXOffset + 2, endYOffset - 1.75f), 0.5f);
						}
					}
				}
				else currentYOffset += textHeight + (textPadding * 2) + 2.f;
			}
		}

		vec4_t winRectPos = vec4_t(xOffset, yOffset, xEnd, currentYOffset);

		if (winRectPos.contains(&mousePos)) {
			if (scrollingDirection > 0 && overflowing) ourWindow->yOffset += scrollingDirection;
			else if (scrollingDirection < 0) ourWindow->yOffset += scrollingDirection;
			scrollingDirection = 0;
			if (ourWindow->yOffset < 0) ourWindow->yOffset = 0;
		}
	}
	DrawUtils::flush();
	// Draw Category Header
	{
		vec2_t textPos = vec2_t(currentXOffset + textPadding + 1, categoryHeaderYOffset + textPadding);
		vec4_t rectPos = vec4_t(
			currentXOffset - categoryMargin, categoryHeaderYOffset - categoryMargin,
			currentXOffset + windowSize->x + paddingRight + categoryMargin,
			categoryHeaderYOffset + textHeight + (textPadding * 2));
		vec4_t rectPos2 = vec4_t(
			currentXOffset - categoryMargin, categoryHeaderYOffset + categoryMargin + 9.5,
			currentXOffset + windowSize->x + paddingRight + categoryMargin,
			categoryHeaderYOffset + textHeight + (textPadding * 2));
		vec4_t rectTest = vec4_t(rectPos.x, rectPos.y + 1, rectPos.z, rectPos.w);
		vec4_t rectTest2 = vec4_t(rectPos.x + 1.f, rectPos.y + 2, rectPos.z - 1.f, rectPos.w);

		for (auto& mod : moduleList) {
			rectTest.w = currentYOffset;
			rectTest2.w = currentYOffset - 2;
		}

		// Extend Logic
		{
			if (rectPos.contains(&mousePos) && shouldToggleRightClick && !isDragging) {
				shouldToggleRightClick = false;
				ourWindow->isExtended = !ourWindow->isExtended;
				if (ourWindow->isExtended && ourWindow->animation == 0) ourWindow->animation = 0.2f;
				else if (!ourWindow->isExtended && ourWindow->animation == 1) ourWindow->animation = 0;
				ourWindow->isInAnimation = true;

				for (auto& mod : moduleList) {
					std::shared_ptr<ClickModule> clickMod = getClickModule(ourWindow, mod->getRawModuleName());
					clickMod->isExtended = false;
				}
			}
		}

		// Dragging Logic
		{
			if (isDragging && Utils::getCrcHash(categoryName) == draggedWindow) {
				if (isLeftClickDown) {
					vec2_t diff = vec2_t(mousePos).sub(dragStart);
					ourWindow->pos = ourWindow->pos.add(diff);
					dragStart = mousePos;
				}
				else {
					isDragging = false;
				}
			}
			else if (rectPos.contains(&mousePos) && shouldToggleLeftClick) {
				isDragging = true;
				draggedWindow = Utils::getCrcHash(categoryName);
				shouldToggleLeftClick = false;
				dragStart = mousePos;
			}
		}

		//Draw a bit more then just the HudEditor button
		/*{
			std::vector<SettingEntry*>* settings = clickGUI->getSettings();
			string textStr = "Packet";
			float textStrLen = DrawUtils::getTextWidth(&string("------------")) - 2.f;
			float textStrLen2 = DrawUtils::getTextWidth(&string("--------------"));
			float stringLen = DrawUtils::getTextWidth(&textStr) + 2;
			vec2_t windowSize2 = g_Data.getClientInstance()->getGuiData()->windowSize;
			float mid = windowSize2.x / 2 - 20;

			vec4_t rect = vec4_t(mid, 0, mid + textStrLen, 18);
			vec4_t settingsRect = vec4_t(rect.x + stringLen + 3, rect.y + 2, rect.x + stringLen + 17, rect.y + 16);
			vec4_t hudEditor = vec4_t(rect.x + stringLen + 19, rect.y + 2, rect.x + stringLen + 33, rect.y + 16);

			if (hudEditor.contains(&mousePos) && shouldToggleLeftClick) clickGUI->setEnabled(false);

			DrawUtils::fillRectangleA(rect, MC_Color(37, 39, 43, 255));
			DrawUtils::fillRectangleA(settingsRect, MC_Color(9, 12, 16, 255));
			DrawUtils::fillRectangleA(hudEditor, MC_Color(15, 20, 26, 255));
			DrawUtils::drawText(vec2_t(rect.x + 3, rect.y + 4), &textStr, MC_Color(255, 255, 255), 1.f, 1.f, true);

			float ourOffset = 17;
			static bool extended = false;

			if (settingsRect.contains(&mousePos) && shouldToggleRightClick) {
				shouldToggleRightClick = false;
				extended = !extended;
			}

			vec4_t idkRect = vec4_t(settingsRect.x, ourOffset, settingsRect.x + textStrLen2, ourOffset + 16);
			for (int t = 0; t < 4; t++)	idkRect.w += ourOffset;

			if (extended) {
				DrawUtils::fillRectangleA(idkRect, MC_Color(45, 45, 45, 255));
				string stringAids;
				string stringAids2;
				if (clickGUI->theme.getSelectedValue() == 0) stringAids = "Theme: Packet";
				if (clickGUI->theme.getSelectedValue() == 1) stringAids = "Theme: Vape";
				if (clickGUI->theme.getSelectedValue() == 2) stringAids = "Theme: Astolfo";

				if (clickGUI->color.getSelectedValue() == 0) stringAids2 = "Color: Rainbow";
				if (clickGUI->color.getSelectedValue() == 1) stringAids2 = "Color: Astolfo";
				if (clickGUI->color.getSelectedValue() == 2) stringAids2 = "Color: Wave";
				if (clickGUI->color.getSelectedValue() == 3) stringAids2 = "Color: RainbowWave";

				vec4_t selectableSurface = vec4_t(settingsRect.x, ourOffset + 2.f, settingsRect.x + textStrLen2, ourOffset + 17.f);
				vec4_t selectableSurface2 = vec4_t(settingsRect.x, ourOffset + 17.f, settingsRect.x + textStrLen2, ourOffset + 37.f);
				DrawUtils::drawText(vec2_t(selectableSurface.x + 2, selectableSurface.y + 3), &stringAids, MC_Color(255, 255, 255), 1.f, 1.f, true);
				DrawUtils::drawText(vec2_t(selectableSurface2.x + 2, selectableSurface2.y + 3), &stringAids2, MC_Color(255, 255, 255), 1.f, 1.f, true);

				if (selectableSurface.contains(&mousePos) && shouldToggleLeftClick) {
					clickGUI->theme.SelectNextValue(false);
					shouldToggleLeftClick = false;
				}
				if (selectableSurface.contains(&mousePos) && shouldToggleRightClick) {
					clickGUI->theme.SelectNextValue(true);
					shouldToggleLeftClick = false;
				}
				if (selectableSurface2.contains(&mousePos) && shouldToggleLeftClick) {
					clickGUI->color.SelectNextValue(false);
					shouldToggleLeftClick = false;
				}
				if (selectableSurface2.contains(&mousePos) && shouldToggleRightClick) {
					clickGUI->color.SelectNextValue(true);
					shouldToggleLeftClick = false;
				}
			}
		}*/

		// Draw category header
		{
			// Draw Text
			auto interfaceColor = ColorUtil::interfaceColor(1);
			string textStr = categoryName;
			//DrawUtils::drawText(textPos, &textStr, MC_Color(255, 255, 255), 1.f, 1.f, true);
			DrawUtils::drawCenteredString(vec2_t((currentXOffset - categoryMargin + currentXOffset + windowSize->x + paddingRight + categoryMargin) / 2, (categoryHeaderYOffset - categoryMargin + categoryHeaderYOffset + textHeight + (textPadding * 2) + 1) / 2), &textStr, 1.f, MC_Color(255, 255, 255, 255), true);
			DrawUtils::fillRectangle(vec4_t(rectPos.x - 0.1, rectPos.y - 2, rectPos.z + 0.1, rectPos.w), MC_Color(interfaceColor), 1.f);
			/*
			if (clickGUI->ModuleOutline) {
				DrawUtils::drawRectangle(vec4_t(rectPos.x - 0.25, rectPos.y - 2, rectPos.z + 0.25, rectPos.w), MC_Color(0.f, 0.f, 0.f), 0.5f);
			}
			*/ // Outline Category

			//DrawUtils::fillRectangleA(rectPos2, MC_Color(interfaceColor));
			//DrawUtils::drawRectangle(rectTest, MC_Color(interfaceColor), 1.f, 1.f);
			//DrawUtils::fillRectangleA(vec4_t(rectTest), MC_Color(0, 0, 0, 180));

			//DrawUtils::drawRoundRectangle(rectTest2, MC_Color(100, 100, 100), true);
		}
	}

	// anti idiot
	{
		vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
		if (ourWindow->pos.x + ourWindow->size.x > windowSize.x) ourWindow->pos.x = windowSize.x - ourWindow->size.x;
		if (ourWindow->pos.y + ourWindow->size.y > windowSize.y) ourWindow->pos.y = windowSize.y - ourWindow->size.y;
		ourWindow->pos.x = (float)fmax(0, ourWindow->pos.x);
		ourWindow->pos.y = (float)fmax(0, ourWindow->pos.y);
	}

	moduleList.clear();
	DrawUtils::flush();
}
#pragma endregion
void ClickGui::render() {
	static auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	auto interfaceDM = ColorUtil::interfaceColor(1);
	if (!moduleMgr->isInitialized()) return;

	if (timesRendered < 1) timesRendered++;

	// Fill Background
	{
		DrawUtils::fillRectangle(vec4_t(0, 0,
			g_Data.getClientInstance()->getGuiData()->widthGame,
			g_Data.getClientInstance()->getGuiData()->heightGame),
			MC_Color(0, 0, 0), 0.3);
		//DrawUtils::drawRectangle(vec4_t(0, 0, g_Data.getClientInstance()->getGuiData()->widthGame, g_Data.getClientInstance()->getGuiData()->heightGame), MC_Color(interfaceDM), 1.f, 2.f);
	}

	// Render all categorys

		renderPacketCategory(Category::COMBAT, MC_Color(255, 255, 255));
		renderPacketCategory(Category::VISUAL, MC_Color(255, 255, 255));
		renderPacketCategory(Category::MOVEMENT, MC_Color(255, 255, 255));
		renderPacketCategory(Category::PLAYER, MC_Color(255, 255, 255));
		renderPacketCategory(Category::EXPLOIT, MC_Color(255, 255, 255));
		renderPacketCategory(Category::OTHER, MC_Color(255, 255, 255));


	shouldToggleLeftClick = false;
	shouldToggleRightClick = false;
	resetStartPos = false;
	DrawUtils::flush();
}

void ClickGui::init() { initialised = true; }

void ClickGui::onMouseClickUpdate(int key, bool isDown) {
	static auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	if (clickGUI->isEnabled() && g_Data.isInGame()) {
		switch (key) {
		case 1:  // Left Click
			isLeftClickDown = isDown;
			if (isDown)
				shouldToggleLeftClick = true;
			break;
		case 2:  // Right Click
			isRightClickDown = isDown;
			if (isDown)
				shouldToggleRightClick = true;
			break;
		}
	}
}

void ClickGui::onWheelScroll(bool direction) {
	if (!direction) scrollingDirection++;
	else scrollingDirection--;
}

void ClickGui::onKeyUpdate(int key, bool isDown) {
	if (!initialised) return;
	static auto clickGUI = moduleMgr->getModule<ClickGUIMod>();

	if (!isDown) return;

	if (!clickGUI->isEnabled()) {
		timesRendered = 0;
		return;
	}

	if (timesRendered < 1) return;
	timesRendered = 0;

	switch (key) {
	case VK_ESCAPE:
		clickGUI->setEnabled(false);
		return;
	default:
		if (key == clickGUI->getKeybind()) clickGUI->setEnabled(false);
	}
}

#pragma region Config Stuff
using json = nlohmann::json;
void ClickGui::onLoadConfig(void* confVoid) {
	savedWindowSettings.clear();
	windowMap.clear();
	json* conf = reinterpret_cast<json*>(confVoid);
	if (conf->contains("ClickGuiMenu")) {
		auto obj = conf->at("ClickGuiMenu");
		if (obj.is_null())
			return;
		for (int i = 0; i <= (int)Category::CUSTOM; i++) {
			auto catName = ClickGui::catToName((Category)i);
			if (obj.contains(catName)) {
				auto value = obj.at(catName);
				if (value.is_null())
					continue;
				try {
					SavedWindowSettings windowSettings = {};
					windowSettings.name = catName;
					if (value.contains("pos")) {
						auto posVal = value.at("pos");
						if (!posVal.is_null() && posVal.contains("x") && posVal["x"].is_number_float() && posVal.contains("y") && posVal["y"].is_number_float()) {
							try {
								windowSettings.pos = { posVal["x"].get<float>(), posVal["y"].get<float>() };
							}
							catch (exception e) {
							}
						}
					}
					if (value.contains("isExtended")) {
						auto isExtVal = value.at("isExtended");
						if (!isExtVal.is_null() && isExtVal.is_boolean()) {
							try {
								windowSettings.isExtended = isExtVal.get<bool>();
							}
							catch (exception e) {
							}
						}
					}
					savedWindowSettings[Utils::getCrcHash(catName)] = windowSettings;
				}
				catch (exception e) {
					logF("Config Load Error (ClickGuiMenu): %s", e.what());
				}
			}
		}
	}
}
void ClickGui::onSaveConfig(void* confVoid) {
	json* conf = reinterpret_cast<json*>(confVoid);
	// First update our map
	for (const auto& wind : windowMap) {
		savedWindowSettings[wind.first] = { wind.second->pos, wind.second->isExtended, wind.second->name };
	}

	// Save to json
	if (conf->contains("ClickGuiMenu"))
		conf->erase("ClickGuiMenu");

	json obj = {};

	for (const auto& wind : savedWindowSettings) {
		json subObj = {};
		subObj["pos"]["x"] = wind.second.pos.x;
		subObj["pos"]["y"] = wind.second.pos.y;
		subObj["isExtended"] = wind.second.isExtended;
		obj[wind.second.name] = subObj;
	}

	conf->emplace("ClickGuiMenu", obj);
}
#pragma endregion