#ifndef __SHIP_LASER_H__
#define __SHIP_LASER_H__

#include "Ray2D.h"
#include "Easings.h"
#include "Color.h"

#include "ShipAmmo.h"

class Screen;
class Vec2D;

class ShipLaser: public ShipAmmo
{
public:

	ShipLaser(const Vec2D& origin, const Vec2D& velocity, const Color& color1, const Color& color2, bool cutting, ease::EasingFunc easingFunction = ease::EaseLinear);


	virtual Type GetType() const override;
	virtual bool IsOutOfBounds(int leftSide, int top, int screenWidth, int screenHeight) const override;
	virtual bool Intersects(const Circle& circle, std::vector<Vec2D>& intersections) const override;
	virtual const Vec2D& GetVelocity() const override;
	virtual float Scale() const override;
	virtual void Update(uint32_t dt) override;
	virtual void Draw(Screen& theScreen) override;
	virtual bool DisappearsOnHit() const override;

private:
	Ray2D mLaserRay;
	Color mColor1;
	Color mColor2;
	uint32_t mTime;
	bool mCutting;
	ease::EasingFunc mEasingFunc;
};

#endif // 
