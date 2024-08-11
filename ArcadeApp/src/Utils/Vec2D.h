/*
 * Vec2D.h
 *
 *  Created on: Jan. 8, 2019
 *      Author: serge
 */

#ifndef UTILS_VEC2D_H_
#define UTILS_VEC2D_H_

#include <iostream>
#include "Easings.h"
#include <cmath>

class Vec2D {

	public:

	static const Vec2D Zero;

	Vec2D(): Vec2D(0,0){}
    Vec2D(float x, float y): mX(x), mY(y){}

    inline void SetX(float x) {mX = x;}
    inline void SetY(float y) {mY = y;}
    inline float GetX() const {return mX;}
    inline float GetY() const {return mY;}

    friend std::ostream& operator<<(std::ostream& consoleOut, const Vec2D& vec);

    bool operator==(const Vec2D& vec2) const;
    bool operator!=(const Vec2D& vec2) const;

    Vec2D operator-() const;
    Vec2D operator*(float scale) const;
    Vec2D operator/(float scale) const;
    Vec2D& operator*=(float scale);
    Vec2D& operator/=(float scale);

    Vec2D operator+(const Vec2D& vec) const;
    Vec2D operator-(const Vec2D& vec) const;
    Vec2D& operator+=(const Vec2D& vec);
    Vec2D& operator-=(const Vec2D& vec);

    inline float Mag2() const { return Dot(*this); }
    inline float Mag() const { return sqrtf(Mag2()); }

    Vec2D GetUnitVec() const;
    Vec2D& Normalize();

    float Distance(const Vec2D& vec) const;
    inline float Dot(const Vec2D& vec) const { return mX * vec.mX + mY * vec.mY; }

    Vec2D ProjectOnto(const Vec2D& vec2) const;

    float AngleBetween(const Vec2D& vec2) const;

    Vec2D Reflect(const Vec2D& normal) const;


    void Rotate(float angle, const Vec2D& aroundPoint = Zero);
    Vec2D RotationResult(float angle, const Vec2D& aroundPoint = Zero) const;


    float GetYSign() { return GetY() > 0 ? 1.0f : -1.0f; }
    float GetXSign() { return GetX() > 0 ? 1.0f : -1.0f; }
    friend Vec2D operator*(float scalar, const Vec2D& vec);

    Vec2D GetClockwisePerpendicularUnitVector() const;
    Vec2D GetCounterClockwisePerpendicularUnitVector() const;

    static Vec2D Lerp(const Vec2D& v1, const Vec2D& v2, float t, ease::EasingFunc func);



	private:
	float mX, mY;
};

#endif /* UTILS_VEC2D_H_ */
