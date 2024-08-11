/*
 * Color.cpp
 *
 *  Created on: Jan. 9, 2019
 *      Author: serge
 */

#include "Color.h"
#include <SDL2/SDL.h>
#include "Utils.h"

const SDL_PixelFormat* Color::mFormat = nullptr;
void Color::InitColorFormat(const SDL_PixelFormat * format)
{
	Color::mFormat = format;
}

Color::Color()
	: mColor(0)
	, mR(0)
	, mG(0)
	, mB(0)
	, mA(0)
{

}

Color::Color(uint32_t color)
	:mColor(color)
{
	SDL_GetRGBA(color, mFormat, &mR, &mG, &mB, &mA);
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	: mColor(0)
	, mR(r)
	, mG(g)
	, mB(b)
	, mA(a)
{
	Generate32BitColor();
}

void Color::Generate32BitColor()
{
	mColor = SDL_MapRGBA(mFormat, mR, mG, mB, mA);
}


void Color::SetRed(uint8_t red)
{
	mR = red;
	Generate32BitColor();
}

void Color::SetGreen(uint8_t green)
{
	mG = green;
	Generate32BitColor();
}

void Color::SetBlue(uint8_t blue)
{
	mB = blue;
	Generate32BitColor();
}

void Color::SetAlpha(uint8_t alpha)
{
	mA = alpha;
	Generate32BitColor();
}

Color Color::Evaluate1MinusSourceAlpha(const Color& source, const Color& destination)
{
	//SourceRGB * sourceAlpha + DestinationRGB * (1 - sourceAlpha)
	uint8_t alpha = source.GetAlpha();

	float sourceAlpha = float(alpha) / 255.0f;
	
    Color newColor = Color::Lerp(destination, source, sourceAlpha, ease::EaseLinear);
    newColor.SetAlpha(255);

	return newColor;
}

Color Color::Lerp(const Color& c1, const Color& c2, float t)
{
	return Lerp(c1, c2, t, ease::EaseLinear);
}

Color Color::Lerp(const Color& c1, const Color& c2, float t, ease::EasingFunc func)
{
	t = func(t);

	uint8_t r = Clamp8(static_cast<uint8_t>(Lerpf(c1.GetRed(), c2.GetRed(), t)), 0, 255);
	uint8_t g = Clamp8(static_cast<uint8_t>(Lerpf(c1.GetGreen(), c2.GetGreen(), t)), 0, 255);
	uint8_t b = Clamp8(static_cast<uint8_t>(Lerpf(c1.GetBlue(), c2.GetBlue(), t)), 0, 255);
	uint8_t a = Clamp8(static_cast<uint8_t>(Lerpf(c1.GetAlpha(), c2.GetAlpha(), t)), 0, 255);

	return Color(r, g, b, a);
}
