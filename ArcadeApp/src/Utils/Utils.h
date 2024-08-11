/*
 * Utils.h
 *
 *  Created on: Jan. 8, 2019
 *      Author: serge
 */

#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>

class Vec2D;


static const float EPSILON = 0.0001f;

const float PI = 3.14159f;
const float TWO_PI = 2.0f * PI;

bool IsEqual(float x, float y);

bool IsGreaterThanOrEqual(float x, float y);

bool IsLessThanOrEqual(float x, float y);

float MillisecondsToSeconds(unsigned int milliseconds);

uint32_t SecondsToMilliseconds(float seconds);

float Pulse(float frequency);

unsigned int GetIndex(unsigned int width, unsigned int r, unsigned int c);
//string == string
bool StringCompare(const std::string& a, const std::string& b);

float Clamp(float val, float min, float max);
uint8_t Clamp8(uint8_t val, uint8_t min, uint8_t max);

float Lerpf(float val1, float val2, float t);
uint32_t LerpInt(uint32_t val1, uint32_t val2, float t);

void GetObjectAxis(const Vec2D& tl, uint32_t width, uint32_t height, float rotation, float scale, Vec2D& xAxis, Vec2D& yAxis, float& invXAxisLengthSq, float& invYAxisLengthSq, std::vector<Vec2D>& points);
Vec2D ConvertWorldSpaceToUVSpace(const Vec2D& worldPosition, const Vec2D& refPoint, const Vec2D& xAxis, const Vec2D& yAxis, float invXAxisLengthSq, float invYAxisLengthSq);

struct Size
{
	unsigned int width = 0, height = 0;
};

#endif /* UTILS_UTILS_H_ */
