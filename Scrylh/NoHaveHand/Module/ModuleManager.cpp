#include "ModuleManager.h"
#include "../../Utils/Logger.h"
#include "../../Utils/Json.hpp"

using json = nlohmann::json;

ModuleManager::ModuleManager(GameData* gameData) {
	this->gameData = gameData;
}

ModuleManager::~ModuleManager() {
	initialized = false;
	auto lock = this->lockModuleListExclusive();
	this->moduleList.clear();
}

void ModuleManager::initModules() {
	{
		auto lock = this->lockModuleListExclusive();

		// Menu
		moduleList.push_back(std::shared_ptr<IModule>(new ConfigManagerMod()));
		moduleList.push_back(std::shared_ptr<IModule>(new ClickGUIMod()));
		moduleList.push_back(std::shared_ptr<IModule>(new DebugMenu()));

		// Combat
		//moduleList.push_back(std::shared_ptr<IModule>(new CrystalAuraBPG()));
		moduleList.push_back(std::shared_ptr<IModule>(new CrystalAuraWTA()));
		//moduleList.push_back(std::shared_ptr<IModule>(new CrystalAuraOW()));
		moduleList.push_back(std::shared_ptr<IModule>(new pvpResources()));
		moduleList.push_back(std::shared_ptr<IModule>(new DamageBug()));
		moduleList.push_back(std::shared_ptr<IModule>(new TrapAnvil()));
		moduleList.push_back(std::shared_ptr<IModule>(new CrystalAuraGK()));
		moduleList.push_back(std::shared_ptr<IModule>(new CrystalAuraOW()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoFallBlock()));
		moduleList.push_back(std::shared_ptr<IModule>(new Crystalbreak()));
		moduleList.push_back(std::shared_ptr<IModule>(new AntiCrystal()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoClicker()));
		moduleList.push_back(std::shared_ptr<IModule>(new cascaffold()));
		moduleList.push_back(std::shared_ptr<IModule>(new TriggerBot()));
		moduleList.push_back(std::shared_ptr<IModule>(new TargetDown()));
		moduleList.push_back(std::shared_ptr<IModule>(new Criticals()));
		moduleList.push_back(std::shared_ptr<IModule>(new BowAimbot()));
		moduleList.push_back(std::shared_ptr<IModule>(new Switcher2()));
		moduleList.push_back(std::shared_ptr<IModule>(new Surround()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoTrap()));
		moduleList.push_back(std::shared_ptr<IModule>(new Killaura()));
		moduleList.push_back(std::shared_ptr<IModule>(new Switcher()));
		moduleList.push_back(std::shared_ptr<IModule>(new HitBoxes()));
		moduleList.push_back(std::shared_ptr<IModule>(new HoleFill()));
		moduleList.push_back(std::shared_ptr<IModule>(new HoleKick()));
		moduleList.push_back(std::shared_ptr<IModule>(new BowSpam()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoPot()));
		moduleList.push_back(std::shared_ptr<IModule>(new Aimbot()));
		moduleList.push_back(std::shared_ptr<IModule>(new TPAura()));
		moduleList.push_back(std::shared_ptr<IModule>(new Reach()));
		moduleList.push_back(std::shared_ptr<IModule>(new Teams()));

		// Visual
		moduleList.push_back(std::shared_ptr<IModule>(new TargetVisualize()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoEntityRender()));
		moduleList.push_back(std::shared_ptr<IModule>(new Notifications()));
		moduleList.push_back(std::shared_ptr<IModule>(new InventoryView()));
		moduleList.push_back(std::shared_ptr<IModule>(new BlockOutline()));
		moduleList.push_back(std::shared_ptr<IModule>(new PlayerListx()));
		moduleList.push_back(std::shared_ptr<IModule>(new TunnelESP()));
		moduleList.push_back(std::shared_ptr<IModule>(new BreadCrumbs()));
		moduleList.push_back(std::shared_ptr<IModule>(new Animations()));
		moduleList.push_back(std::shared_ptr<IModule>(new CombatInfo()));
		moduleList.push_back(std::shared_ptr<IModule>(new ArrayList()));
		moduleList.push_back(std::shared_ptr<IModule>(new CustomSky()));
		moduleList.push_back(std::shared_ptr<IModule>(new Interface()));
		moduleList.push_back(std::shared_ptr<IModule>(new TargetHUD()));
		moduleList.push_back(std::shared_ptr<IModule>(new Watermark()));
		moduleList.push_back(std::shared_ptr<IModule>(new BindList()));
		moduleList.push_back(std::shared_ptr<IModule>(new HoleEsp()));
		moduleList.push_back(std::shared_ptr<IModule>(new Ambience()));
		moduleList.push_back(std::shared_ptr<IModule>(new BlockESP()));
		moduleList.push_back(std::shared_ptr<IModule>(new ChestESP()));
		moduleList.push_back(std::shared_ptr<IModule>(new Freelook()));
		moduleList.push_back(std::shared_ptr<IModule>(new NameTags()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoRender()));
		moduleList.push_back(std::shared_ptr<IModule>(new Welcome()));
		moduleList.push_back(std::shared_ptr<IModule>(new Search()));
		moduleList.push_back(std::shared_ptr<IModule>(new Camera()));
		moduleList.push_back(std::shared_ptr<IModule>(new Radar()));
		moduleList.push_back(std::shared_ptr<IModule>(new Zoom()));
		moduleList.push_back(std::shared_ptr<IModule>(new Xray()));
		moduleList.push_back(std::shared_ptr<IModule>(new ESP()));

		// Movement
		moduleList.push_back(std::shared_ptr<IModule>(new CubeCraftSpeed()));
		moduleList.push_back(std::shared_ptr<IModule>(new TargetStrafe()));
		moduleList.push_back(std::shared_ptr<IModule>(new ElytraFly()));
		moduleList.push_back(std::shared_ptr<IModule>(new AntiVoid()));
		moduleList.push_back(std::shared_ptr<IModule>(new FastStop()));
		moduleList.push_back(std::shared_ptr<IModule>(new LongJump()));
		moduleList.push_back(std::shared_ptr<IModule>(new Velocity()));
		moduleList.push_back(std::shared_ptr<IModule>(new Jetpack()));
		moduleList.push_back(std::shared_ptr<IModule>(new InvMove()));
		moduleList.push_back(std::shared_ptr<IModule>(new FlyBypass())); // 2b2eFly
		moduleList.push_back(std::shared_ptr<IModule>(new HoleSwap())); // 2b2eFly
		moduleList.push_back(std::shared_ptr<IModule>(new NoClip()));
		moduleList.push_back(std::shared_ptr<IModule>(new Flight()));
		moduleList.push_back(std::shared_ptr<IModule>(new FastFly()));
		moduleList.push_back(std::shared_ptr<IModule>(new AirStuck()));
		moduleList.push_back(std::shared_ptr<IModule>(new AirJump()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoSlow()));
		moduleList.push_back(std::shared_ptr<IModule>(new Spider()));
		moduleList.push_back(std::shared_ptr<IModule>(new Sprint()));
		moduleList.push_back(std::shared_ptr<IModule>(new Jesus()));
		moduleList.push_back(std::shared_ptr<IModule>(new Sneak()));
		moduleList.push_back(std::shared_ptr<IModule>(new Glide()));
		moduleList.push_back(std::shared_ptr<IModule>(new Speed()));
		moduleList.push_back(std::shared_ptr<IModule>(new Step()));
		moduleList.push_back(std::shared_ptr<IModule>(new Bhop()));
		// Player
		moduleList.push_back(std::shared_ptr<IModule>(new ChestStealer()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoJumpDelay()));
		moduleList.push_back(std::shared_ptr<IModule>(new BlockReach()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoHotbar()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoButton()));
		moduleList.push_back(std::shared_ptr<IModule>(new AntiAnvil()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoTotem()));
		moduleList.push_back(std::shared_ptr<IModule>(new FastPlace()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoArmor()));
		moduleList.push_back(std::shared_ptr<IModule>(new Scaffold()));
		moduleList.push_back(std::shared_ptr<IModule>(new AutoLog()));
		moduleList.push_back(std::shared_ptr<IModule>(new Freecam()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoSwing()));
		moduleList.push_back(std::shared_ptr<IModule>(new FastEat()));
		moduleList.push_back(std::shared_ptr<IModule>(new Blink()));
		moduleList.push_back(std::shared_ptr<IModule>(new Timer()));
		moduleList.push_back(std::shared_ptr<IModule>(new XP()));

		// Exploit
		moduleList.push_back(std::shared_ptr<IModule>(new PhaseForAntiAnvil()));
	//	moduleList.push_back(std::shared_ptr<IModule>(new AntiRollBack()));
		moduleList.push_back(std::shared_ptr<IModule>(new AntiImmobile()));
		moduleList.push_back(std::shared_ptr<IModule>(new BlockClip()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoLagBack()));
		moduleList.push_back(std::shared_ptr<IModule>(new SpeedMine2()));
		moduleList.push_back(std::shared_ptr<IModule>(new SpeedMine()));
		moduleList.push_back(std::shared_ptr<IModule>(new EntityFly()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoPacket()));
		moduleList.push_back(std::shared_ptr<IModule>(new FastUse()));
		moduleList.push_back(std::shared_ptr<IModule>(new Crasher()));
		moduleList.push_back(std::shared_ptr<IModule>(new FreeTP()));
		moduleList.push_back(std::shared_ptr<IModule>(new NoFall()));
		moduleList.push_back(std::shared_ptr<IModule>(new Phase()));

		// Other
		moduleList.push_back(std::shared_ptr<IModule>(new ToggleSounds()));
		moduleList.push_back(std::shared_ptr<IModule>(new HitEffects()));
		moduleList.push_back(std::shared_ptr<IModule>(new ChatSuffix()));
		moduleList.push_back(std::shared_ptr<IModule>(new Spammer()));
		moduleList.push_back(std::shared_ptr<IModule>(new ClickTP()));
		moduleList.push_back(std::shared_ptr<IModule>(new ItemTP()));
		//moduleList.push_back(std::shared_ptr<IModule>(new IDK()));
//		moduleList.push_back(std::shared_ptr<IModule>(new AutoEZ()));
		moduleList.push_back(std::shared_ptr<IModule>(new Nuker()));
		moduleList.push_back(std::shared_ptr<IModule>(new Derp()));
		moduleList.push_back(std::shared_ptr<IModule>(new MCF()));
		moduleList.push_back(std::shared_ptr<IModule>(new PacketLogger()));

		std::sort(moduleList.begin(), moduleList.end(), [](auto lhs, auto rhs) {
			auto current = lhs;
			auto other = rhs;
			return std::string{*current->getModuleName()} < std::string{*other->getModuleName()};
		});
		initialized = true;
	}
	
	getModule<ClickGUIMod>()->setEnabled(false);
	getModule<Interface>()->setEnabled(true);
	getModule<ArrayList>()->setEnabled(true);
	getModule<Watermark>()->setEnabled(true);
	//getModule<IDK>()->setEnabled(true);
}

void ModuleManager::disable() {
	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		if (mod->isEnabled())
			mod->setEnabled(false);
	}
}

void ModuleManager::onLoadConfig(void* confVoid) {
	auto conf = reinterpret_cast<json*>(confVoid);
	if (!isInitialized()) return;

	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		mod->onLoadConfig(conf);
	}

	if (!getModule<Step>()->isEnabled() && g_Data.getLocalPlayer() != nullptr) g_Data.getLocalPlayer()->stepHeight = 0.5625f;
	getModule<ClickGUIMod>()->setEnabled(false);
	//getModule<IDK>()->setEnabled(true);
}

void ModuleManager::onSaveConfig(void* confVoid) {
	auto conf = reinterpret_cast<json*>(confVoid);
	if (!isInitialized()) return;
	//getModule<IDK>()->setEnabled(true);
	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		mod->onSaveConfig(conf);
	}
}

void ModuleManager::onLoadSettings(void* confVoid) {
	auto conf = reinterpret_cast<json*>(confVoid);
	if (!isInitialized()) return;
	//getModule<IDK>()->setEnabled(true);
	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		mod->onLoadSettings(conf);
	}
	getModule<ClickGUIMod>()->setEnabled(false);
}

void ModuleManager::onSaveSettings(void* confVoid) {
	auto conf = reinterpret_cast<json*>(confVoid);
	if (!isInitialized()) return;
	//getModule<IDK>()->setEnabled(true);
	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		mod->onSaveSettings(conf);
	}
}

void ModuleManager::onPlayerTick(C_Player* plr) {
	if (!isInitialized()) return;

	auto lock = lockModuleList();
	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->callWhenDisabled())
			mod->onPlayerTick(plr);
	}
}

void ModuleManager::onWorldTick(C_GameMode* gameMode) {
	if (!isInitialized()) return;

	auto lock = lockModuleList();
	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->callWhenDisabled())
			mod->onWorldTick(gameMode);
	}
}

void ModuleManager::onTick(C_GameMode* gameMode) {
	if (!isInitialized()) return;

	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		if (mod->isEnabled() || mod->callWhenDisabled())
			mod->onTick(gameMode);
	}
}

void ModuleManager::onAttack(C_Entity* attackEnt) {
	if (!isInitialized()) return;

	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		if (mod->isEnabled() || mod->callWhenDisabled())
			mod->onAttack(attackEnt);
	}
}

void ModuleManager::onKeyUpdate(int key, bool isDown) {
	if (!isInitialized()) return;

	auto lock = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		mod->onKeyUpdate(key, isDown);
	}
}

void ModuleManager::onPreRender(C_MinecraftUIRenderContext* renderCtx) {
	if (!isInitialized()) return;

	auto mutex = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		if (mod->isEnabled() || mod->callWhenDisabled())
			mod->onPreRender(renderCtx);
	}
}

void ModuleManager::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	if (!isInitialized()) return;

	auto mutex = this->lockModuleList();
	for (auto& mod : this->moduleList) {
		if (mod->isEnabled() || mod->callWhenDisabled())
			mod->onPostRender(renderCtx);
	}
}

void ModuleManager::onSendPacket(Packet* packet) {
	if (!isInitialized()) return;

	auto lock = lockModuleList();
	for (auto& it : moduleList) {
		if (it->isEnabled() || it->callWhenDisabled())
			it->onSendPacket(packet);
	}
}

std::vector<std::shared_ptr<IModule>>* ModuleManager::getModuleList() {
	return &this->moduleList;
}

int ModuleManager::getModuleCount() {
	return (int)moduleList.size();
}

int ModuleManager::getEnabledModuleCount() {
	int i = 0;
	auto lock = this->lockModuleList();
	for (auto& it : moduleList) {
		if (it->isEnabled()) i++;
	}
	return i;
}

void ModuleManager::onMove(C_MoveInputHandler* hand) {
	if (!isInitialized()) return;

	auto lock = this->lockModuleList();
	for (auto& it : moduleList) {
		if (it->isEnabled() || it->callWhenDisabled())
			it->onMove(hand);
	}
}

void ModuleManager::onLevelRender() {
	if (!isInitialized()) return;
	auto lock = this->lockModuleList();
	for (auto& it : moduleList) {
		if (it->isEnabled() || it->callWhenDisabled()) it->onLevelRender();
	}
}

ModuleManager* moduleMgr = new ModuleManager(&g_Data);