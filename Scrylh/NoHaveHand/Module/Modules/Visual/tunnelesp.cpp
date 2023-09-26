#include "tunnelesp.h"

TunnelESP::TunnelESP() : IModule(0x0, Category::VISUAL, "Locate a good Tunnels ;)") {
	registerIntSetting("Range", &this->range, 1, 5, 100);
}

TunnelESP::~TunnelESP() {
}

const char* TunnelESP::getModuleName() {
	return ("TunnelESP");
}

void TunnelESP::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	if (!(g_Data.getLocalPlayer() == nullptr || !GameData::canUseMoveKeys())) {
		if (g_Data.isInGame()) {
			if (!moduleMgr->getModule<ClickGUIMod>()->isEnabled()) {
				vec3_t* pos = g_Data.getLocalPlayer()->getPos();
				for (int x = (int)pos->x - range; x < pos->x + range; x++) {
					for (int z = (int)pos->z - range; z < pos->z + range; z++) {
						for (int y = (int)pos->y; y < pos->y; y++) {
							vec4_t underline;
							// Real Block Pos
							vec3_t blockPos = vec3_t(x, y, z);
							// Real Block Pos

							// Y Minus-Plus
							vec3_t blockPosYMinus = vec3_t(x, y - 1, z);
							vec3_t blockPosYPlus = vec3_t(x, y + 3, z);
							// Y Minus-Plus

							// AirPos
							vec3_t air = vec3_t(x, y + 1, z);
							vec3_t air2 = vec3_t(x, y + 2, z);
							// AirPos

							// BlockPos-RenderPos
							vec3_t blockPosRender1 = vec3_t(x, y, z);
							vec3_t blockPosRender2 = vec3_t(x, y - 1, z);
							vec3_t blockPosRender3 = vec3_t(x, y + 1, z);
							// BlockPos-RenderPos

							// BlockPos-XPlus-ZMinus
							vec3_t blockPosXPlus = vec3_t(x + 1, y, z);
							vec3_t blockPosXMinus = vec3_t(x - 1, y, z);
							vec3_t blockPosZPlus = vec3_t(x, y, z + 1);
							vec3_t blockPosZMinus = vec3_t(x, y, z - 1);
							// BlockPos-XPlus-ZMinus

							// Real Block Pos
							C_Block* block = g_Data.getLocalPlayer()->region->getBlock(blockPos);
							// Real Block Pos

							// Y Minus-Plus
							C_Block* blockMinus = g_Data.getLocalPlayer()->region->getBlock(blockPosYMinus);
							C_Block* blockPlus = g_Data.getLocalPlayer()->region->getBlock(blockPosYPlus);
							// Y Minus-Plus

							// BlockPos-XPlus-ZMinus
							C_Block* blockXPlus = g_Data.getLocalPlayer()->region->getBlock(blockPosXPlus);
							C_Block* blockXMinus = g_Data.getLocalPlayer()->region->getBlock(blockPosXMinus);
							C_Block* blockZPlus = g_Data.getLocalPlayer()->region->getBlock(blockPosZPlus);
							C_Block* blockZMinus = g_Data.getLocalPlayer()->region->getBlock(blockPosZMinus);
							// BlockPos-XPlus-ZMinus

							C_Block* airpos = g_Data.getLocalPlayer()->region->getBlock(air);
							C_Block* airpos2 = g_Data.getLocalPlayer()->region->getBlock(air2);

							if (block != nullptr && blockMinus != nullptr && blockXPlus != nullptr && blockXMinus != nullptr && blockZPlus != nullptr && blockZMinus != nullptr) {
								// Real Block Pos
								int blockId = block->toLegacy()->blockId;
								// Real Block Pos

								// Y Minus-Plus
								int blockIdMinus = blockMinus->toLegacy()->blockId;
								int blockIdPlus = blockPlus->toLegacy()->blockId;
								// Y Minus-Plus

								// BlockPos-XPlus-ZMinus
								int blockIdXPlus = block->toLegacy()->blockId;
								int blockIdXMinus = block->toLegacy()->blockId;
								int blockIdZPlus = block->toLegacy()->blockId;
								int blockIdZMinus = block->toLegacy()->blockId;
								// BlockPos-XPlus-ZMinus
								int aircheck = airpos->toLegacy()->blockId;
								int aircheck2 = airpos2->toLegacy()->blockId;

								bool obs = false;

								if (blockId == 0) obs = true;
								if (blockIdXPlus != 0 && blockIdXMinus != 0 && blockIdZPlus != 0 && blockIdZMinus != 0 && aircheck == 0 && aircheck2 == 0) {
									// ColorSettings
									DrawUtils::setColor(255.f, 255.f, 255.f, 1.f);
									// Render 1
									DrawUtils::drawBox(blockPosRender1, vec3_t(blockPosRender1).add(1.f, -0.01f, 1.f), (float)0.4f, false);
									// Render 2
									DrawUtils::drawBox(blockPosRender2, vec3_t(blockPosRender2).add(1.f, -0.01f, 1.f), (float)0.4f, false);
									// Render 3
									DrawUtils::drawBox(blockPosRender3, vec3_t(blockPosRender3).add(1.f, -0.01f, 1.f), (float)0.4f, false);
								}
							}
						}
					}
				}
			}
		}
	}
}