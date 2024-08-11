/*
 * Color.h
 *
 *  Created on: Jan. 9, 2019
 *      Author: serge
 */

#ifndef GRAPHICS_COLOR_H_
#define GRAPHICS_COLOR_H_

#include <stdint.h>
#include "Easings.h"

struct SDL_PixelFormat;

class Color
{
	uint32_t mColor;
	uint8_t mR;
	uint8_t mG;
	uint8_t mB;
	uint8_t mA;
public:

	static const SDL_PixelFormat* mFormat;
	static void InitColorFormat(const SDL_PixelFormat * format);

	//blending equation
	//SourceRGB * sourceAlpha + DestinationRGB * (1 - sourceAlpha)
	static Color Evaluate1MinusSourceAlpha(const Color& source, const Color& destination);

	static Color Black() {return Color(0, 0, 0, 255);}
	static Color ClearBlack() { return Color(0, 0, 0, 0); }
	static Color White() {return Color(255, 255, 255, 255);}
	static Color Blue() {return Color(0, 0, 255, 255);}
	static Color Red() {return Color(255, 0, 0, 255);}
	static Color Green() {return Color(0, 255, 0, 255);}
	static Color Yellow() {return Color(255, 255, 0, 255);}
	static Color Magenta() {return Color(255, 0, 255, 255);}
	static Color Cyan() {return Color(37, 240, 217, 255);}
	static Color Pink() {return Color(252, 197, 224, 255);}
	static Color Orange() {return Color(245, 190, 100, 255);}

	Color();
	Color(uint32_t color);
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	inline bool operator==(const Color& c) const {return mColor == c.mColor;}
	inline bool operator!=(const Color& c) const {return !(*this == c);}
	inline uint32_t GetPixelColor() const {return mColor;}

	void Generate32BitColor();

	void SetRed(uint8_t red);
	void SetGreen(uint8_t green);
	void SetBlue(uint8_t blue);
	void SetAlpha(uint8_t alpha);

	inline uint8_t GetRed() const { return mR; }
	inline uint8_t GetGreen() const { return mG; }
	inline uint8_t GetBlue() const { return mB; }
	inline uint8_t GetAlpha() const { return mA; }

	static Color Lerp(const Color& c1, const Color& c2, float t);

	static Color Lerp(const Color& c1, const Color& c2, float t, ease::EasingFunc func);
};



#endif /* GRAPHICS_COLOR_H_ */
