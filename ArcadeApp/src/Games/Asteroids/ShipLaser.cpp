#include "ShipLaser.h"
#include "Utils.h"
#include "Screen.h"
#include "App.h"

ShipLaser::ShipLaser(const Vec2D& origin, const Vec2D& velocity, const Color& color1, const Color& color2, bool cutting, ease::EasingFunc easingFunction)
	:mColor1(color1)
	,mColor2(color2)
	,mEasingFunc(easingFunction)
	,mTime(0)
	,mCutting(cutting)
{
	mLaserRay.Init(origin, velocity);
}


void ShipLaser::Update(uint32_t dt)
{
	mTime += dt; 
}

void ShipLaser::Draw(Screen& theScreen)
{
	float t = Pulse(5);
	Color c = Color::Lerp(mColor1, mColor2, t);

	Line2D laser = mLaserRay.GetLineSegmentForTime(MillisecondsToSeconds(mTime), mEasingFunc);

	if (mCutting)
	{
		Vec2D offset = (laser.GetP1() - laser.GetP0()).GetCounterClockwisePerpendicularUnitVector();

		Line2D laserLeft = Line2D(laser.GetP0() - offset, laser.GetP1() - offset);
		Line2D laserRight = Line2D(laser.GetP0() + offset, laser.GetP1() + offset);

		theScreen.Draw(laser, Color::White());
		theScreen.Draw(laserLeft, c);
		theScreen.Draw(laserRight, c);
	}
	else
	{
		theScreen.Draw(laser, c);
	}
}

ShipAmmo::Type ShipLaser::GetType() const
{
	return ShipAmmo::Type::LASER;
}

bool ShipLaser::IsOutOfBounds(int leftSide, int top, int screenWidth, int screenHeight) const
{
	constexpr int LASER_FUDGE_FACTOR = 50;
	leftSide -= LASER_FUDGE_FACTOR;
	top -= LASER_FUDGE_FACTOR;
	screenWidth += LASER_FUDGE_FACTOR;
	screenHeight += LASER_FUDGE_FACTOR;

	Vec2D laserEndPosition = MillisecondsToSeconds(mTime) * mLaserRay.GetVelocity() + mLaserRay.GetOrigin();
	return (laserEndPosition.GetX() > screenWidth || laserEndPosition.GetX() < leftSide ||
		laserEndPosition.GetY() > screenHeight || laserEndPosition.GetY() < top);
}

bool ShipLaser::Intersects(const Circle& circle, std::vector<Vec2D>& intersections) const
{
	float t1 = -1.0f;
	float t2 = -1.0f;

	bool intersects = mLaserRay.Intersects(circle, t1, t2);

	if (!intersects)
	{
		return false;
	}

	float currentTime = MillisecondsToSeconds(mTime);
	float intersectTime1 = -1.0f;
	float intersectTime2 = -1.0f;
	int numIntersections = 0;

	if (t1 > 0 && t2 > 0)
	{
		intersectTime1 = std::min(t1, t2);
		intersectTime2 = std::max(t1, t2);
	}
	else if (t1 > 0 && t2 < 0)
	{
		intersectTime1 = t1;
	}
	else if (t2 > 0 && t1 < 0)
	{
		intersectTime1 = t2;
	}

	if (intersectTime1 > 0 && currentTime> intersectTime1)
	{
		intersections.push_back(intersectTime1 * mLaserRay.GetVelocity() + mLaserRay.GetOrigin());

		++numIntersections;
	}

	if (intersectTime2 > 0 && ((numIntersections > 0 && mCutting) || currentTime > intersectTime2))
	{
		if (mCutting)
		{
			intersections.push_back(intersectTime2 * mLaserRay.GetVelocity() + mLaserRay.GetOrigin());

			++numIntersections;
		}
	}

	return numIntersections > 0;
}

const Vec2D& ShipLaser::GetVelocity() const
{
	return mLaserRay.GetVelocity();
}

float ShipLaser::Scale() const
{
	return 1.0f;
}

bool ShipLaser::DisappearsOnHit() const
{
	return !mCutting;
}