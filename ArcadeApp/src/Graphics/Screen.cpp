/*
 * Screen.cpp
 *
 *  Created on: Jan. 10, 2019
 *      Author: serge
 */


#include "Screen.h"
#include "Vec2D.h"
#include "Line2D.h"
#include "Triangle.h"
#include "AARectangle.h"
#include "Circle.h"
#include "BMPImage.h"
#include "SpriteSheet.h"
#include "BitmapFont.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include "App.h"

namespace
{
	std::vector<std::string> s_preferredPixelFormats = {
		"SDL_PIXELFORMAT_ARGB8888",
		"SDL_PIXELFORMAT_RGBA8888",
		"SDL_PIXELFORMAT_BGRA8888"
	};
}

Screen::Screen()
	: mWidth(0)
	, mHeight(0)
	, mMagnification(1)
	, moptrWindow(nullptr)
	, mnoptrWindowSurface(nullptr)
	, mRenderer(nullptr)
	, mPixelFormat(nullptr)
	, mTexture(nullptr)
	, mBackgroundTexture(nullptr)
	, mFast(true)
	, mScreenShakeTimer(0)
	, mScreenShakePower(0)
	, mScreenShakeOffset(Vec2D::Zero)
{

}


Screen::~Screen()
{
	if(mPixelFormat)
	{
		SDL_FreeFormat(mPixelFormat);
		mPixelFormat = nullptr;
	}

	if(mTexture)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
	}

	if (mBackgroundTexture)
	{
		SDL_DestroyTexture(mBackgroundTexture);
		mBackgroundTexture = nullptr;
	}

	if(mRenderer)
	{
		SDL_DestroyRenderer(mRenderer);
		mRenderer = nullptr;
	}

	if(moptrWindow)
	{
		SDL_DestroyWindow(moptrWindow);
		moptrWindow = nullptr;
	}

	SDL_Quit();
}

SDL_Window* Screen::Init(uint32_t w, uint32_t h, uint32_t mag, bool fast)
{

	mFast = fast;

	if(SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "Error SDL_Init Failed" << std::endl;
		return nullptr;
	}

	mWidth = w;
	mHeight = h;
	mMagnification = mag;

	moptrWindow = SDL_CreateWindow("Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth * mag, mHeight * mag, 0);

	if(moptrWindow)
	{ 

		uint8_t rClear = 0;
		uint8_t gClear = 0;
		uint8_t bClear = 0;
		uint8_t aClear = 255;

		if(mFast)
		{
			mRenderer = SDL_CreateRenderer(moptrWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if(mRenderer == nullptr)
			{
				std::cout << "SDL_CreateRenderer failed" << std::endl;
				return nullptr;
			}



			SDL_SetRenderDrawColor(mRenderer, rClear, gClear, bClear, aClear);
		}
		else
		{
			mnoptrWindowSurface = SDL_GetWindowSurface(moptrWindow);
		}

		SDL_RendererInfo info;
		SDL_GetRendererInfo(mRenderer, &info);
		int32_t foundIndex = -1;

		for (Uint32 i = 0; i < info.num_texture_formats; i++)
		{
			auto iter = std::find(s_preferredPixelFormats.begin(), s_preferredPixelFormats.end(), std::string(SDL_GetPixelFormatName(info.texture_formats[i])));
			if (iter != s_preferredPixelFormats.end())
			{
				foundIndex = i;
				break;
			}
		}

		assert(foundIndex != -1);
		mPixelFormat = SDL_AllocFormat(info.texture_formats[foundIndex]);

		if(mFast)
		{
			mTexture = SDL_CreateTexture(mRenderer, mPixelFormat->format, SDL_TEXTUREACCESS_STREAMING, w, h);
			mBackgroundTexture = SDL_CreateTexture(mRenderer, mPixelFormat->format, SDL_TEXTUREACCESS_STREAMING, w, h);
			SDL_SetTextureBlendMode(mBackgroundTexture, SDL_BLENDMODE_NONE);
			SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		}

		Color::InitColorFormat(mPixelFormat);

		

		mBackBuffer.Init(mPixelFormat->format, mWidth, mHeight);

		mBackBuffer.Clear();

		mBackgroundBuffer.Init(mPixelFormat->format, mWidth, mHeight);
		mBackgroundBuffer.Clear();
	}


	return moptrWindow;

}

void Screen::Update(uint32_t dt)
{
	if (mScreenShakeTimer > 0)
	{
		mScreenShakeTimer -= int(dt);

		if (mScreenShakeTimer <= 0)
		{
			mScreenShakeTimer = 0;
		}

		//Section 2 - Exercise 1
		//TODO: implement
	}
	else
	{
		mScreenShakeOffset = Vec2D::Zero;
	}
}

void Screen::SwapScreens()
{
	assert(moptrWindow);
	if(moptrWindow)
	{
		ClearScreen();

		if(mFast)
		{
			uint8_t* textureData = nullptr;
			int texturePitch = 0;
			SDL_Rect destRect;
			destRect.x = static_cast<int>(round(mScreenShakeOffset.GetX()));
			destRect.y = static_cast<int>(round(mScreenShakeOffset.GetY()));

			destRect.w = mWidth * mMagnification;
			destRect.h = mHeight * mMagnification;

			if (SDL_LockTexture(mBackgroundTexture, nullptr, (void**)&textureData, &texturePitch) >= 0)
			{
				SDL_Surface* surface = mBackgroundBuffer.GetSurface();

				memcpy(textureData, surface->pixels, static_cast<size_t>(surface->w) * static_cast<size_t>(surface->h) * static_cast<size_t>(mPixelFormat->BytesPerPixel));

				SDL_UnlockTexture(mBackgroundTexture);


				SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &destRect);
			}

			if(SDL_LockTexture(mTexture, nullptr, (void**)&textureData, &texturePitch) >= 0)
			{
				SDL_Surface* surface = mBackBuffer.GetSurface();

				memcpy(textureData, surface->pixels, static_cast<size_t>(surface->w) * static_cast<size_t>(surface->h) * static_cast<size_t>(mPixelFormat->BytesPerPixel));

				SDL_UnlockTexture(mTexture);

				SDL_RenderCopy(mRenderer, mTexture, nullptr, &destRect);
			}

			SDL_RenderPresent(mRenderer);
		}
		else
		{
			SDL_BlitScaled(mBackBuffer.GetSurface(), nullptr, mnoptrWindowSurface, nullptr);

			SDL_UpdateWindowSurface(moptrWindow);
		}

		mBackBuffer.Clear();
	}
}

void Screen::Draw(int x, int y, const Color& color)
{
	assert(moptrWindow);
	if(moptrWindow)
	{
		SetPixel(mBackBuffer, color, x, y);
	}
}

void Screen::Draw(const Vec2D& point, const Color& color)
{
	assert(moptrWindow);
	if(moptrWindow)
	{
		SetPixel(mBackBuffer, color, static_cast<int>(point.GetX()), static_cast<int>(point.GetY()));
	}
}

void Screen::Draw(const Line2D& line, const Color& color)
{
	assert(moptrWindow);
	if(moptrWindow)
	{
		int dx, dy;

		int x0 = static_cast<int>(roundf(line.GetP0().GetX()));
		int y0 = static_cast<int>(roundf(line.GetP0().GetY()));
		int x1 = static_cast<int>(roundf(line.GetP1().GetX()));
		int y1 = static_cast<int>(roundf(line.GetP1().GetY()));

		dx = x1 - x0;
		dy = y1 - y0;

		signed const char ix((dx > 0) - (dx < 0)); // evaluate to 1 or -1
		signed const char iy((dy > 0) - (dy < 0));

		dx = abs(dx) * 2;
		dy = abs(dy) * 2;

		Draw(x0, y0, color);

		if(dx >= dy)
		{
			//go along in the x

			int d = dy - dx/2;

			while(x0 != x1)
			{
				if(d >= 0)
				{
					d -= dx;
					y0 += iy;
				}

				d += dy;
				x0 += ix;

				Draw(x0, y0, color);
			}
		}
		else
		{
			//go along in y
			int d = dx - dy/2;

			while(y0 != y1)
			{
				if(d >= 0)
				{
					d -= dy;
					x0 += ix;
				}

				d += dx;
				y0 += iy;

				Draw(x0, y0, color);
			}
		}
	}
}

void Screen::Draw(const Triangle& triangle, const Color& color, bool fill, const Color& fillColor)
{
	if(fill)
	{
		FillPoly(triangle.GetPoints(), [fillColor](uint32_t x, uint32_t y){return fillColor;});
	}

	Line2D p0p1 = Line2D(triangle.GetP0(), triangle.GetP1());
	Line2D p1p2 = Line2D(triangle.GetP1(), triangle.GetP2());
	Line2D p2p0 = Line2D(triangle.GetP2(), triangle.GetP0());

	Draw(p0p1, color);
	Draw(p1p2, color);
	Draw(p2p0, color);
}

void Screen::Draw(const AARectangle& rect, const Color& color, bool fill, const Color& fillColor)
{
	if(fill)
	{
		FillPoly(rect.GetPoints(), [fillColor](uint32_t x, uint32_t y){return fillColor;});
	}
	std::vector<Vec2D> points = rect.GetPoints();

	Line2D p0p1 = Line2D(points[0], points[1]);
	Line2D p1p2 = Line2D(points[1], points[2]);
	Line2D p2p3 = Line2D(points[2], points[3]);
	Line2D p3p0 = Line2D(points[3], points[0]);

	Draw(p0p1, color);
	Draw(p1p2, color);
	Draw(p2p3, color);
	Draw(p3p0, color);
}

void Screen::Draw(const Circle& circle, const Color& color, bool fill, const Color& fillColor)
{
	static unsigned int NUM_CIRCLE_SEGMENTS = 30;

	std::vector<Vec2D> circlePoints;
	std::vector<Line2D> lines;

	float angle = TWO_PI / float(NUM_CIRCLE_SEGMENTS);

	Vec2D p0 = Vec2D(circle.GetCenterPoint().GetX() + circle.GetRadius(), circle.GetCenterPoint().GetY());
	Vec2D p1 = p0;
	Line2D nextLineToDraw;
	//circlePoints.push_back(p0);

	for(unsigned int i = 0; i < NUM_CIRCLE_SEGMENTS; ++i)
	{
		p1.Rotate(angle, circle.GetCenterPoint());
		nextLineToDraw.SetP1(p1);
		nextLineToDraw.SetP0(p0);

		lines.push_back(nextLineToDraw);
		p0 = p1;
		circlePoints.push_back(p0);
	}

	if(fill)
	{
		FillPoly(circlePoints, [fillColor](uint32_t x, uint32_t y){return fillColor;});
	}

	for(const Line2D& line : lines)
	{
		Draw(line, color);
	}
}

void Screen::Draw(const SpriteSheet& ss, const std::string& spriteName, const DrawTransform& transform, const ColorParams& colorParams, const UVParams& uvParams)
{
	Draw(ss.GetBMPImage(), ss.GetSprite(spriteName), transform, colorParams, uvParams);
}

void Screen::Draw(const BMPImage& image, const Sprite& sprite, const DrawTransform& transform, const ColorParams& colorParams, const UVParams& uvParams, DrawSurface drawSurface)
{
	float normalizedOverlayColor[4];
	normalizedOverlayColor[0] = static_cast<float>(colorParams.overlay.GetRed()) / 255.0f;
	normalizedOverlayColor[1] = static_cast<float>(colorParams.overlay.GetGreen()) / 255.0f;
	normalizedOverlayColor[2] = static_cast<float>(colorParams.overlay.GetBlue()) / 255.0f;
	normalizedOverlayColor[3] = static_cast<float>(colorParams.overlay.GetAlpha()) / 255.0f;

	const std::vector<Color>& pixels = image.GetPixels();

	std::vector<Vec2D> points;

	Vec2D xAxis;
	Vec2D yAxis;

	float invXAxisLengthSq;
	float invYAxisLengthSq;

	GetObjectAxis(transform.pos, sprite.width, sprite.height, transform.rotationAngle, transform.scale, xAxis, yAxis, invXAxisLengthSq, invYAxisLengthSq, points);

	ScreenBuffer* screenBufferPtr = &mBackBuffer;
	if (drawSurface == BACKGROUND)
	{
		screenBufferPtr = &mBackgroundBuffer;
	}

	FillPolySprite(
		*screenBufferPtr,
		points,
		pixels,
		image.GetWidth(),
		normalizedOverlayColor,
		Vec2D(static_cast<float>(sprite.xPos), static_cast<float>(sprite.yPos)),
		Vec2D(static_cast<float>(sprite.width), static_cast<float>(sprite.height)),
		xAxis, yAxis,
		invXAxisLengthSq, invYAxisLengthSq,
		colorParams.alpha,
		colorParams.bilinearFiltering, colorParams.gradient, uvParams);
}

void Screen::Draw(const BitmapFont& font, const std::string& textLine, const DrawTransform& transform, const ColorParams& colorParams, const UVParams& uvParams)
{
	uint32_t xPos = static_cast<uint32_t>(transform.pos.GetX());

	const SpriteSheet& ss = font.GetSpriteSheet();

	for (char c : textLine)
	{
		if (c == ' ')
		{
			xPos += font.GetFontSpacingBetweenWords();
			continue;
		}

		Sprite sprite = ss.GetSprite(std::string("") + c);

		DrawTransform newTransform = transform;
		newTransform.pos = Vec2D(static_cast<float>(xPos), transform.pos.GetY());

		Draw(ss.GetBMPImage(), sprite, newTransform, colorParams, uvParams);

		xPos += sprite.width;

		xPos += font.GetFontSpacingBetweenLetters();
	}
}

void Screen::Draw(const BMPImage& image, const Sprite& sprite, const Vec2D& pos, float angle, float scale, const Color& overlayColor, float alpha, bool bilinearFilter)
{
	DrawTransform transform;
	transform.pos = pos;
	transform.scale = scale;
	transform.rotationAngle = angle;

	ColorParams colorParams;
	colorParams.alpha = alpha;
	colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
	colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;
	colorParams.bilinearFiltering = bilinearFilter;
	colorParams.overlay = overlayColor;

	UVParams uvParams;

	Draw(image, sprite, transform, colorParams, uvParams);
}

void Screen::DrawBackground(const BMPImage& image, const Sprite& sprite, const Vec2D& pos, const Color& overlayColor)
{
	DrawTransform transform;
	transform.pos = pos;
	transform.scale = 1.0f;
	transform.rotationAngle = 0.0f;

	ColorParams colorParams;
	colorParams.alpha = 1.0f;
	colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
	colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;
//	colorParams.gradient.color1 = Color::White();
//	colorParams.gradient.color2 = Color::Blue();

	colorParams.bilinearFiltering = false;
	colorParams.overlay = overlayColor; 

	UVParams uvParams;

	Draw(image, sprite, transform, colorParams, uvParams, BACKGROUND);
}

void Screen::Draw(const SpriteSheet& ss, const std::string& spriteName, const Vec2D& pos, float angle, float scale, const Color& overlayColor, float alpha, bool bilinearFilter)
{
	Draw(ss.GetBMPImage(), ss.GetSprite(spriteName), pos, angle, scale, overlayColor, alpha, bilinearFilter);
}

void Screen::Draw(const BitmapFont& font, const std::string& textLine, const Vec2D& atPosition, const Color& overlayColor, float scale, float alpha, bool bilinearFilter)
{
	DrawTransform transform;

	transform.pos = atPosition;
	transform.rotationAngle = 0.0f;
	transform.scale = scale;

	ColorParams colorParams;
	colorParams.alpha = alpha;
	colorParams.bilinearFiltering = bilinearFilter;
	colorParams.overlay = overlayColor;
	colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
	colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;

	UVParams uvParams;

	Draw(font, textLine, transform, colorParams, uvParams);
}

void Screen::ClearScreen()
{
	assert(moptrWindow);
	if(moptrWindow)
	{
		if(mFast)
		{
			SDL_RenderClear(mRenderer);
		}
		else
		{
			SDL_FillRect(mnoptrWindowSurface, nullptr, mClearColor.GetPixelColor());
		}
	}
}

void Screen::ClearBackground()
{
	mBackgroundBuffer.Clear(mClearColor);
}


void Screen::Shake(float power, float durationInSeconds)
{
	mScreenShakeTimer = SecondsToMilliseconds(durationInSeconds);
	mScreenShakePower = power;
}

void Screen::FillPoly(const std::vector<Vec2D>& points, FillPolyFunc func)
{
	if(points.size() > 0)
	{
		float top = points[0].GetY();
		float bottom = points[0].GetY();
		float right = points[0].GetX();
		float left = points[0].GetX();

		for(size_t i = 1; i < points.size(); ++i)
		{
			if(points[i].GetY() < top)
			{
				top = points[i].GetY();
			}

			if(points[i].GetY() > bottom)
			{
				bottom = points[i].GetY();
			}

			if(points[i].GetX() < left)
			{
				left = points[i].GetX();
			}

			if(points[i].GetX() > right)
			{
				right= points[i].GetX();
			}
		}


		for(int pixelY = (int)roundf(top); pixelY < (int)roundf(bottom); ++pixelY)
		{
			std::vector<float> nodeXVec;

			size_t j = points.size() -1;

			for(size_t i = 0; i < points.size(); ++i)
			{
				float pointiY = points[i].GetY();
				float pointjY = points[j].GetY();


				if((pointiY <= pixelY && pointjY > pixelY) || (pointjY <= pixelY && pointiY > pixelY))
				{
					float denom = pointjY - pointiY;
					if(IsEqual(denom, 0))
					{
						continue;
					}

					float x = points[i].GetX() + (pixelY - pointiY)/(denom) * (points[j].GetX() - points[i].GetX());
					nodeXVec.push_back(x);
				}

				j = i;
			}

			std::sort(nodeXVec.begin(), nodeXVec.end(), std::less<>());

			for(size_t k = 0; k < nodeXVec.size(); k+=2)
			{
				if(nodeXVec[k] > right)
				{
					break;
				}

				if(nodeXVec[k+1] > left)
				{
					if(nodeXVec[k] < left)
					{
						nodeXVec[k] = left;
					}
					if(nodeXVec[k+1] > right)
					{
						nodeXVec[k+1] = right;
					}

					for(int pixelX = (int)roundf(nodeXVec[k]); pixelX < (int)roundf(nodeXVec[k+1]); ++pixelX)
					{
						Draw(pixelX, pixelY, func(pixelX,pixelY));
					}
				}
			}
		}
	}
}

void Screen::SetPixel(ScreenBuffer& screenBuffer, const Color& color, int x, int y)
{
	if (!(y >= 0 && y < static_cast<int>(mHeight) && x >= 0 && x < static_cast<int>(mWidth)))
	{
		return;
	}

	Color surfaceColor;

	SDL_Surface* backgroundSurface = mBackgroundBuffer.GetSurface();
	SDL_Surface* backBufferSurface = mBackBuffer.GetSurface();

	if (screenBuffer.GetSurface() == backBufferSurface)
	{
		if (backgroundSurface)
		{
			assert(backBufferSurface);

			uint32_t* pixels = (uint32_t*)backgroundSurface->pixels;
			uint32_t index = GetIndex(backgroundSurface->w, y, x);
			surfaceColor = Color(pixels[index]);

			uint32_t* backBufferPixels = (uint32_t*)backBufferSurface->pixels;
			Color backBufferColor = Color(backBufferPixels[index]);

			surfaceColor = Color::Evaluate1MinusSourceAlpha(backBufferColor, surfaceColor);
		}
		else
		{
			assert(backBufferSurface);
			uint32_t* pixels = (uint32_t*)backBufferSurface->pixels;
			uint32_t index = GetIndex(backBufferSurface->w, y, x);
			surfaceColor = Color(pixels[index]);
		}
	}
	else
	{
		assert(backgroundSurface);
		uint32_t* pixels = (uint32_t*)backgroundSurface->pixels;
		uint32_t index = GetIndex(backgroundSurface->w, y, x);
		surfaceColor = Color(pixels[index]);
	}
	

	screenBuffer.SetPixel(color, surfaceColor, x, y);
}

void Screen::DrawBilinearFilteredPixel(
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
	const UVParams& uvParams)
{
	float tx = 1.0f + uv.GetX() * static_cast<float>(spriteSize.GetX() - 3.0f);
	float ty = 1.0f + uv.GetY() * static_cast<float>(spriteSize.GetY() - 3.0f);

	uint32_t txInt = (uint32_t)tx;
	uint32_t tyInt = (uint32_t)ty;

	float fx = tx - static_cast<float>(txInt);
	float fy = ty - static_cast<float>(tyInt);


	unsigned int row = static_cast<unsigned int>(ty + spritePos.GetY());
	unsigned int col = static_cast<unsigned int>(tx + spritePos.GetX());

	auto pixelIndex = GetIndex(imageWidth, row, col);
	auto pixelIndex2 = GetIndex(imageWidth, row, col + 1);
	auto pixelIndex3 = GetIndex(imageWidth, row + 1, col);
	auto pixelIndex4 = GetIndex(imageWidth, row + 1, col + 1);

	if (pixelIndex < imagePixels.size() &&
		pixelIndex2 < imagePixels.size() &&
		pixelIndex3 < imagePixels.size() &&
		pixelIndex4 < imagePixels.size())
	{
		Color imageColor = imagePixels[pixelIndex];
		Color imageColor2 = imagePixels[pixelIndex2];
		Color imageColor3 = imagePixels[pixelIndex3];
		Color imageColor4 = imagePixels[pixelIndex4];

		Color result = Color::Lerp(Color::Lerp(imageColor, imageColor2, fx), Color::Lerp(imageColor3, imageColor4, fx), fy);

		float newOverlayColor[4] = { overlayColor[0], overlayColor[1], overlayColor[2], overlayColor[3] };

		Gradient(gradient, uv.GetX(), uv.GetY(), newOverlayColor);
		
		ClipUV(uv, uvParams, result);

		Color newColor = {
			static_cast<uint8_t>(result.GetRed() * newOverlayColor[0]),
			static_cast<uint8_t>(result.GetGreen() * newOverlayColor[1]),
			static_cast<uint8_t>(result.GetBlue() * newOverlayColor[2]),
			static_cast<uint8_t>(result.GetAlpha() * newOverlayColor[3] * alpha) };

		SetPixel(screenBuffer, newColor, pixelX, pixelY);
	}
}

void Screen::DrawUnfilteredPixel(
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
	const UVParams& uvParams)
{
	float tx = roundf(uv.GetX() * spriteSize.GetX());
	float ty = roundf(uv.GetY() * spriteSize.GetY());

	unsigned int row = static_cast<unsigned int>(ty + spritePos.GetY());
	unsigned int col = static_cast<unsigned int>(tx + spritePos.GetX());

	auto pixelIndex = GetIndex(imageWidth, row, col);

	if (pixelIndex < imagePixels.size())
	{
		float newOverlayColor[4] = { overlayColor[0], overlayColor[1], overlayColor[2], overlayColor[3] };

		Gradient(gradient, uv.GetX(), uv.GetY(), newOverlayColor);

		Color imageColor = imagePixels[pixelIndex];

		ClipUV(uv, uvParams, imageColor);

		Color newColor = {
			static_cast<uint8_t>(imageColor.GetRed() * newOverlayColor[0]),
			static_cast<uint8_t>(imageColor.GetGreen() * newOverlayColor[1]),
			static_cast<uint8_t>(imageColor.GetBlue() * newOverlayColor[2]),
			static_cast<uint8_t>(imageColor.GetAlpha() * newOverlayColor[3] * alpha) };
		
		SetPixel(screenBuffer, newColor, pixelX, pixelY);
	}
}

void Screen::Gradient(const GradientParams& gradient, float u, float v, float overlayColor[4])
{
	if (gradient.xParam == GradientXParam::NO_X_GRADIENT && gradient.yParam == GradientYParam::NO_Y_GRADIENT)
		return;

	Color gradientOverlayColor = Color::White();
	Color xOverlay = Color::White();
	Color yOverlay = Color::White();

	//Section 3 - EXERCISE 2
	{
		
	}
	

	overlayColor[0] = static_cast<float>(gradientOverlayColor.GetRed()) / 255.0f;
	overlayColor[1] = static_cast<float>(gradientOverlayColor.GetGreen()) / 255.0f;
	overlayColor[2] = static_cast<float>(gradientOverlayColor.GetBlue()) / 255.0f;
	overlayColor[3] = static_cast<float>(gradientOverlayColor.GetAlpha()) / 255.0f;
}

void Screen::ClipUV(const Vec2D& uv, const UVParams& uvParams, Color& imageColor)
{
	//Section 4 - Exercise 1
	//TODO: implement

	//if(we shouldn't draw this part of the texture)
	//{
	//	imageColor = Color::ClearBlack();
	//}
}

void Screen::FillPolySprite(
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
	const UVParams& uvParams) 
{
	if (points.size() > 0)
	{
		float top = points[0].GetY();
		float bottom = points[0].GetY();
		float right = points[0].GetX();
		float left = points[0].GetX();

		for (size_t i = 1; i < points.size(); ++i)
		{
			if (points[i].GetY() < top)
			{
				top = points[i].GetY();
			}

			if (points[i].GetY() > bottom)
			{
				bottom = points[i].GetY();
			}

			if (points[i].GetX() < left)
			{
				left = points[i].GetX();
			}

			if (points[i].GetX() > right)
			{
				right = points[i].GetX();
			}
		}


		for (int pixelY = (int)roundf(top); pixelY < (int)roundf(bottom); ++pixelY)
		{
			std::vector<float> nodeXVec;

			size_t j = points.size() - 1;

			for (size_t i = 0; i < points.size(); ++i)
			{
				float pointiY = points[i].GetY();
				float pointjY = points[j].GetY();


				if ((pointiY <= (float)pixelY && pointjY > (float)pixelY) || (pointjY <= (float)pixelY && pointiY > (float)pixelY))
				{
					float denom = pointjY - pointiY;
					if (IsEqual(denom, 0))
					{
						continue;
					}

					float x = points[i].GetX() + (pixelY - pointiY) / (denom) * (points[j].GetX() - points[i].GetX());
					nodeXVec.push_back(x);
				}

				j = i;
			}

			std::sort(nodeXVec.begin(), nodeXVec.end(), std::less<>());

			for (size_t k = 0; k < nodeXVec.size(); k += 2)
			{
				if (nodeXVec[k] > right)
				{
					break;
				}

				if (k + 1 >= nodeXVec.size())
				{
					break;
				}

				if (nodeXVec[k + 1] > left)
				{
					if (nodeXVec[k] < left)
					{
						nodeXVec[k] = left;
					}
					if (nodeXVec[k + 1] > right)
					{
						nodeXVec[k + 1] = right;
					}

					for (int pixelX = (int)roundf(nodeXVec[k]); pixelX < (int)roundf(nodeXVec[k + 1]); ++pixelX)
					{
						Vec2D p = { static_cast<float>(pixelX), static_cast<float>(pixelY) };

						Vec2D uv = ConvertWorldSpaceToUVSpace(p, points[0], xAxis, yAxis, invXAxisLengthSq, invYAxisLengthSq);

						if (!bilinearFilter)
						{
							DrawUnfilteredPixel(
								screenBuffer,
								imagePixels,
								pixelX,
								pixelY,
								uv,
								imageWidth,
								spriteSize,
								spritePos,
								overlayColor, 
								alpha,
								gradient,
								uvParams);
						}
						else
						{
							DrawBilinearFilteredPixel(
								screenBuffer,
								imagePixels,
								pixelX,
								pixelY,
								uv,
								imageWidth,
								spriteSize,
								spritePos,
								overlayColor,
								alpha,
								gradient,
								uvParams);
						}
					}
				}
			}
		}
	}
}
