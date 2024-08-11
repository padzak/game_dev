/*
 * Utils.cpp
 *
 *  Created on: Jan. 8, 2019
 *      Author: serge
 */

#include "Utils.h"
#include <cmath>
#include <algorithm>
#include <cctype>
#include <SDL2/SDL.h>
#include "Vec2D.h"

bool IsEqual(float x, float y)
{
	return fabsf(x - y) < EPSILON;
}

bool IsGreaterThanOrEqual(float x, float y)
{
	return x > y || IsEqual(x, y);
}

bool IsLessThanOrEqual(float x, float y)
{
	return x < y || IsEqual(x, y);
}

float MillisecondsToSeconds(unsigned int milliseconds)
{
	return static_cast<float>(milliseconds) / 1000.0f;
}

uint32_t SecondsToMilliseconds(float seconds)
{
	seconds = abs(seconds); //make sure it's positive!
	return static_cast<uint32_t>(seconds * 1000.f);
}

unsigned int GetIndex(unsigned int width, unsigned int r, unsigned int c)
{
	return r * width + c;
}

bool StringCompare(const std::string& a, const std::string& b)
{
	if(a.length() == b.length())
	{
		return std::equal(b.begin(), b.end(), a.begin(), [](unsigned char a, unsigned char b){
			return std::tolower(a) == std::tolower(b);
		});
	}

	return false;
}

float Clamp(float val, float min, float max)
{
	if(val > max)
	{
		return max;
	}
	else if(val < min)
	{
		return min;
	}

	return val;
}

uint8_t Clamp8(uint8_t val, uint8_t min, uint8_t max)
{
	if (val > max)
	{
		return max;
	}
	else if (val < min)
	{
		return min;
	}

	return val;
}

float Lerpf(float val1, float val2, float t)
{
	return (1.0f - t) * val1 + t * val2;
}

uint32_t LerpInt(uint32_t val1, uint32_t val2, float t)
{
	float result = (1.0f - t) * (float)val1 + t * (float)val2;
	return static_cast<uint32_t>(roundf(result));
}

float Pulse(float frequency)
{
	//Amplitude * sin(frequency * (x + xdisplacement)) + ydisplacement
	return 0.25f * sinf(frequency * PI * MillisecondsToSeconds(SDL_GetTicks())) + 0.75f;
}

Vec2D ConvertWorldSpaceToUVSpace(const Vec2D& worldPosition, const Vec2D& refPoint, const Vec2D& xAxis, const Vec2D& yAxis, float invXAxisLengthSq, float invYAxisLengthSq)
{
	Vec2D d = worldPosition - refPoint;

	float u = invXAxisLengthSq * d.Dot(xAxis);
	float v = invYAxisLengthSq * d.Dot(yAxis);

	u = Clamp(u, 0.0f, 1.0f);
	v = Clamp(v, 0.0f, 1.0f);

	return Vec2D(u, v);
}

void GetObjectAxis(const Vec2D& tl , uint32_t width, uint32_t height, float rotation, float scale, Vec2D& xAxis, Vec2D& yAxis, float& invXAxisLengthSq, float& invYAxisLengthSq, std::vector<Vec2D>& points)
{	
	Vec2D topLeft = tl;
	auto topRight = topLeft + Vec2D(width * scale, 0);
	auto bottomLeft = topLeft + Vec2D(0, height * scale);
	auto bottomRight = topLeft + Vec2D(width * scale, height * scale);

	Vec2D center = topLeft + (Vec2D(float(width) / 2.0f, float(height) / 2.0f)) * scale;

	topLeft.Rotate(rotation, center);
	topRight.Rotate(rotation, center);
	bottomLeft.Rotate(rotation, center);
	bottomRight.Rotate(rotation, center);

	xAxis = topRight - topLeft;
	yAxis = bottomLeft - topLeft;

	invXAxisLengthSq = 1.0f / xAxis.Mag2();
	invYAxisLengthSq = 1.0f / yAxis.Mag2();

	points = { topLeft, bottomLeft, bottomRight, topRight };
}