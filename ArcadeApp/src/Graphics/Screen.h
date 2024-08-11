/*
 * Screen.h
 *
 *  Created on: Jan. 10, 2019
 *      Author: serge
 */

#ifndef GRAPHICS_SCREEN_H_
#define GRAPHICS_SCREEN_H_

#include <stdint.h>
#include "ScreenBuffer.h"
#include "Color.h"
#include <functional>
#include <vector>

#include "Vec2D.h"

class Line2D;
class Triangle;
class AARectangle;
class Circle;
struct SDL_Window;
struct SDL_Surface;
class BMPImage;
class SpriteSheet;
struct Sprite;
class BitmapFont;

struct SDL_Renderer;
struct SDL_PixelFormat;
struct SDL_Texture;

enum UVOrientation
{
	INSIDE = 0,
	OUTSIDE
};

struct UVParams
{

	UVOrientation mOrientation = INSIDE;
	Vec2D mSplitPoint1 = {1.0f, 0.0f};
	Vec2D mSplitPoint2 = {1.0f, 1.0f};
};

enum GradientXParam
{
	NO_X_GRADIENT = 0,
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT
};

enum GradientYParam
{
	NO_Y_GRADIENT = 0,
	BOTTOM_TO_TOP,
	TOP_TO_BOTTOM
};

struct GradientParams
{
	GradientXParam xParam = NO_X_GRADIENT;
	GradientYParam yParam = NO_Y_GRADIENT;

	Color color1 = Color::White();
	Color color2 = Color::White();
};

struct DrawTransform
{
	Vec2D pos = Vec2D::Zero;
	float scale = 1.0f;
	float rotationAngle = 0.0f;
};

struct ColorParams
{
	GradientParams gradient = {};
	Color overlay = Color::White();
	float alpha = 1.0f;
	bool bilinearFiltering = false;
};

enum DrawSurface
{
	FOREGROUND = 0,
	BACKGROUND
};

class Screen
{
public:
	Screen();
	~Screen();

	SDL_Window* Init(uint32_t w, uint32_t h, uint32_t mag, bool fast = true);
	void Update(uint32_t dt);
	void SwapScreens();

	inline void SetClearColor(const Color& clearColor) {mClearColor = clearColor;}
	inline uint32_t Width() const {return mWidth;}
	inline uint32_t Height() const {return mHeight;}

	//Draw Methods go here

	void Draw(int x, int y, const Color& color);
	void Draw(const Vec2D& point, const Color& color);
	void Draw(const Line2D& line, const Color& color);
	void Draw(const Triangle& triangle, const Color& color, bool fill = false, const Color& fillColor = Color::White());
	void Draw(const AARectangle& rect, const Color& color, bool fill = false, const Color& fillColor = Color::White());
	void Draw(const Circle& circle, const Color& color, bool fill = false, const Color& fillColor = Color::White());


	void Draw(const BMPImage& image, const Sprite& sprite, const DrawTransform& transform, const ColorParams& colorParams, const UVParams& uvParams, DrawSurface drawSurface = FOREGROUND);

	void Draw(const SpriteSheet& ss, const std::string& spriteName, const DrawTransform& transform, const ColorParams& colorParams, const UVParams& uvParams);
	void Draw(const BitmapFont& font, const std::string& textLine, const DrawTransform& transform, const ColorParams& colorParams, const UVParams& uvParams);


	//Deprecated for the other games
	void Draw(const BMPImage& image, const Sprite& sprite, const Vec2D& pos, float angle = 0.0f, float scale = 1.0f, const Color& overlayColor = Color::White(), float alpha = 1.0f, bool bilinearFilter = false);
	void Draw(const SpriteSheet& ss, const std::string& spriteName, const Vec2D& pos, float angle = 0.0f, float scale = 1.0f, const Color& overlayColor = Color::White(), float alpha = 1.0f, bool bilinearFilter = false);
	void Draw(const BitmapFont& font, const std::string& textLine, const Vec2D& pos, const Color& overlayColor = Color::White(), float scale = 1.0f, float alpha = 1.0f, bool bilinearFilter = false);
	void DrawBackground(const BMPImage& image, const Sprite& sprite, const Vec2D& pos, const Color& overlayColor = Color::White());



	void ClearBackground();

	//Screen shake
	void Shake(float power, float durationInSeconds);

private:

	Screen(const Screen& screen);
	Screen& operator=(const Screen& screen);

	void ClearScreen();

	void SetPixel(ScreenBuffer& screenBuffer, const Color& color, int x, int y);
	

	using FillPolyFunc = std::function<Color (uint32_t x, uint32_t y)>;

	void FillPoly(const std::vector<Vec2D>& points, FillPolyFunc func);

	void FillPolySprite(
		ScreenBuffer& screenBuffer,
		const std::vector<Vec2D>& points,
		const std::vector<Color>& imagePixels,
		uint32_t imageWidth,
		const float overlayColor[4],
		const Vec2D& spritePos,
		const Vec2D& spriteSize,
		const Vec2D& xAxis,
		const Vec2D& yAxis,
		const float invXAxisLengthSq,
		const float invYAxisLengthSq,
		float alpha,
		bool bilinearFilter,
		const GradientParams& gradient,
		const UVParams& uvParams);

	void DrawBilinearFilteredPixel(
		ScreenBuffer& screenBuffer,
		const std::vector<Color>& imagePixels,
		int pixelX,
		int pixelY,
		const Vec2D& uv,
		uint32_t imageWidth,
		const Vec2D& spriteSize,
		const Vec2D& spritePos,
		const float overlayColor[4], 
		float alpha,
		const GradientParams& gradient,
		const UVParams& uvParams);

	void DrawUnfilteredPixel(
		ScreenBuffer& screenBuffer,
		const std::vector<Color>& imagePixels,
		int pixelX,
		int pixelY,
		const Vec2D& uv,
		uint32_t imageWidth,
		const Vec2D& spriteSize,
		const Vec2D& spritePos,
		const float overlayColor[4],
		float alpha,
		const GradientParams& gradient,
		const UVParams& uvParams);


	void Gradient(const GradientParams& gradient, float u, float v, float overlayColor[4]);
	void ClipUV(const Vec2D& uv, const UVParams& uvParams, Color& imageColor);

	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mMagnification;

	Color mClearColor;
	ScreenBuffer mBackBuffer;
	ScreenBuffer mBackgroundBuffer;

	SDL_Window* moptrWindow;
	SDL_Surface* mnoptrWindowSurface;

	SDL_Renderer* mRenderer;
	SDL_PixelFormat* mPixelFormat;
	SDL_Texture* mTexture;
	SDL_Texture* mBackgroundTexture;
	bool mFast;

	//Screen Shake
	int mScreenShakeTimer; // in milliseconds
	float mScreenShakePower;
	Vec2D mScreenShakeOffset;
};



#endif /* GRAPHICS_SCREEN_H_ */
