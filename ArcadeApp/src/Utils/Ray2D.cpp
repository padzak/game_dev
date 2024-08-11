#include "Ray2D.h"
#include "Circle.h"
#include "Utils.h"
#include <cmath>

Ray2D::Ray2D()
	:mOrigin(Vec2D::Zero)
	,mVelocity(Vec2D::Zero)
{
}

void Ray2D::Init(const Vec2D& origin, const Vec2D& velocity)
{
	mOrigin = origin;
	mVelocity = velocity;
}

Line2D Ray2D::GetLineSegmentForTime(float t, ease::EasingFunc func /*= ease::EaseLinear*/) const
{
	if (t > 0)
	{
		Line2D line;
		Vec2D p2 = mOrigin + mVelocity * func(t);

		line.SetP0(mOrigin);
		line.SetP1(p2);

		return line;
	}

	return Line2D();
}

bool Ray2D::Intersects(const Circle& circle, float& t1, float& t2) const
{
	Vec2D OMinusC = mOrigin - circle.GetCenterPoint();
	float OMinusC2 = OMinusC.Dot(OMinusC);
	float Vel2 = mVelocity.Dot(mVelocity);
	float VelDotOMinusC = mVelocity.Dot(OMinusC);
	float Radius2 = circle.GetRadius() * circle.GetRadius();

	float disc = VelDotOMinusC * VelDotOMinusC - Vel2 * (OMinusC2 - Radius2);

	if (disc < 0.0f)
	{
		//no solution -> no intersection
		return false;
	}

	float sqrtDisc = std::sqrt(disc);
	t1 = (-VelDotOMinusC - sqrtDisc) / Vel2;
	t2 = (-VelDotOMinusC + sqrtDisc) / Vel2;

	return true;
}
