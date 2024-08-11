#ifndef __SHIP_AMMO_H__
#define __SHIP_AMMO_H__

#include "Vec2D.h"

class Circle;
class Screen;


class ShipAmmo
{
public:

	enum class Type
	{
		MISSILE,
		LASER
	};

    virtual ~ShipAmmo(){}
    
	inline virtual size_t GetID() const 
	{
		return mID;
	}
	virtual void SetID(size_t theID) 
	{
		mID = theID;
	}

	virtual Type GetType() const = 0;
	virtual bool IsOutOfBounds(int leftSide, int top, int screenWidth, int screenHeight) const = 0;
	virtual bool Intersects(const Circle& circle, std::vector<Vec2D>& intersections) const = 0;
	virtual const Vec2D& GetVelocity() const = 0;
	virtual float Scale() const = 0;
	virtual void Update(uint32_t dt) = 0;
	virtual void Draw(Screen& theScreen) = 0;
	virtual bool DisappearsOnHit() const = 0;

private:
	size_t mID;
};

#endif // 
