#ifndef __RAY2D_H__
#define __RAY2D_H__

#include "Vec2D.h"
#include "Line2D.h"
#include "Easings.h"

class Circle;

class Ray2D
{
public:

	Ray2D();

	void Init(const Vec2D& origin, const Vec2D& velocity);

	Line2D GetLineSegmentForTime(float t, ease::EasingFunc func = ease::EaseLinear) const;

	bool Intersects(const Circle& circle, float& t1, float& t2) const;

	const Vec2D& GetVelocity() const { return mVelocity; }
	const Vec2D& GetOrigin() const { return mOrigin; }

private:
	Vec2D mOrigin;
	Vec2D mVelocity;
};

#endif // __RAY2D_H__
