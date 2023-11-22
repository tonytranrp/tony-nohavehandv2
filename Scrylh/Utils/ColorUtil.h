#pragma once

class ColorUtil {
public:
	static MC_Color waveColor(int red, int green, int blue, int red2, int green2, int blue2, long index);
	static MC_Color RGBWave(int red, int green, int blue, int red2, int green2, int blue2, long index);
	static MC_Color rainbowColor(float seconds, float saturation, float brightness, long index);
	static MC_Color astolfoRainbow(int yOffset, int yTotal);
	static MC_Color interfaceColor(int index);
	static void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);
};