#include "../NoHaveHand/Module/ModuleManager.h"
#include "../Utils/TimerUtil.h"
#include "ColorUtil.h"
#include "DrawUtils.h"

MC_Color ColorUtil::rainbowColor(float seconds, float saturation, float brightness, long index) {
	float hue6 = (((TimerUtil::getCurrentMs() + index) % (int)(6000)) / (float)(6000));
	float r, g, b = 0;
	Utils::HSVtoRGB(hue6, saturation, brightness, r, g, b);

	return MC_Color(r * 255, g * 255, b * 255, 255);
}

MC_Color ColorUtil::RGBWave(int red, int green, int blue, int red2, int green2, int blue2, long index) {
	auto interface = moduleMgr->getModule<Interface>();
	float hue = ((TimerUtil::getCurrentMs()) % (int)(((int)10) * 1000)) / (float)(((int)10) * 1000);
	float r, g, b = 0;
	Utils::HSVtoRGB(hue, interface->saturation, 1, r, g, b);
	double offset = ((TimerUtil::getCurrentMs() - index) / 2) / (double)120;
	double aids123 = ((TimerUtil::getCurrentMs() - index) % 1000 / 1000.000);
	int aids1234 = ((TimerUtil::getCurrentMs() - index) % 2000 / 2000.000) * 2;
	aids123 = aids1234 % 2 == 0 ? aids123 : 1 - aids123;
	double inverse_percent = 1 - aids123;
	int redPart = (int)(r * red * inverse_percent + r * red2 * aids123);
	int greenPart = (int)(g * green * inverse_percent + g * green2 * aids123);
	int bluePart = (int)(b * blue * inverse_percent + b * blue2 * aids123);

	return MC_Color(redPart, greenPart, bluePart);
}

MC_Color ColorUtil::waveColor(int red, int green, int blue, int red2, int green2, int blue2, long index) {
	double offset = ((TimerUtil::getCurrentMs() - index) / 8) / (double)120;
	double aids123 = ((TimerUtil::getCurrentMs() - index) % 1000 / 1000.000);
	int aids1234 = ((TimerUtil::getCurrentMs() - index) % 2000 / 2000.000) * 2;
	aids123 = aids1234 % 2 == 0 ? aids123 : 1 - aids123;
	double inverse_percent = 1 - aids123;
	int redPart = (int)(red * inverse_percent + red2 * aids123);
	int greenPart = (int)(green * inverse_percent + green2 * aids123);
	int bluePart = (int)(blue * inverse_percent + blue2 * aids123);

	return MC_Color(redPart, greenPart, bluePart);
}

MC_Color ColorUtil::astolfoRainbow(int yOffset, int yTotal) {
	auto interface = moduleMgr->getModule<Interface>();
	float speed = 3000;
	float hue = (float)(TimerUtil::getCurrentMs() % (int)speed) + ((yTotal - yOffset) * 9);
	while (hue > speed) { hue -= speed; } hue /= speed;
	if (hue > 0.5) { hue = 0.5F - (hue - 0.5f); } hue += 0.5f;
	float r, g, b = 0;
	Utils::HSVtoRGB(hue, 0.5, 1.f, r, g, b);

	return MC_Color(r * 255, g * 255, b * 255, 255);
}
void ColorUtil::HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
	int i = static_cast<int>(h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);
	switch (i % 6) {
	case 0: r = v, g = t, b = p; break;
	case 1: r = q, g = v, b = p; break;
	case 2: r = p, g = v, b = t; break;
	case 3: r = p, g = q, b = v; break;
	case 4: r = t, g = p, b = v; break;
	case 5: r = v, g = p, b = q; break;
	}
}

MC_Color ColorUtil::interfaceColor(int index) {
	// Calculate the hue value based on the index
	float hue = std::sin(static_cast<float>(index) * 0.1f); // Adjust the value (0.1f) for the desired speed of transition

	// Map the hue value to the range 0 to 1
	hue = (hue + 1.0f) * 0.5f;

	// Set the saturation and value for a nice pastel color
	float saturation = 0.7f;
	float value = 0.9f;

	// Convert the HSV color to RGB
	float r, g, b;
	HSVtoRGB(hue, saturation, value, r, g, b);

	// Scale the RGB values to the valid range (0 to 255)
	int redValue = static_cast<int>(r * 255.0f);
	int greenValue = static_cast<int>(g * 255.0f);
	int blueValue = static_cast<int>(b * 255.0f);

	// Set the color using the calculated RGB values
	MC_Color color(redValue, greenValue, blueValue);

	return color;
}

